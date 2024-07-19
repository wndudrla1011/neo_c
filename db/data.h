#ifndef DATA_H
#define DATA_H

#include <stdio.h>
#include <stdlib.h>
#include "table.h"

typedef struct Domain // DCL용
{
    char *column;        // 컬령명
    char *type;          // 데이터 타입
    int len;             // 데이터 길이
    char *nullable;      // 널 가능 여부
    struct Domain *next; // 다음 Domain 포인터
    struct Data *head;   // 도메인의 데이터 목록
} Domain;

typedef struct Data
{
    char *value;       // 해당 Domain에 해당하는 데이터
    struct Data *next; // 다음 데이터 포인터
} Data;

Domain *init_domain(Table *table) // Domain 생성 후 테이블 연결
{
    Domain *head;
    head = (Domain *)malloc(sizeof(Domain));
    table->dhead = head;

    return (head);
}

Data *init_data(Domain *domain) // Data 목록의 head 생성
{
    Data *head;
    head = (Data *)malloc(sizeof(Data));
    head->next = NULL;
    domain->head = head;

    return (head);
}

void add_domain(Domain *D, char *column, char *type, int len, char *nullable) // Domain의 각 멤버에 값 입력
{
}

#endif