#ifndef DATA_H
#define DATA_H

#include <stdio.h>
#include <stdlib.h>
#include "table.h"
#include "domain.h"
#include "./util/substring.h"

#define MAX_INPUT 100 // 최대 입력 값 길이

typedef struct Data
{
    char value[MAX_INPUT]; // 해당 Domain에 해당하는 데이터
    struct Data *tuple;    // 다음 열 데이터
    struct Data *next;     // 다음 행 데이터
} Data;

Data *init_data(Domain *domain)
{
    Data *head;
    head = (Data *)malloc(sizeof(Data));
    head->tuple = NULL;
    head->next = NULL;
    domain->head = head;

    return (head);
}

int data_len(Data *h)
{
    int cnt;
    Data *cur;
    cur = h;

    cnt = 0;
    while (cur->next != NULL)
    {
        cnt++;
        cur = cur->next;
    }

    return cnt;
}

void *find_data(Domain *domain, Data *data, char *column) // 조건이 없는 Tuple 검색
{
    while (strcmp(domain->column, column))
    {
        data = data->tuple;
        domain = domain->next;
    }

    printf("%s  |  ", data->value);
}

/*
 * 단일 조건에 맞는 Tuple 검색
 * col: 조건 컬럼
 * val: 조건 값
 * op: 연산자
 */
int find_single_data(Table *table, Domain *domain, Data *data, char *col, char *val, char op)
{
    int result = 0; // 조건 부합 => 1

    while (strcmp(domain->column, col)) // 검사해야 할 컬럼
    {
        data = data->tuple;
        domain = domain->next;
    }

    if (strstr(domain->type, "int")) // 숫자 타입
    {
        int limit = atoi(val);        // 제한 값
        int item = atoi(data->value); // 타깃 값

        switch (op)
        {
        case '<':
            if (item < limit)
                result = 1;
            break;
        case '>':
            if (item > limit)
                result = 1;
            break;
        case '=':
            if (item == limit)
                result = 1;
            break;
        case '!':
            if (item != limit)
                result = 1;
            break;
        default:
            break;
        }
    }

    else // 문자열 타입
    {
        char *limit = substring(1, strlen(val) - 2, val);
        char *item = data->value;

        switch (op)
        {
        case '<':
            if (strcmp(item, limit) == -1)
                result = 1;
            break;
        case '>':
            if (strcmp(item, limit) == 1)
                result = 1;
            break;
        case '=':
            if (!strcmp(item, limit))
                result = 1;
            break;
        case '!':
            if (strcmp(item, limit))
                result = 1;
            break;
        default:
            break;
        }
    }

    return result;
}

/*
 * 다중 조건에 맞는 Tuple 검색
 * col1: 조건1 컬럼
 * val1: 조건1 값
 * op1: 조건1 연산자
 * col2: 조건2 컬럼
 * val2: 조건2 값
 * op2: 조건2 연산자
 */
int find_multi_data(Table *table, Domain *domain, Data *data, char *col1, char *val1, char op1, char *col2, char *val2, char op2, int flag)
{
    int result1 = 0; // 조건 부합 => 1
    int result2 = 0; // 조건 부합 => 1

    while (strcmp(domain->column, col1)) // find col1
    {
        data = data->tuple;
        domain = domain->next;
    }

    if (strstr(domain->type, "int")) // 숫자 타입
    {
        int limit = atoi(val1);       // 제한 값
        int item = atoi(data->value); // 타깃 값

        switch (op1)
        {
        case '<':
            if (item < limit)
                result1 = 1;
            break;
        case '>':
            if (item > limit)
                result1 = 1;
            break;
        case '=':
            if (item == limit)
                result1 = 1;
            break;
        case '!':
            if (item != limit)
                result1 = 1;
            break;
        default:
            break;
        }
    }

    else // 문자열 타입
    {
        char *limit = substring(1, strlen(val1) - 2, val1);
        char *item = data->value;

        switch (op1)
        {
        case '<':
            if (strcmp(item, limit) == -1)
                result1 = 1;
            break;
        case '>':
            if (strcmp(item, limit) == 1)
                result1 = 1;
            break;
        case '=':
            if (!strcmp(item, limit))
                result1 = 1;
            break;
        case '!':
            if (strcmp(item, limit))
                result1 = 1;
            break;
        default:
            break;
        }
    }

    if (flag == 1) // and 연산
    {
        if (!result1) // false
            return result1;
    }

    domain = table->dhead->next; // Move first column (head next)

    while (strcmp(domain->column, col2)) // find col2
    {
        data = data->tuple;
        domain = domain->next;
    }

    if (strstr(domain->type, "int")) // 숫자 타입
    {
        int limit = atoi(val2);       // 제한 값
        int item = atoi(data->value); // 타깃 값

        switch (op2)
        {
        case '<':
            if (item < limit)
                result2 = 1;
            break;
        case '>':
            if (item > limit)
                result2 = 1;
            break;
        case '=':
            if (item == limit)
                result2 = 1;
            break;
        case '!':
            if (item != limit)
                result2 = 1;
            break;
        default:
            break;
        }
    }

    else // 문자열 타입
    {
        char *limit = substring(1, strlen(val2) - 2, val2);
        char *item = data->value;

        switch (op2)
        {
        case '<':
            if (strcmp(item, limit) == -1)
                result2 = 1;
            break;
        case '>':
            if (strcmp(item, limit) == 1)
                result2 = 1;
            break;
        case '=':
            if (!strcmp(item, limit))
                result2 = 1;
            break;
        case '!':
            if (strcmp(item, limit))
                result2 = 1;
            break;
        default:
            break;
        }
    }

    return flag == 1 ? result1 & result2 : result1 | result2;
}

Data *find_bottom_data(Data *data) // Data 목록에서 가장 아래 Data
{
    Data *cur;
    cur = data;

    while (cur->next != NULL)
    {
        cur = cur->next;
    }

    return (cur);
}

Data *find_right_data(Data *data) // Data 목록에서 가장 우측 Data
{
    Data *cur;
    cur = data;

    while (cur->tuple != NULL)
    {
        cur = cur->tuple;
    }

    return (cur);
}

void add_bottom_data(Data *data, char *iv) // Link to last next data
{
    Data *end;
    end = find_bottom_data(data);
    Data *new_data;
    new_data = (Data *)malloc(sizeof(Data)); // 새 Data 생성
    strcpy(new_data->value, iv);             // 입력 데이터 바인드
    end->next = new_data;
    new_data->next = NULL;
    new_data->tuple = NULL;
}

void add_right_data(Data *data, char *iv) // Link to last tuple data
{
    Data *end;
    end = find_right_data(data);
    Data *new_data;
    new_data = (Data *)malloc(sizeof(Data)); // 새 Data 생성
    strcpy(new_data->value, iv);             // 입력 데이터 바인드
    end->tuple = new_data;
    new_data->tuple = NULL;
    new_data->next = NULL;
}

void print_tuple(Data *data) // Tuple 출력
{
    Data *cur;
    cur = data;

    printf("+--------------------------------------+\n");
    printf("|  ");
    while (cur->tuple != NULL)
    {
        printf("%s  |  ", cur->value);
        cur = cur->tuple;
    }

    printf("%s  |\n", cur->value);
    printf("+--------------------------------------+\n");
}

void print_data(Domain *domain, Data *data, char *column) // 검색하는 컬럼 출력
{
    printf("|  ");
    while (strcmp(domain->column, column))
    {
        data = data->tuple;
        domain = domain->next;
    }

    printf("%s  |  ", data->value);
}

#endif