#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "db.h"
#include "table.h"

#define MAX_COLUMN 20      // 최대 속성 값 개수
#define MAX_INPUT 100      // 최대 입력 값 길이
#define MAX_CADINALITY 200 // 최대 튜플 개수

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

char *types[] = {"int", "bigint", "varchar", "text"};

//>>>>>>>>>>>>>>>>>>>>>>>Domain 관련 메서드>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

int main(void)
{
    DB *db = NULL;
    DB *head = NULL; // DB head
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
                if (get_cnt_db(head) == 0)
                {
                    printf("No database exist\n");
                    continue;
                }

                else
                {
                    print_all_db(head); // 생성된 DB 출력
                }
            }

            else if (!strcmp(command, "tables") || !strcmp(command, "TABLES")) // Query > show tables
            {
                if (db->tcnt == 0) // 생성한 Table이 없을 때
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
                if (get_cnt_db(head) == 0) // 첫 DB 생성
                {
                    db = init_db(); // DB 초기화
                    head = db;
                }
                add_db(db, strtok(NULL, ";")); // 연결 리스트 -> New DB
                printf("Query Success!\n");
            }

            else if (!strcmp(command, "table") || !strcmp(command, "TABLE")) // Query > create table
            {
                command = strtok(NULL, "("); // table name

                if (db == head)
                {
                    printf("No database selected\n");
                    continue;
                }

                if (db->tcnt == 0) // 첫 Table 생성
                {
                    table = init_table(db); // Table 초기화
                }

                add_table(db, table, command); // 연결 리스트 -> New Table

                char *attr_info[MAX_INPUT];
                char *column = NULL, *type = NULL, *nullable = NULL;

                int cnt = 0;       // create token 개수
                int flag = 1;      // NOT NULL, NULL 구분용
                int type_flag = 0; // 현재 데이터가 type인지 여부
                int data_len = 0;  // 데이터 길이
                char *tmp;
                while ((tmp = strtok(NULL, ", ()")) != NULL)
                {
                    attr_info[cnt++] = tmp;
                }

                for (int i = 0; i < cnt; i++)
                {
                    for (int j = 0; j < sizeof(types) / sizeof(types[0]); j++)
                    {
                        if (!strcasecmp(attr_info[i], types[j]))
                        {
                            type = attr_info[i];
                            type_flag = 1;
                            break;
                        }
                    }

                    if (!strcasecmp(attr_info[i], "NOT"))
                    {
                        flag = 0;
                        continue;
                    }

                    if (!strcasecmp(attr_info[i], "NULL"))
                    {
                        if (flag == 0) // NOT NULL
                            nullable = "NOT NULL";
                        else
                            nullable = "NULL";

                        flag = 1, type_flag = 0;

                        printf("%s %s %s\n", column, type, nullable);
                        continue;
                    }

                    if (type_flag == 0)
                        column = attr_info[i];
                }

                printf("Query Success!\n");
            }
        }

        else if (!strcmp(command, "use") || !strcmp(command, "USE")) // Query > use database
        {
            command = strtok(NULL, ";"); // DB name

            db = read_db(head, command); // 찾은 DB로 이동

            if (db == NULL)
                printf("Unknown database '%s'\n", command);
            else
                printf("Database changed\n");
        }

        else if (!strcmp(command, "drop") || !strcmp(command, "DROP"))
        {
            command = strtok(NULL, " ");

            if (!strcmp(command, "database") || !strcmp(command, "DATABASE")) // Query > drop database
            {
                command = strtok(NULL, ";");

                if (read_db(head, command) == NULL)
                {
                    printf("Now current DB is NULL\nPlease use another DB\n");
                    continue;
                }

                delete_db(head, command);
                printf("Query Success!\n");
            }

            else if (!strcmp(command, "table") || !strcmp(command, "TABLE")) // Query > drop table
            {
                command = strtok(NULL, ";");

                delete_table(db, command);
            }
        }
    }

    return 0;
}