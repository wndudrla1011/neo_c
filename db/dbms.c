#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_COLUMN 20      // 최대 속성 값 개수
#define MAX_INPUT 100      // 최대 입력 값 길이
#define MAX_CADINALITY 200 // 최대 튜플 개수

typedef struct DB
{
    char dname[MAX_INPUT]; // DB명
    int tcnt;              // 테이블 개수
    struct DB *next;       // 다음 DB 포인터
    struct Table *thead;   // 테이블 목록 (Table 순차 검색)
} DB;

typedef struct Table
{
    char tname[MAX_INPUT];                  // 테이블명
    int cadinality;                         // 튜블 개수 == tuple 스택의 top
    struct Table *next;                     // 다음 테이블 포인터
    char *tuple[MAX_CADINALITY][MAX_INPUT]; // 튜플 선언
} Table;

//>>>>>>>>>>>>>>>>>>>>>>>DB 관련 메서드>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

DB *init_db() // DB 목록의 head 생성
{
    DB *head;
    head = (DB *)malloc(sizeof(DB));
    head->next = NULL;

    return (head);
}

int get_cnt_db(DB *db) // 생성된 DB 개수
{
    int cnt = 0;
    DB *cur = db;

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

void print_all_db(DB *db) // 모든 DB 출력
{
    DB *cur;
    cur = db->next;

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

//>>>>>>>>>>>>>>>>>>>>>>>Table 관련 메서드>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

Table *init_table(DB *db) // Table 목록의 head 생성
{
    Table *head;
    head = (Table *)malloc(sizeof(Table));
    head->next = NULL;
    db->thead = head; // DB의 Table head 설정

    return (head);
}

int get_cnt_table(Table *T) // 생성된 Table 개수
{
    int cnt = 0;
    Table *cur = T;

    while (cur != NULL)
    {
        cur = cur->next;
        cnt++;
    }

    return cnt;
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
    Table *table;
    table = db->thead->next;

    if (table != NULL)
    {
        printf("======================================");
        while (1)
        {
            printf("\n%s", table->tname);
            if (table->next == NULL)
            {
                printf("\n");
                break;
            }

            table = table->next;
        }
        printf("======================================\n\n");
    }
}

int main(void)
{
    DB *db = NULL;
    Table *table = NULL;
    char input[MAX_INPUT]; // 입력 값
    char *command;         // 명령어

    while (1)
    {
        fgets(input, MAX_INPUT, stdin);

        command = strtok(input, " ");

        if (!strcmp(command, "show") || !strcmp(command, "SHOW"))
        {
            command = strtok(NULL, ";");

            if (!strcmp(command, "databases") || !strcmp(command, "DATABASES")) // Query > show databases
            {
                if (get_cnt_db(db) == 0) // 생성한 DB가 없을 때
                {
                    printf("No database exist\n");
                    continue;
                }
                else
                {
                    print_all_db(db); // 생성된 DB 출력
                }
            }

            else if (!strcmp(command, "tables") || !strcmp(command, "TABLES")) // Query > show table
            {
                if (get_cnt_table(table) == 0) // 생성한 Table이 없을 때
                {
                    printf("Entry empty\n");
                    continue;
                }
                else
                {
                    print_all_table(db); // 생성된 Table 출력
                }
            }
        }
        else if (!strcmp(command, "create") || !strcmp(command, "CREATE"))
        {
            command = strtok(NULL, " ");

            if (!strcmp(command, "database") || !strcmp(command, "DATABASE")) // Query > create database
            {
                if (get_cnt_db(db) == 0) // 첫 DB 생성
                {
                    db = init_db(); // DB 초기화
                }
                add_db(db, strtok(NULL, ";")); // 연결 리스트 -> New DB
                printf("Query Success!\n");
            }

            else if (!strcmp(command, "table") || !strcmp(command, "TABLE")) // Query > create table
            {
                if (get_cnt_table(table) == 0) // 첫 Table 생성
                {
                    if (db == NULL)
                    {
                        printf("No database selected\n");
                        continue;
                    }
                    table = init_table(db); // Table 초기화
                }
                add_table(db, table, strtok(NULL, ";")); // 연결 리스트 -> New Table
                printf("Query Success!\n");
            }
        }
    }

    return 0;
}