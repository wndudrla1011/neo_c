#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "db.h"
#include "table.h"
#include "domain.h"
#include "data.h"
#include "./hooks/create_table.h"

#define MAX_COLUMN 20      // 최대 속성 값 개수
#define MAX_INPUT 100      // 최대 입력 값 길이
#define MAX_CADINALITY 200 // 최대 튜플 개수

int main(void)
{
    DB *db = NULL;
    DB *head = NULL; // DB head
    Table *table = NULL;
    Domain *domain = NULL;
    Data *data = NULL;
    char input[MAX_INPUT]; // 입력 값
    char *command;         // 명령어

    while (1)
    {
        fgets(input, MAX_INPUT, stdin);

        command = strtok(input, " ");

        if (!strcasecmp(command, "show"))
        {
            command = strtok(NULL, ";");

            if (!strcasecmp(command, "databases")) // Query > show databases
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

            else if (!strcasecmp(command, "tables")) // Query > show tables
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

        else if (!strcasecmp(command, "create"))
        {
            command = strtok(NULL, " ");

            if (!strcasecmp(command, "database")) // Query > create database
            {
                if (get_cnt_db(head) == 0) // 첫 DB 생성
                {
                    db = init_db(); // DB 초기화
                    head = db;
                }
                add_db(db, strtok(NULL, ";")); // 연결 리스트 -> New DB
                printf("Query Success!\n");
            }

            else if (!strcasecmp(command, "table")) // Query > create table
            {
                command = strtok(NULL, "("); // table name

                if (db == head)
                {
                    printf("No database selected\n");
                    continue;
                }

                if (db->tcnt == 0) // 첫 Table 생성
                {
                    table = init_table(db);      // Table 초기화
                    domain = init_domain(table); // Domain 초기화
                    data = init_data(domain);    // Data 초기화
                }

                create_table(command, db, table, domain, data);

                printf("Query Success!\n");
            }
        }

        else if (!strcasecmp(command, "use")) // Query > use database
        {
            command = strtok(NULL, ";"); // DB name

            db = read_db(head, command); // 찾은 DB로 이동

            if (db == NULL)
                printf("Unknown database '%s'\n", command);
            else
                printf("Database changed\n");
        }

        else if (!strcasecmp(command, "drop"))
        {
            command = strtok(NULL, " ");

            if (!strcasecmp(command, "database")) // Query > drop database
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

            else if (!strcasecmp(command, "table")) // Query > drop table
            {
                command = strtok(NULL, ";");

                delete_table(db, command);
            }
        }

        else if (!strcasecmp(command, "desc")) // Query > desc table
        {
            command = strtok(NULL, ";"); // Table name

            print_all_domain(table);
        }

        else if (!strcasecmp(command, "insert")) // Query > insert table
        {
            command = strtok(NULL, " "); // into

            int cnt = 0; // cnt: '(' 개수

            for (int i = 0; i < sizeof(input); i++)
            {
                if (input[i] == '(')
                    cnt++;
            }

            // command = 테이블명
            if (cnt > 1) // columns(fields) 입력
                command = strtok(NULL, "(");
            else // columns(fields) 생략
                command = strtok(NULL, " ");
        }

    } // while(1)

    return 0;
}