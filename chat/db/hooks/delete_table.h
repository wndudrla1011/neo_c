#ifndef DELETE_TABLE_H
#define DELETE_TABLE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include "../db.h"
#include "../table.h"
#include "../domain.h"
#include "../data.h"
#include "./search.h"
#include "../util/directory.h"

void query_delete(char *parent, char *command, int is_where)
{
    char table_dir[MAX_INPUT] = {0};
    int cnt = 0;              // token count
    int cnt_cons = 0;         // 조건 개수
    int flag = 0;             // 0: none, 1: and, 2: or
    char wheres[MAX_INPUT];   // 조건절
    char *wtokens[MAX_INPUT]; // where 뒤의 모든 토큰
    char *tokens[MAX_INPUT];  // 모든 token
    char *token = (char *)malloc(100 * sizeof(char));

    char *col1 = NULL; // 조건1 -> 속성
    char *val1 = NULL; // 조건1 -> 값
    char op1;          // 조건1 -> 연산자
    char *col2 = NULL; // 조건2 -> 속성
    char *val2 = NULL; // 조건2 -> 값
    char op2;          // 조건2 -> 연산자

    wheres[0] = '\0'; // 조건절 초기화

    strcpy(table_dir, read_dir(table->tname, parent));

    char *domain_dir = find_data_dirName(table_dir, 0);

    if (!is_where) // where절 존재x
    {
        int row = 0;
        int limit = get_cnt_dir(domain_dir); // Domain 폴더 내 폴더 개수 == 행 개수

        while (row < limit)
        {
            delete_dir(0, table_dir);

            row++;
        }

        return;
    }

    token = strtok(command, " "); // where

    while ((token = strtok(NULL, ", ;")) != NULL) // Tokenizer
    {
        tokens[cnt++] = token;
    }

    for (int i = 0; i < cnt - 1; i++) // create where clause
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
    wtokens[0] = token; // 조건절 저장
    cnt_cons++;         // 조건절 개수 카운팅

    while ((token = strtok(NULL, " ")) != NULL) // 조건절 저장
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

    int result = 0;                      // 데이터 탐색 결과
    int row = 0;                         // 탐색할 행
    int limit = get_cnt_dir(domain_dir); // Domain 폴더 내 폴더 개수 == 행 개수

    int delete_list[100] = {0}; // 삭제할 데이터 목록 (1: 삭제 대상)

    while (row < limit) // 삭제해야 될 데이터 목록 수집
    {
        if (flag > 0) // 다중 조건
        {
            result = find_multi_dir(row, table_dir, col1, val1, op1, col2, val2, op2, flag);
        }

        else // 단일 조건
        {
            result = find_single_dir(row, table_dir, col1, val1, op1);
        }

        if (result) // 조건에 부합
        {
            delete_list[row] = 1;
        }

        row++;
    }

    for (int i = 0; i < limit; i++) // 삭제 대상 탐색
    {
        if (delete_list[i] == 1) // 삭제 수행
        {
            delete_dir(i, table_dir);
        }
    }

    call_indexing_dir(table_dir, limit); // 삭제 후 인덱싱

    free(token);
}

#endif