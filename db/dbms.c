#include <stdio.h>

int cstrlen(char *str)
{
    int idx = 0;
    while (str[idx] != '\0')
        ++idx;
    return idx;
}

char *cstrtok(char *query, char *delim)
{
    char *start = 0; // 문자열 시작 위치
    int i = 0;
    static char *tmp; // 문자열 주소 저장

    if (query != NULL) // 첫 토큰 받을 때
        start = query;
    else // 두 번째 이후 토큰 받을 때
        start = tmp;

    if (cstrlen(start) < 1) // 문자열 종료
        return NULL;

    for (i = 0; i < cstrlen(start); i++)
    {
        if (start[i] == (*delim) || start[i] == '\0')
        {
            start[i] = '\0';
            break;
        }
    }

    tmp = &start[i + 1]; // 다음 배열 시작 위치

    return start;
}

int main(void)
{
    printf("test");
    return 0;
}