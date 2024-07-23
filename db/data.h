#ifndef DATA_H
#define DATA_H

#include <stdio.h>
#include <stdlib.h>
#include "table.h"
#include "domain.h"

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

Data *find_first_data(Data *h) // Data 목록에서 head 바로 뒤 Data
{
    if (h == NULL)
        return (h);

    Data *cur;
    cur = h->next;

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

void add_data(Data *data, char *iv)
{
    Data *end;
    end = find_right_data(data);
    Data *new_data;
    new_data = (Data *)malloc(sizeof(Data)); // 새 Data 생성
    strcpy(new_data->value, iv);             // 입력 데이터 바인드
    end->tuple = new_data;
    new_data->tuple = NULL;
}

#endif