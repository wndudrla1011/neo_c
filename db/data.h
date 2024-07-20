#ifndef DATA_H
#define DATA_H

#include <stdio.h>
#include <stdlib.h>
#include "table.h"

typedef struct Data
{
    char *value;       // 해당 Domain에 해당하는 데이터
    struct Data *next; // 다음 데이터 포인터
} Data;

Data *init_data(Domain *domain)
{
    Data *head;
    head = (Data *)malloc(sizeof(Data));
    domain->head = head;

    return (head);
}

Data *find_end_data(Data *data) // Domain의 Data 목록에서 가장 마지막 Data 찾기
{
    Data *cur;
    cur = data;

    while (cur->next != NULL)
    {
        cur = cur->next;
    }

    return (cur);
}

void add_data(Table *table, Data *data)
{
    Domain *cur;
    cur = table->dhead->next;

    Data *end;
    end = find_end_data(data); // Leaf Data
    Data *new_data;
    new_data = (Data *)malloc(sizeof(Data)); // 새 Data 생성

    // while (cur->next != NULL) // 각 Domain에 Data 입력
    // {
    // }
}

#endif