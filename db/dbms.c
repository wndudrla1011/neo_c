#include <stdio.h>
#include <string.h>

#define QUERY_SIZE 100

int main(void)
{
    char query[QUERY_SIZE]; // 입력 쿼리
    char *delim = " ";      // delimiter
    char *token;

    fgets(query, QUERY_SIZE, stdin); // 쿼리 저장

    token = strtok(query, delim);

    while (token != NULL)
    {
        printf("token: %s\n", token);
        token = strtok(NULL, delim);
    }

    return 0;
}