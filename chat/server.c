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
#define MAX_CLNT 256

void error_handling(char *message);
void *handle_client(void *arg);
void send_msg(char *msg, int len);

int client_cnt = 0;
int client_socks[MAX_CLNT];
pthread_mutex_t mtx;

int main(int argc, char **argv)
{
    int server_sock;                // server socket descriptor
    int client_sock;                // client socket descriptor
    struct sockaddr_in server_addr; // 서버 소켓 정보
    struct sockaddr_in client_addr; // 클라이언트 소켓 정보
    int client_addr_size;
    // socklen_t client_addr_size; -> Linux
    pthread_t tid;

    if (argc != 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    pthread_mutex_init(&mtx, NULL);
    server_sock = socket(AF_INET, SOCK_STREAM, 0); // 소켓 생성

    if (server_sock == -1)
    {
        error_handling("socket() error");
    }

    // 소켓 값 바인딩
    memset(&server_addr, 0, sizeof(server_addr)); // 서버 소켓 메모리 할당
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1])); // atoi (char -> integer)

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) // 서버 소켓 바인딩
    {
        error_handling("bind() error");
    }

    if (listen(server_sock, 5) == -1) // 클라이언트 요청 대기
    {
        error_handling("listen() error");
    }

    while (1)
    {
        client_addr_size = sizeof(client_addr);
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_size); // 클라이언트 요청 수락
        if (client_sock == -1)
        {
            error_handling("accept() error");
        }

        pthread_mutex_lock(&mtx);
        client_socks[client_cnt++] = client_sock;
        pthread_mutex_unlock(&mtx);

        pthread_create(&tid, NULL, handle_client, (void *)&client_sock);
        pthread_detach(tid);
        printf("Connected client IP : %s\n", inet_ntoa(client_addr.sin_addr));
    }

    close(server_sock); // 서버 소켓 Close

    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

void *handle_client(void *arg)
{
    int client_sock = *((int *)arg);
    int str_len = 0;
    char msg[BUF_SIZE];

    // 클라이언트가 close를 날린다면 EOF가 도달해서 read 함수가 0을 반환한다
    while (str_len = read(client_sock, msg, sizeof(msg)) != 0)
    {
        send_msg(msg, str_len);
    }

    // 클라이언트
    pthread_mutex_lock(&mtx);
    for (int i = 0; i < client_cnt; i++)
    {
        if (client_sock == client_socks[i])
        {
            while (++i < client_cnt)
            {
                client_socks[i - 1] = client_socks[i];
            }
            break;
        }
    }
    client_cnt--;
    pthread_mutex_unlock(&mtx);
    close(client_sock);

    return NULL;
}

void send_msg(char *msg, int len)
{
    pthread_mutex_lock(&mtx);
    for (int i = 0; i < client_cnt; i++)
    {
        write(client_socks[i], msg, len);
    }
    pthread_mutex_unlock(&mtx);
}