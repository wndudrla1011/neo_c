#ifndef CREATE_TABLE_H
#define CREATE_TABLE_H

#include "../util/getLen.h"
#include "../db.h"
#include "../table.h"
#include "../data.h"
#include <string.h>

char *types[] = {"int", "INT", "bigint", "BIGINT", "varchar", "VARCHAR", "text", "TEXT"};

void create_table(char *name, DB *db, Table *table, Domain *domain, Data *data)
{
    add_table(db, table, name); // 연결 리스트 -> New Table

    char *attr_info[MAX_INPUT]; // create table 을 Tokenizer 한 결과 저장
    char *column = NULL, *type = NULL, *nullable = NULL;

    int cnt = 0;       // create token 개수
    int flag = 1;      // NOT NULL, NULL 구분용
    int type_flag = 0; // 현재 데이터가 type인지 여부
    int len = 0;       // 데이터 길이
    char *token;       // 데이터 도메인 토큰
    char *type_token;  // 타입 + 길이

    while ((token = strtok(NULL, ", );")) != NULL) // 토큰 저장
    {
        attr_info[cnt++] = token;
    }

    for (int i = 0; i < cnt; i++) // 저장된 토큰 읽기
    {
        for (int j = 0; j < sizeof(types) / sizeof(types[0]); j++)
        {
            if (strstr(attr_info[i], types[j]) != NULL) // 데이터 타입 목록과 일치
            {
                type_token = strtok(attr_info[i], "("); // 데이터 타입
                type = type_token;

                len = getLen(type_token, types[j]); // 타입에 맞는 데이터 길이를 구함

                type_flag = 1; // 현재 토큰은 타입
                break;
            }
        }

        if (!strcasecmp(attr_info[i], "NOT") || !strcasecmp(attr_info[i], "not"))
        {
            flag = 0;
            continue;
        }

        if (!strcasecmp(attr_info[i], "NULL") || !strcasecmp(attr_info[i], "null"))
        {
            if (flag == 0) // NOT NULL
                nullable = "NOT NULL";
            else // NULL
                nullable = "NULL";

            // Domain 1개 종료

            flag = 1, type_flag = 0; // flag 초기화

            add_domain(domain, column, type, len, nullable); // Domain 추가

            continue;
        }

        if (type_flag == 0) // 현재 토큰이 타입이 아님
            column = attr_info[i];
    }
}

#endif