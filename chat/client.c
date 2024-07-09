#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define BUF_SIZE 100
#define NAME_SIZE 30

char msg[BUF_SIZE];
char name[NAME_SIZE] = "[DEFAULT]";

typedef struct recieve_data
{
    char *message;
    int *serv_sock;
} recieve_data;

void *send_msg(void *serv_sock)
{
    int *cs = (int *)serv_sock;
    while (1)
    {
        printf("\n[%s] client -> : ", name);
        fgets(msg, BUF_SIZE, stdin);
        write(*cs, msg, sizeof(msg));
    }
}

void *recv_msg(void *rcvDt)
{

    recieve_data *data = (recieve_data *)rcvDt;
    char *msg = data->message;
    while (1)
    {
        int str_len = read(*(data->serv_sock), msg, sizeof(char) * 30);
        if (str_len != -1)
        {
            printf("\n <- server : %s\n", msg);
        }
    }
}

int main(int argc, char *argv[])
{

    int serv_sock;
    struct sockaddr_in serv_addr;
    int str_len;

    if (argc != 4)
    {
        printf("Usage : %s <IP> <port> <Name>\n", argv[0]);
        exit(1);
    }

    sprintf(name, "%s", argv[3]);
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    connect(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    pthread_t p_thread[2];
    int t;
    int status;
    recieve_data rcvDt;
    rcvDt.message = msg;
    rcvDt.serv_sock = &serv_sock;

    for (t = 0; t < 2; t++)
    {
        if (t == 0)
            pthread_create(&p_thread[t], NULL, send_msg, (void *)&serv_sock);
        else if (t == 1)
            pthread_create(&p_thread[t], NULL, recv_msg, (void *)&rcvDt);
    }

    pthread_join(p_thread[0], (void **)&status);
    pthread_join(p_thread[1], (void **)&status);

    close(serv_sock);
    return 0;
}