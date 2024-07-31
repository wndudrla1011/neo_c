#ifndef TABLE_H
#define TABLE_H

#include <stdio.h>
#include <stdlib.h>

typedef struct Table
{
    char tname[MAX_INPUT]; // 테이블명
    int cadinality;        // 튜블 개수 == tuple 스택의 top
    int degree;            // 속성 개수
    struct Table *next;    // 다음 테이블 포인터
    struct Domain *dhead;  // 도메인 목록
} Table;

Table *init_table(DB *db) // Table 목록의 head 생성
{

    printf("init table \n");
    Table *head;
    head = (Table *)malloc(sizeof(Table));
    head->next = NULL;

    head->tname[0] = NULL;

    printf("head->tname : %s\n", head->tname);
    db->thead = head; // DB의 Table head 설정

    return (head);
}

Table *find_end_table(Table *T) // DB의 Table 목록에서 가장 마지막 Table 찾기
{
    Table *cur;
    cur = T;

    while (cur->next != NULL)
    {
        cur = cur->next;
    }

    return (cur);
}

void add_table(DB *db, Table *table, char *name) // DB의 Table 목록 마지막 노드에 새 Table 추가
{
    Table *end;
    end = find_end_table(table); // Leaf Table
    Table *new_table;
    new_table = (Table *)malloc(sizeof(Table)); // 새 Table 생성
    strcpy(new_table->tname, name);             // 테이블명 입력
    end->next = new_table;                      // last table -> new table
    new_table->next = NULL;
    db->tcnt++; // DB의 테이블 개수 증가
}

void print_all_table(DB *db) // 모든 Table 출력
{
    Table *cur;
    cur = db->thead->next;

    if (cur != NULL)
    {
        printf("======================================");
        while (1)
        {
            printf("\n%s", cur->tname);
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

Table *read_table(Table *h, char *tname) // Table 이름으로 Table 찾기
{
    if (h == NULL) // Table을 생성하기 전 상황
        return (h);

    Table *cur;
    cur = h;

    printf("cur : %d h : %d tname :%d\n", cur, h, tname);

    printf("tname : %s\n", tname);

    printf("cur->name : %s\n", cur->tname);
    while (strcmp(cur->tname, tname) && cur->next != NULL)
    {
        printf("cur->name : %s\n", cur->tname);
        cur = cur->next;
    }

    if (cur->next == NULL && strcmp(cur->tname, tname)) // not found table
    {
        return NULL;
    }

    return (cur);
}

void delete_all_table(DB *db) // 모든 테이블 삭제
{
    Table *cur;
    cur = db->thead;

    Table *next;
    while (cur != NULL)
    {
        next = cur->next;

        free(cur);
        cur = next;
    }
    free(db->thead);
}

void delete_table(DB *db, char *name) // 테이블 삭제
{
    Table *prev;
    Table *cur; // 삭제할 테이블
    cur = db->thead;

    while (strcmp(cur->tname, name) && cur->next != NULL)
    {
        if (!strcmp(cur->next->tname, name))
            prev = cur;
        cur = cur->next;
    }

    prev->next = cur->next;
    cur->next = NULL;

    free(cur);
    db->tcnt--; // DB의 Table 개수 감소
}

#endif