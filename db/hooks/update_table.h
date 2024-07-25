#ifndef UPDATE_TABLE_H
#define UPDATE_TABLE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../db.h"
#include "../table.h"
#include "../domain.h"
#include "../data.h"

#define MAX_COLUMN 20 // 최대 속성 값 개수
#define MAX_INPUT 100 // 최대 입력 값 길이

void query_update(DB *db, Table *table, Domain *domain, Data *data)
{
    int cnt = 0;                 // token count
    int cnt_cons = 0;            // 조건 개수
    int pos_cons = 0;            // conditions 시작 위치
    int flag = 0;                // 0: none, 1: and, 2: or
    char *set_tokens[MAX_INPUT]; // set 뒤의 모든 토큰
    char wheres[MAX_INPUT];      // 조건절
    char *wtokens[MAX_INPUT];    // where 뒤의 모든 토큰
    char *tokens[MAX_INPUT];     // 모든 token
    char *token = NULL;

    char *col1 = NULL; // 조건1 -> 속성
    char *val1 = NULL; // 조건1 -> 값
    char op1;          // 조건1 -> 연산자
    char *col2 = NULL; // 조건2 -> 속성
    char *val2 = NULL; // 조건2 -> 값
    char op2;          /// 조건2 -> 연산자

    wheres[0] = '\0'; // 조건절 초기화

    if (table->cadinality == 0) // insert 0회
    {
        printf("Empty set\n");
        return;
    }

    while ((token = strtok(NULL, ", ;")) != NULL) // Tokenizer
    {
        if (!strcmp(token, "where"))
        {
            pos_cons = cnt;
        }

        tokens[cnt++] = token;
    }

    free(token);
    token = NULL;

    if (pos_cons > 0) // where문 존재
    {
        for (int i = pos_cons + 1; i < cnt - 1; i++) // create where clause
        {
            if (!strcmp(tokens[i], "or") || !strcmp(tokens[i], "and"))
            {
                strcat(wheres, " ");
                strcat(wheres, tokens[i]);
                strcat(wheres, " ");
            }
            else
            {
                strcat(wheres, tokens[i]);
            }
        }

        token = strtok(wheres, " ");
        wtokens[0] = token;
        cnt_cons++;

        while ((token = strtok(NULL, " ")) != NULL)
        {
            if (!strcmp(token, "or"))
                flag = 2;
            else if (!strcmp(token, "and"))
                flag = 1;

            wtokens[cnt_cons++] = token;
        }

        if (flag > 0) // 조건문 2개
        {
            for (int i = 0; i < sizeof(search_op) / sizeof(char *); i++)
            {
                if (strstr(wtokens[0], search_op[i]) != NULL)
                {
                    op1 = op[i];
                    col1 = strtok(wtokens[0], search_op[i]);
                    val1 = strtok(NULL, search_op[i]);
                }

                if (strstr(wtokens[2], search_op[i]) != NULL)
                {
                    op2 = op[i];
                    col2 = strtok(wtokens[2], search_op[i]);
                    val2 = strtok(NULL, search_op[i]);
                }
            }
        }

        else // 조건문 1개
        {
            for (int i = 0; i < sizeof(search_op) / sizeof(char *); i++)
            {
                if (strstr(wtokens[0], search_op[i]) != NULL)
                {
                    op1 = op[i];
                    col1 = strtok(wtokens[0], search_op[i]);
                    val1 = strtok(NULL, search_op[i]);
                }
            }
        }

        // >>>>>>>>>>>>>>>>>>>>> Parsing where
    }
}

#endif