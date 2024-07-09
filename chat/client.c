#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <winsock2.h>
#include <unistd.h>
/* Linux
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/un.h>
*/
#include <pthread.h>
#define BUF_SIZE 100
#define NAME_SIZE 20

void error_handling(char *message);
void *send_msg(void *arg);
void *recv_msg(void *arg);

char name[NAME_SIZE] = "[DEFAULT]";
char msg[BUF_SIZE];

int main(int argc, char **argv)
{
    int sock = 0;                   // socket descriptor
    struct sockaddr_in server_addr; // 서버 소켓 정보
    pthread_t send_thread, recv_thread;

    if (argc != 4)
    {
        printf("Usage : %s <IP> <PORT> <NAME>\n", argv[0]);
        exit(1);
    }

    sprintf(name, "[%s]", argv[3]);
    sock = socket(AF_INET, SOCK_STREAM, 0); // 소켓 생성

    if (sock == -1)
    {
        error_handling("socket() error");
    }

    // 소켓 값 생성
    memset(&server_addr, 0, sizeof(server_addr)); // 서버 소켓 메모리 할당
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.S_un.S_addr = inet_addr(argv[1]); // dotted decimal -> unsigned long
    server_addr.sin_port = htons(atoi(argv[2]));           // atoi (char -> integer)

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) // 서버에 연결 요청
    {
        error_handling("connect() error");
    }

    pthread_create(&send_thread, NULL, send_msg, (void *)&sock);
    pthread_create(&recv_thread, NULL, recv_msg, (void *)&sock);
    pthread_join(send_thread, NULL);
    pthread_join(recv_thread, NULL);

    close(sock); // 클라이언트 소켓 Close

    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

void *send_msg(void *arg)
{
    int sock = *((int *)arg);
    char name_msg[NAME_SIZE + BUF_SIZE];
    while (1)
    {
        fgets(msg, BUF_SIZE, stdin);
        if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n"))
        {
            close(sock);
            // exit(0)가 아닌 return NULL로 하면 아직 recv_msg 스레드가 살아있어서 바로 종료되지 않는다
            exit(0);
        }
        sprint(name_msg, "%s %s", name, msg);
        write(sock, name_msg, strlen(name_msg));
    }

    return NULL;
}

void *recv_msg(void *arg)
{
    int sock = *((int *)arg);
    char name_msg[NAME_SIZE + BUF_SIZE];
    int str_len;
    while (1)
    {
        str_len = read(sock, name_msg, sizeof(name_msg) - 1);
        if (str_len < 0)
        {
            return NULL;
        }
        name_msg[str_len] = 0;
        fputs(name_msg, stdout);
    }

    return NULL;
}