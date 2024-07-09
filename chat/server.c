#include <stdio.h>
#include <winsock2.h>
#include <unistd.h>
#define BUFSIZE 1024

void error_handling(char *message);

int main(int argc, char **argv)
{
    int server_sock;       // server socket descriptor
    int client_sock;       // client socket descriptor
    char message[BUFSIZE]; // storage for messages
    int str_len;
    struct sockaddr_in server_addr; // server socket info
    struct sockaddr_in client_addr; // client socket info
    int client_addr_size;

    if (argc != 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    server_sock = socket(PF_INET, SOCK_STREAM, 0); // creating a socket

    if (server_sock == -1)
    {
        error_handling("socket() error");
    }

    // binding values to socket
    memset(&server_addr, 0, sizeof(server_addr)); // allocating memory
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1])); // atoi (char -> integer)

    if (bind(server_sock, (struct socketaddr *)&server_addr, sizeof(server_addr)) == -1) // bind socket and port
    {
        error_handling("bind() error");
    }

    if (listen(server_sock, 5) == -1) // waiting for client's request, q size is 5
    {
        error_handling("listen() error");
    }

    client_addr_size = sizeof(client_addr);
    client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_size); // accept client's request

    if (client_sock == -1)
    {
        error_handling("accept() error");
    }

    /* Data send/recv */
    while ((str_len = read(client_sock, message, BUFSIZE)) != 0)
    {
        write(client_sock, message, str_len); // store client's message to client_sock
        write(1, message, str_len);
    }

    close(client_sock); // closing client socket

    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}