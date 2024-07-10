#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define BUF_SIZE 100
#define NAME_SIZE 20

void *send_msg(void *arg);
void *recv_msg(void *arg);

char msg[BUF_SIZE];
char name[NAME_SIZE] = "[DEFAULT]"; // 채팅창에 보여질 이름의 형태(20자 제한)
char logout[] = "님이 로그아웃했습니다.\n";

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    pthread_t send_thread, recv_thread; // 송신 스레드, 수신 스레드
    void *thread_return;                // pthread_join에 사용
    int str_len;

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

    // 송신과 수신을 수행할 두 스레드 생성
    // 연결 요청 대상인 서버는 동일하므로 매개변수는 sock으로 동일
    pthread_create(&send_thread, NULL, send_msg, (void *)&sock);
    pthread_create(&recv_thread, NULL, recv_msg, (void *)&sock);

    // 스레드 종료 대기
    pthread_join(send_thread, NULL);
    pthread_join(recv_thread, NULL);

    close(sock); // 클라이언트 연결 종료

    return 0;
}

void *send_msg(void *arg)
{
    int sock = *((int *)arg);            // void descriptor -> int 변환
    char name_msg[NAME_SIZE + BUF_SIZE]; // 사용자 ID와 메시지를 합칠 것임
    char logout_msg[NAME_SIZE + strlen(logout)];

    while (1)
    {
        fgets(msg, BUF_SIZE, stdin); // 사용자 입력을 msg에 저장

        if (!strcmp(msg, "exit\n"))
        {
            sprintf(logout_msg, "%s %s", name, logout);
            write(sock, logout_msg, sizeof(logout_msg));

            close(sock); // 서버에 EOF 보냄
            exit(1);
        }

        // 생성된 name_msg를 출력
        sprintf(name_msg, "%s %s", name, msg); // ID가 joo이고 메시지가 "Hi" 라면, [joo] Hi

        write(sock, name_msg, sizeof(name_msg)); // 서버로 채팅을 보냄
    }

    return NULL;
}

void *recv_msg(void *arg)
{
    int sock = *((int *)arg);            // void descriptor -> int 변환
    char name_msg[NAME_SIZE + BUF_SIZE]; // 사용자 ID와 메시지를 합칠 것임
    int str_len = 0;

    while (1)
    {
        str_len = read(sock, name_msg, sizeof(name_msg) - 1); // 서버에서 들어온 메시지 수신

        /*
        str_len 이 -1이라는 것은 서버 소켓과 연결이 끊어졌다는 것
        send_msg close(sock) -> EOF -> server close(clnt_sock)
        그러면 read 결과가 -1이 나옴
        */
        if (str_len == -1)
        {
            // 종료를 위한 리턴값, thread_return으로 갈 것
            return (void *)-1; // pthread_join을 실행시키기 위함
        }

        name_msg[str_len] = 0; // 버퍼 맨 마지막 값 NULL

        fputs(name_msg, stdout); // 받은 메시지 출력 (서버에서 write 한 메시지)
    }

    return NULL;
}