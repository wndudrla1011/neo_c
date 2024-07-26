#ifndef HANDLE_BRANKET_H
#define HANDLE_BRANKET_H

#include <stdio.h>
#include <string.h>

void handle_branket(char *input)
{
    int cnt_open_branket;      // '(' 개수
    int cnt_close_branket;     // ')' 개수
    char *pos_open_branket;    // '(' 위치
    char *valid_close_branket; // ')' 위치

    pos_open_branket = strstr(input, "(");

    while (pos_open_branket != NULL) // '(' 찾기
    {
        cnt_open_branket++; // '(' 카운팅
        pos_open_branket = strstr(input, "(");
    }

    valid_close_branket = strstr(input, ")");

    while (valid_close_branket != NULL) // ')' 찾기
    {
        cnt_close_branket++; // ')' 카운팅
        valid_close_branket = strstr(input, ")");
    }

    if (cnt_open_branket % 2 > 0 || cnt_close_branket % 2 > 0) // 유효 여부 판단
    {
        printf("You have an error in your SQL syntax;\n");
    }
}

#endif