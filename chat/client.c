#include <stdio.h>
#include <winsock2.h>
#include <unistd.h>
#define BUFSIZE 1024

void error_handling(char *message);

int main(int argc, char **argv)
{
    int sock = 0;          // socket descriptor
    char message[BUFSIZE]; // storage for messages
    int str_len;
    struct sockaddr_in server_addr; // server socket info

    if (argc != 3)
    {
        printf("Usage : %s <IP>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);

    if (sock == -1)
    {
        error_handling("socket() error");
    }

    // binding values to socket
    memset(&server_addr, 0, sizeof(server_addr)); // allocating memory
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.S_un.S_addr = inet_addr(argv[1]); // dotted decimal -> unsigned long
    server_addr.sin_port = htons(atoi(argv[2]));           // atoi (char -> integer)

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) // request connection to server
    {
        error_handling("connect() error");
    }

    while (1)
    {
        /* Transfer messages */
        fputs("전송할 메시지를 입력하세요. (q to quit) : ", stdout);
        fgets(message, BUFSIZE, stdin);
        if (!strcmp(message, "q\n"))
            break;
        write(sock, message, strlen(message)); // store client's message to sock

        /* Receive messages */
        str_len = read(sock, message, BUFSIZE - 1);
        message[str_len] = 0;
        printf("서버로부터 전송된 메시지 : %s\n", message);
    }

    close(sock); // closing client socket

    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}