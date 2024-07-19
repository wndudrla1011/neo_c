#ifndef DATA_H
#define DATA_H

#include <stdio.h>
#include <stdlib.h>
#include "table.h"

typedef struct Domain // DCL용
{
    char *column;        // 컬령명
    char *type;          // 데이터 타입
    char *len;           // 데이터 길이
    int nullable;        // 널 가능 여부
    struct Tuple *tuple; // 데이터
} Domain;

typedef struct Tuple
{
    char *data;         // id text text일 때, "1 title1 test" 식으로 저장
    struct Tuple *next; // 다음 튜플
} Tuple;

Domain *init_domain(Table *T) // Domain 생성 후 테이블 연결
{
    Domain *domain;
    domain = (Domain *)malloc(sizeof(Domain));
    T->domain = domain;

    return (domain);
}

Tuple *init_tuple(Domain *D) // Tuple 목록의 head 생성
{
    Tuple *head;
    head = (Tuple *)malloc(sizeof(Tuple));
    head->next = NULL;

    return (head);
}

#endif