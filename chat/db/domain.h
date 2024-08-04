#ifndef DOMAIN_H
#define DOMAIN_H

#include <stdio.h>
#include <stdlib.h>

typedef struct Domain // DCL용
{
    char column[MAX_INPUT];   // 컬령명
    char type[MAX_INPUT];     // 데이터 타입
    int len;                  // 데이터 길이
    char nullable[MAX_INPUT]; // 널 가능 여부
    struct Domain *next;      // 다음 Domain 포인터
    struct Data *head;        // 도메인의 데이터 목록
} Domain;

Domain *init_domain(Table *table) // Domain 생성 후 테이블 연결
{
    Domain *head;
    head = (Domain *)malloc(sizeof(Domain));
    head->next = NULL;
    table->dhead = head;

    return (head);
}

Domain *find_end_domain(Domain *D) // Table의 Domain 목록에서 가장 마지막 Domain 찾기
{
    Domain *cur;
    cur = D;

    while (cur->next != NULL)
    {
        cur = cur->next;
    }

    return (cur);
}

void add_domain(Domain *D, char *column, char *type, int len, char *nullable) // Domain의 각 멤버에 값 입력
{
    Domain *end;
    end = find_end_domain(D); // Leaf Domain
    Domain *new_domain;
    new_domain = (Domain *)malloc(sizeof(Domain)); // 새 Domain 생성
    strcpy(new_domain->column, column);            // 컬럼명
    strcpy(new_domain->type, type);                // 데이터 타입
    new_domain->len = len;                         // 데이터 길이
    strcpy(new_domain->nullable, nullable);        // 널 여부
    end->next = new_domain;                        // last domain -> new domain
    new_domain->next = NULL;
}

void print_all_domain(Table *table) // Table의 모든 Domain 정보 출력
{
    Domain *cur;
    cur = table->dhead->next;

    if (cur != NULL)
    {
        printf("+---------------------------------+");
        printf("\n| Field\t| Type\t| Len | Null |\n");
        printf("+---------------------------------+\n");
        while (1)
        {
            printf("| %s | %s | %d | %s |\n", cur->column, cur->type, cur->len, !strcmp(cur->nullable, "NULL") ? "YES" : "NO");
            if (cur->next == NULL)
            {
                break;
            }

            cur = cur->next;
        }
        printf("+---------------------------------+\n\n");
    }
}

#endif