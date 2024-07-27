#ifndef SEARCH_TABLE_H
#define SEARCH_TABLE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../db.h"
#include "../table.h"
#include "../domain.h"
#include "../data.h"

#define MAX_COLUMN 20 // 최대 속성 값 개수
#define MAX_INPUT 100 // 최대 입력 값 길이

char op[] = {'<', '>', '=', '!'};
char *search_op[] = {"<", ">", "=", "!"};

void query_select(DB *db, Table *table, Domain *domain, Data *data, FILE *store)
{
    int cnt = 0;               // token count
    int cnt_cols = 0;          // column 개수
    int cnt_cons = 0;          // 조건 개수
    int pos_tname = 0;         // table name 위치
    int pos_cons = 0;          // conditions 시작 위치
    int flag = 0;              // 0: none, 1: and, 2: or
    int flag_empty = 1;        // 0: not empty, 1: empty
    char *columns[MAX_COLUMN]; // 모든 column
    char wheres[MAX_INPUT];    // 조건절
    char *wtokens[MAX_INPUT];  // 모든 조건 token
    char *tokens[MAX_INPUT];   // 모든 token
    char *token = NULL;

    char *col1 = NULL; // 조건1 -> 속성
    char *val1 = NULL; // 조건1 -> 값
    char op1;          // 조건1 -> 연산자
    char *col2 = NULL; // 조건2 -> 속성
    char *val2 = NULL; // 조건2 -> 값
    char op2;          // 조건2 -> 연산자

    wheres[0] = '\0'; // 조건절 초기화

    while ((token = strtok(NULL, ", ;")) != NULL) // Tokenizer
    {
        if (!strcmp(token, "from"))
            pos_tname = cnt;

        if (!strcmp(token, "where"))
            pos_cons = cnt;

        tokens[cnt++] = token;
    }

    for (int i = 0; i < pos_tname; i++) // save columns
    {
        columns[i] = tokens[i];
    }

    ++pos_tname;
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
    }

    cnt_cols = pos_tname - 1; // counting cols

    int result = 0; // 데이터 탐색 결과

    if (db->thead == NULL) // empty table list
    {
        printf("Table '%s' doesn't exist\n", tokens[pos_tname]);
        return;
    }

    table = read_table(db->thead, tokens[pos_tname]); // find table

    if (table == NULL) // not found table
    {
        printf("Table '%s' doesn't exist\n", tokens[pos_tname]);
        return;
    }

    // if (table->cadinality == 0) // insert 0회
    // {
    //     printf("Empty set\n");
    //     return;
    // }

    if (!strcmp(columns[0], "*")) // select all
    {
        domain = table->dhead->next; // Move first column (head next)

        data = domain->head->next; // Move head data

        char str[MAX_INPUT];

        while (!feof(store))
        {
            fgets(str, MAX_INPUT, store);

            printf("%s", str);
        }

        /*while (data != NULL)
        {
            if (pos_cons > 0) // where 문 존재
            {
                if (flag > 0) // 다중 조건
                {
                    result = find_multi_data(table, domain, data, col1, val1, op1, col2, val2, op2, flag);
                }

                else // 단일 조건
                {
                    result = find_single_data(table, domain, data, col1, val1, op1);
                }

                if (result) // 조건에 부합
                {
                    flag_empty = 0;
                    print_tuple(data);
                }
            }

            else // where 문 존재x
            {
                flag_empty = 0;
                print_tuple(data);
            }

            domain = table->dhead->next; // Move first column (head next)
            data = data->next;           // next data (next tuple)
        }*/
    }

    // >>>>>>>>>>>>>>>>>>>>> Select all

    else // select cols
    {
        domain = table->dhead->next; // Move first column (head next)

        data = domain->head->next; // Move head data

        while (data != NULL)
        {
            if (pos_cons > 0) // where 문 존재
            {
                if (flag > 0) // 다중 조건
                {
                    result = find_multi_data(table, domain, data, col1, val1, op1, col2, val2, op2, flag);
                }

                else // 단일 조건
                {
                    result = find_single_data(table, domain, data, col1, val1, op1);
                }

                if (result) // 조건에 부합
                {
                    flag_empty = 0;
                    printf("+--------------------------------------+\n");
                    for (int i = 0; i < cnt_cols; i++) // 조건에 부합하는 Tuple 출력
                    {
                        print_data(domain, data, columns[i]);
                    }
                    printf("\n+--------------------------------------+\n");
                }
            }

            else // where 문 존재x
            {
                flag_empty = 0;
                printf("+--------------------------------------+\n");
                printf("|  ");
                for (int i = 0; i < pos_tname - 1; i++)
                {
                    find_data(domain, data, columns[i]);
                }
                printf("\n+--------------------------------------+\n");
            }

            domain = table->dhead->next; // Move first column (head next)
            data = data->next;           // next data (next tuple)
        }
    }

    // >>>>>>>>>>>>>>>>>>>>> Select cols

    // if (flag_empty == 1)
    // {
    //     printf("Empty set\n");
    // }
}

#endif