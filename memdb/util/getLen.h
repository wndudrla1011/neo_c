#ifndef GETLEN_H
#define GETLEN_H

#include <string.h>

int getLen(char *type_token, char *type)
{
    int len = 0; // 데이터 길이
    int i = 0;   // length를 숫자로 변환하기 위한 index

    if (!strcmp(type, "text") || !strcmp(type, "TEXT"))
        len = 65533;

    else if (!strcmp(type, "bigint") || !strcmp(type, "BIGINT"))
        len = 19; // -18자리 ~ 19자리

    else
    {
        type_token = strtok(NULL, " "); // 지정한 길이

        if (type_token == NULL) // Case: 데이터 길이를 지정x
        {
            if (!strcmp(type, "int") || !strcmp(type, "INT")) // int
            {
                len = 10; // -9자리 ~ 10자리
                return len;
            }

            else if (!strcmp(type, "varchar") || !strcmp(type, "VARCHAR")) // varchar
            {
                len = 65535;
                return len;
            }
        }

        else // Case: 데이터 길이를 직접 지정
        {
            while (type_token[i] != '\0')
            {
                len = len * 10 + (type_token[i] - 48);
                i++;
            }
        }
    }

    return len;
}

#endif