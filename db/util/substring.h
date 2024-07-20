#ifndef SUBSTRING_H
#define SUBSTRING_H

#include <stdlib.h>
#include <string.h>

char *substring(int s, int e, char *str) //s: 시작 위치, e: 끝 위치, str: 작업할 문자열
{
    char *unpack = (char *)malloc(sizeof(char) * (e - s + 2));
    strncpy(unpack, str + s, e - s + 1); // s 부터 (s ~ e) 길이만큼 저장

    return (unpack);
}

#endif