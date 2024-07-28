#ifndef INSERT_TABLE_H
#define INSERT_TABLE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../db.h"
#include "../table.h"
#include "../domain.h"
#include "../data.h"

void query_insert(DB *db, Table *table, Domain *domain, Data *data, FILE *store)
{
    int degree = 0;          // 속성 개수
    char *values[MAX_INPUT]; // 토큰화된 입력 데이터
    char *token;
    char *pos;    // 첫 single quote가 등장하는 위치 == 문자열 데이터 시작 위치
    char *unpack; // substring 결과

    while ((token = strtok(NULL, ",);")) != NULL) // 토큰화
    {
        values[degree++] = token;
    }

    table->degree = degree - 1; // 열 개수 입력
    table->cadinality++;        // 행 개수 추가

    for (int i = 0; i < table->degree; i++)
    {
        if ((pos = strstr(values[i], "\'")) != NULL) // 문자열 데이터
        {
            unpack = substring(1, strlen(pos) - 2, pos); // 맨 앞뒤 single quote 제거
            values[i] = unpack;
        }
    }

    // >>>>>>>>>>>>>>>>>>>> Parsing Data

    domain = table->dhead->next; // 첫 번째 column 이동

    for (int i = 0; i < table->degree; i++) // 각 속성에 값 넣기
    {
        if (i == 0) // 첫 열 데이터 넣기
        {
            if (strlen(values[i]) > domain->len) // DCL 정의한 길이보다 긴 경우
            {
                printf("ERROR: Data too long for column '%s'\n", domain->column);
                exit(1);
            }
            fprintf(store, "%s,", values[i]);
            add_bottom_data(domain->head, values[i]);
            domain = domain->next;
        }
        else // 나머지 열 데이터 넣기
        {
            if (strlen(values[i]) > domain->len) // DCL 정의한 길이보다 긴 경우
            {
                printf("ERROR: Data too long for column '%s'\n", domain->column);
                exit(1);
            }
            if (i == table->degree - 1)
                fprintf(store, "%s", values[i]);
            else
                fprintf(store, "%s,", values[i]);
            add_right_data(find_bottom_data(domain->head), values[i]); // 데이터 추가 (열 방향)
            domain = domain->next;
        }
    }

    fputs("\n", store);

    // >>>>>>>>>>>>>>>>>>>>> Insert Data
}

#endif