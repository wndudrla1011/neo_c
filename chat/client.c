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
char name_msg[NAME_SIZE + BUF_SIZE];

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
        printf("\n%s client -> : ", name);
        fgets(msg, BUF_SIZE, stdin);

        if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n"))
        {
            close(*cs);
            exit(1);
        }

        sprintf(name_msg, "%s %s", name, msg);
        write(*cs, name_msg, sizeof(name_msg));
    }
}

void *recv_msg(void *rcvDt)
{

    recieve_data *data = (recieve_data *)rcvDt;
    char *msg = data->message;

    while (1)
    {
        int str_len = read(*(data->serv_sock), msg, sizeof(char) * BUF_SIZE);

        if (str_len != -1)
        {
            printf("\n%s", name_msg);
        }
    }
}

int main(int argc, char *argv[])
{

    int serv_sock;
    int status;
    struct sockaddr_in serv_addr;
    int str_len;
    pthread_t send_thread, recv_thread;

    if (argc != 4)
    {
        printf("Usage : %s <IP> <port> <Name>\n", argv[0]);
        exit(1);
    }

    sprintf(name, "[%s]", argv[3]);
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    connect(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    recieve_data rcvDt;
    rcvDt.message = msg;
    rcvDt.serv_sock = &serv_sock;

    pthread_create(&send_thread, NULL, send_msg, (void *)&serv_sock);
    pthread_create(&recv_thread, NULL, recv_msg, (void *)&rcvDt);

    pthread_join(send_thread, (void **)&status);
    pthread_join(recv_thread, (void **)&status);

    close(serv_sock);
    return 0;
}