#ifndef INSERT_TABLE_H
#define INSERT_TABLE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../db.h"
#include "../table.h"
#include "../domain.h"
#include "../data.h"
#include "../util/directory.h"

void query_insert(char *table_dir, DB *db, Table *table, char *command)
{
    int row;                 // 순서에 맞는 행 번호
    int degree = 0;          // 속성 개수
    char *values[MAX_INPUT]; // 토큰화된 입력 데이터
    char *pos = NULL;        // 첫 single quote가 등장하는 위치 == 문자열 데이터 시작 위치
    char *unpack = NULL;     // substring 결과
    char *token = (char *)malloc(100 * sizeof(char));

    char *domain_dir = (char *)malloc(100 * sizeof(char));
    domain_dir = find_data_dirName(table_dir, 0);
    row = get_cnt_dir(domain_dir);

    token = strtok(command, "("); // values

    while ((token = strtok(NULL, ",);")) != NULL) // 토큰화
    {
        values[degree++] = token;
    }

    table->degree = degree - 1; // 열 개수 입력 (마지막 NULL 값 제외)
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

    for (int i = 0; i < table->degree; i++) // 각 속성에 값 넣기
    {
        save_data_dir(table_dir, i, row, values[i]);
    }

    // >>>>>>>>>>>>>>>>>>>>> Insert Data

    free(token);
}

#endif