#ifndef DB_H
#define DB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "./util/directory.h"

typedef struct DB
{
    char dname[MAX_INPUT]; // DB명
    int tcnt;              // 테이블 개수
    struct DB *next;       // 다음 DB 포인터
    struct Table *thead;   // 테이블 목록 (Table 순차 검색)
} DB;

DB *init_db() // DB 목록의 head 생성
{
    DB *head;
    head = (DB *)malloc(sizeof(DB));
    head->next = NULL;
    head->thead = NULL;

    return (head);
}

int get_cnt_db(DB *h) // 생성된 DB 개수
{
    if (h == NULL) // DB를 생성하기 전 상황
        return 0;

    int cnt = -1;
    DB *cur = h;

    while (cur != NULL)
    {
        cur = cur->next;
        cnt++;
    }

    return cnt;
}

DB *find_end_db(DB *db) // 가장 마지막 DB 찾기
{
    DB *cur;
    cur = db;

    while (cur->next != NULL)
    {
        cur = cur->next;
    }

    return (cur);
}

void add_db(DB *db, char *name) // 마지막 노드에 새 DB 추가
{
    DB *end;
    end = find_end_db(db); // Leaf DB
    DB *new_db;
    new_db = (DB *)malloc(sizeof(DB)); // 새 DB 생성
    strcpy(new_db->dname, name);       // DB명 입력
    end->next = new_db;                // last db -> new db
    new_db->next = NULL;
}

void print_all_db(DB *h) // 모든 DB 출력
{
    DB *cur;
    cur = h->next;

    if (cur != NULL)
    {
        printf("======================================");
        while (1)
        {
            printf("\n%s", cur->dname);
            if (cur->next == NULL)
            {
                printf("\n");
                break;
            }

            cur = cur->next;
        }
        printf("======================================\n\n");
    }
}

DB *read_db(DB *h, char *dname) // DB 이름으로 DB 찾기
{
    if (h == NULL)
        return (h);

    DB *cur;
    cur = h->next;

    while (strcmp(cur->dname, dname) && cur->next != NULL)
    {
        cur = cur->next;
    }

    if (cur->next == NULL && strcmp(cur->dname, dname)) // Not found DB
    {
        return NULL;
    }

    return (cur);
}

void delete_db(DB *h, char *name) // DB 삭제
{
    DB *prev;
    DB *cur; // 삭제할 DB
    cur = h;

    while (strcmp(cur->dname, name) && cur->next != NULL)
    {
        if (!strcmp(cur->next->dname, name))
            prev = cur;
        cur = cur->next;
    }

    prev->next = cur->next;
    cur->next = NULL;

    free(cur);
}

#endif