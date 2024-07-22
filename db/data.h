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
    struct Data *next;     // 다음 행 데이터
} Data;

Data *init_data(Domain *domain)
{
    Data *head;
    head = (Data *)malloc(sizeof(Data));
    head->next = NULL;
    domain->head = head;

    return (head);
}

Data *find_end_data(Data *data) // Data 목록에서 가장 아래 Data
{
    Data *cur;
    cur = data;

    while (cur->next != NULL)
    {
        cur = cur->next;
    }

    return (cur);
}

void add_data(Data *data, char *iv)
{
    Data *end;
    end = find_bottom_data(data); // Leaf Data
    Data *new_data;
    new_data = (Data *)malloc(sizeof(Data)); // 새 Data 생성
    strcpy(new_data->value, iv);             // 입력 데이터 바인드
    end->next = new_data;
    new_data->next = NULL;
}

#endif