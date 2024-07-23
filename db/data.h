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

Data *find_first_data(Data *h) // Data 목록에서 head 바로 뒤 Data
{
    if (h == NULL)
        return (h);

    Data *cur;
    cur = h->next;

    return (cur);
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

Data *find_bottom_index_data(Data *h, int idx)
{
    if (idx == 1)
        return find_first_data(h);
    else if (idx == data_len(h))
        return find_bottom_data(h);
    else if (idx < 1 || idx > data_len(h))
    {
        printf("Wrong index\n");
        return NULL;
    }
    else
    {
        int i;
        Data *prev;
        prev = h;
        i = 0;
        while (i < idx - 1)
        {
            prev = prev->next;
            i++;
        }

        return prev->next;
    }
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

void print_tuple(Data *data)
{
    Data *cur;
    cur = data;
    int index = 1;

    while (cur->tuple != NULL)
    {
        printf("%s  |  ", cur->value);
        cur = cur->tuple;
    }

    printf("%s\n", cur->value);
}

#endif