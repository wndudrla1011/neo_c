#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <time.h>
#include "./db/dbms.h"
#include "./db/util/directory.h"
#include "./db/hooks/insert_table.h"

#define BUF_SIZE 1024
#define MAX_CLNT 256
#define QUERY_SIZE 100

void *handle_clnt(void *arg);
void send_msg(char *msg, int len);

void send_msg_me(int clnt_sock, char *msg, int len);
void error_handling(char *message);

int clnt_cnt = 0; // 접속한 클라이언트 수
/*
여러 명의 클라이언트가 접속하므로 클라이언트 소켓은 배열
멀티스레드 시, clnt_cnt와 clnt_socks 에 여러 스레드가 접속할 수 있기 때문에
두 변수를 사용하는 영역은 임계 영역
*/
int clnt_socks[MAX_CLNT];
pthread_mutex_t mtx; // mutex 선언 (스레드끼리 전역변수 동시 사용 방지)
char login[] = "로그인을 완료하였습니다. 로그아웃 명령은 'exit' 입니다.\n";
time_t now;          // PK
struct tm *t;        // 시간 구조체
char *log[BUF_SIZE]; // log
int cnt = 0;

int main(int argc, char *argv[])
{
    int serv_sock, clnt_sock; // serv_sock(듣기 소켓), clnt_sock(연결 소켓)
    struct sockaddr_in serv_addr, clnt_addr;
    pthread_t tid; // thread 선언
    socklen_t clnt_addr_size;

    // JOOSQL
    if (joosql_init("user1", "0000") == -1) // DB 접속
    {
        exit(1);
    }

    if (joosql_connect() != 0)
    {
        printf("Failed to enter DB\n");
        exit(1);
    }

    // 소켓 옵션 설정을 위한 변수들
    int option = 0;
    socklen_t optlen;

    printf("read port....\n");
    if (argc != 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    printf("set server socket\n");

    pthread_mutex_init(&mtx, NULL); // mutex 생성

    if ((serv_sock = socket(PF_INET, SOCK_STREAM, 0)) == -1) // 듣기 소켓 생성
    {
        error_handling("socket() error");
    }

    // Time-wait 해제
    // SO_REUSEADDR을 0에서 1로 변경
    optlen = sizeof(option);
    option = 1;
    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, (void *)&option, optlen);

    // 주소 정보 바인딩
    printf("set server addr...\n");
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    printf("binding...\n");
    if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) // 소켓과 주소 정보 결합
    {
        error_handling("bind() error");
    }

    // 5는 큐의 크기일 뿐이다. 운영체제가 여유가 된다면 알아서 accept 할 것
    // 즉, 총 256명까지 접속 가능한 것.
    // 웹 서버같이 수 천명의 클라이언트로 바쁠 경우, 15로 잡는 경우가 보통
    if (listen(serv_sock, 5) == -1)
    {
        error_handling("listen() error");
    }

    printf("waiting...\n");

    while (1)
    {
        clnt_addr_size = sizeof(clnt_addr);
        // clnt_addr -> 연결된 클라이언트의 주소 정보
        if ((clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size)) < 0)
        {
            error_handling("accept() error");
        }

        printf("[NOW] accept\n");

        pthread_mutex_lock(&mtx);           // 전역 변수 사용을 위해 mutex 락
        clnt_socks[clnt_cnt++] = clnt_sock; // 소켓 배정
        pthread_mutex_unlock(&mtx);         // mutex 언락

        pthread_create(&tid, NULL, handle_clnt, (void *)&clnt_sock); // handle_clnt를 작업하는 스레드 생성

        pthread_detach(tid); // 해당 스레드 분리

        printf("accepted host(IP: %s, Port: %d)\n", inet_ntoa(clnt_addr.sin_addr), ntohs(serv_addr.sin_port));
    }

    close(serv_sock);

    return 0;
}

void *handle_clnt(void *arg)
{
    int clnt_sock = *((int *)arg); // void descriptor -> int 변환
    int str_len = 0;
    char msg[BUF_SIZE] = {0};
    char query[QUERY_SIZE + BUF_SIZE] = {0};
    char pk[QUERY_SIZE] = {0};

    send_msg_me(clnt_sock, login, strlen(login));

    /*
    클라이언트에서 보낸 메시지 받음
    클라이언트에서 EOF를 보내 str_len 이 0이 될때까지 반복
    EOF는 클라이언트에서 소켓을 close 했을 때 보냄
    즉, 클라이언트가 접속을 하고 있는 동안에는 while 문을 벗어나지 않는다.
    */
    while ((str_len = read(clnt_sock, msg, sizeof(msg))) != 0)
    {
        send_msg(msg, str_len); // 접속한 모두에게 메시지 보내기

        // escape single quote
        for (int i = 0; i < str_len; i++)
        {
            if (msg[i] == 39)
            {
                for (int j = str_len; j > i; j--)
                {
                    msg[j] = msg[j - 1];
                }
                i += 2;
            }
        }

        now = time(NULL); // 현재 시간

        sprintf(pk, "%ld", now);
        sprintf(query, "INSERT INTO chat VALUES('%s', '%s')", pk, msg); // 채팅 쿼리 생성

        joosql_insert(pk, msg);
    }

    // while 문을 탈출 -> 현재 담당하는 소켓의 연결이 끊김

    pthread_mutex_lock(&mtx); // 전역 변수 사용을 위해 mutex 락
    // 현재 스레드에서 담당하는 소켓(disconnected) 삭제
    for (int i = 0; i < clnt_cnt; i++)
    {
        if (clnt_sock == clnt_socks[i]) // 현재 담당하는 클라이언트 소켓의 descriptor를 찾으면
        {
            while (i++ < clnt_cnt - 1) // 해당 위치부터 클라이언트 소켓 1칸씩 당기기
                clnt_socks[i] = clnt_socks[i + 1];

            break;
        }
    }

    clnt_cnt--;                 // 클라이언트 수 감소
    pthread_mutex_unlock(&mtx); // mutex 언락

    close(clnt_sock); // 서버의 스레드에서 담당하는 클라이언트 소켓 종료

    return NULL;
}

// 접속한 모두에게 메시지 보내기
void send_msg(char *msg, int len)
{
    pthread_mutex_lock(&mtx); // 전역 변수 사용을 위해 mutex 락
    for (int i = 0; i < clnt_cnt; i++)
        write(clnt_socks[i], msg, len); // 모든 클라이언트 소켓에 메시지 전달

    pthread_mutex_unlock(&mtx); // mutex 언락
}

// 접속한 대상에만 메시지 보내기
void send_msg_me(int clnt_sock, char *msg, int len)
{
    write(clnt_sock, msg, len); // 접속 클라이언트 소켓에 메시지 전달
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}