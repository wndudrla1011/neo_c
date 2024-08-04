#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <time.h>
#include <mysql/mysql.h>
#include "./db/dbms.h"

#define BUF_SIZE 100
#define NAME_SIZE 30
#define TIME_SIZE 100

void *send_msg(void *arg);
void *recv_msg(void *arg);
void finish_with_error(MYSQL *con);

char msg[BUF_SIZE];
char name[NAME_SIZE] = "[DEFAULT]"; // 채팅창에 보여질 이름의 형태(20자 제한)
char logout[] = "님이 로그아웃했습니다.\n";
MYSQL *con;                   // SQL connection
MYSQL_RES *sql_result = NULL; // SQL 응답
MYSQL_ROW sql_row;            // SQL 결과 배열

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    pthread_t send_thread, recv_thread; // 송신 스레드, 수신 스레드
    void *thread_return;                // pthread_join에 사용
    int str_len;

    // MYSQL
    con = mysql_init(NULL);

    if (con == NULL)
    {
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);
    }

    if (mysql_real_connect(con, "localhost", "user1", "0000", "chatdb", 3306, NULL, 0) == NULL)
    {
        finish_with_error(con);
    }

    if (joosql_init("user1", "0000") == -1) // DB 접속
    {
        exit(1);
    }

    if (joosql_connect("db", "t1") != 0)
    {
        printf("Failed to enter DB\n");
        exit(1);
    }

    if (argc != 4)
    {
        printf("Usage : %s <IP> <port> <Name>\n", argv[0]);
        exit(1);
    }

    sprintf(name, "[%s]", argv[3]); // ex) 입력 값이 joo라면, name = [joo]

    sock = socket(PF_INET, SOCK_STREAM, 0); // 소켓 생성

    // 연결할 서버 정보 할당
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    // 서버에 연결 요청
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("connect");
        return -1;
    }

    if (mysql_query(con, "SELECT * FROM CHAT ORDER BY date DESC LIMIT 10") != 0) // 최근 10개 레코드 조회
        finish_with_error(con);

    sql_result = mysql_store_result(con); // 쿼리 결과 호출

    while ((sql_row = mysql_fetch_row(sql_result)) != NULL)
    {
        printf("%s : %s\n", sql_row[0], sql_row[1]);
    }

    // 송신과 수신을 수행할 두 스레드 생성
    // 연결 요청 대상인 서버는 동일하므로 매개변수는 sock으로 동일
    pthread_create(&send_thread, NULL, send_msg, (void *)&sock);
    pthread_create(&recv_thread, NULL, recv_msg, (void *)&sock);

    // 스레드 종료 대기
    pthread_join(send_thread, NULL);
    pthread_join(recv_thread, NULL);

    close(sock); // 클라이언트 연결 종료

    mysql_free_result(sql_result); // SQL 응답 포인터 해제

    return 0;
}

void *send_msg(void *arg)
{
    int sock = *((int *)arg);                        // void descriptor -> int 변환
    char name_msg[TIME_SIZE + NAME_SIZE + BUF_SIZE]; // 사용자 ID와 메시지를 합칠 것임
    char logout_msg[NAME_SIZE + strlen(logout)];     // 사용자 ID와 로그아웃 메시지를 합칠 것임
    char local_date_time[TIME_SIZE];                 // 포맷팅한 시간 정보
    time_t now = time(NULL);                         // 현재 시간
    struct tm *t = localtime(&now);                  // 시간 포맷팅

    while (1)
    {
        fgets(msg, BUF_SIZE, stdin); // 사용자 입력을 msg에 저장

        asctime_r(t, local_date_time); // 현재 시간 갱신

        if (!strcmp(msg, "exit\n"))
        {
            sprintf(logout_msg, "%s %s", name, logout);  // (사용자 ID + 로그아웃 메시지) 를 버퍼에 저장
            write(sock, logout_msg, sizeof(logout_msg)); // 서버로 전송하여 모든 클라이언트에 뿌림

            close(sock); // 서버에 EOF 보냄
            exit(1);
        }

        // 생성된 name_msg를 출력
        sprintf(name_msg, "%s > %s %s", local_date_time, name, msg); // ID가 joo이고 메시지가 "Hi" 라면, [joo] Hi

        write(sock, name_msg, sizeof(name_msg)); // 서버로 채팅을 보냄
    }

    return NULL;
}

void *recv_msg(void *arg)
{
    int sock = *((int *)arg);                        // void descriptor -> int 변환
    char name_msg[TIME_SIZE + NAME_SIZE + BUF_SIZE]; // 사용자 ID와 메시지를 합칠 것임
    int str_len = 0;

    while (1)
    {
        str_len = read(sock, name_msg, sizeof(name_msg) - 1); // 서버에서 들어온 메시지 수신

        name_msg[str_len] = 0; // 버퍼 맨 마지막 값 NULL

        fputs(name_msg, stdout); // 받은 메시지 출력 (서버에서 write 한 메시지)
    }

    return NULL;
}

void finish_with_error(MYSQL *con)
{
    fprintf(stderr, "%s \n", mysql_error(con));
    mysql_close(con);
    exit(1);
}