#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "db.h"
#include "table.h"
#include "./util/getLen.h"
#include "data.h"

#define MAX_COLUMN 20      // 최대 속성 값 개수
#define MAX_INPUT 100      // 최대 입력 값 길이
#define MAX_CADINALITY 200 // 최대 튜플 개수

char *types[] = {"int", "INT", "bigint", "BIGINT", "varchar", "VARCHAR", "text", "TEXT"};

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
                    table = init_table(db);      // Table 초기화
                    domain = init_domain(table); // Domain 초기화
                    data = init_data(domain);    // Data 초기화
                }

                add_table(db, table, command); // 연결 리스트 -> New Table

                char *attr_info[MAX_INPUT]; // create table 을 Tokenizer 한 결과 저장
                char *column = NULL, *type = NULL, *nullable = NULL;

                int cnt = 0;       // create token 개수
                int flag = 1;      // NOT NULL, NULL 구분용
                int type_flag = 0; // 현재 데이터가 type인지 여부
                int len = 0;       // 데이터 길이
                char *token;       // 데이터 도메인 토큰
                char *type_token;  // 타입 + 길이

                while ((token = strtok(NULL, ", );")) != NULL) // 토큰 저장
                {
                    attr_info[cnt++] = token;
                }

                for (int i = 0; i < cnt; i++) // 저장된 토큰 읽기
                {
                    for (int j = 0; j < sizeof(types) / sizeof(types[0]); j++)
                    {
                        if (strstr(attr_info[i], types[j]) != NULL) // 데이터 타입 목록과 일치
                        {
                            type_token = strtok(attr_info[i], "("); // 데이터 타입
                            type = type_token;

                            len = getLen(type_token, types[j]); // 타입에 맞는 데이터 길이를 구함

                            type_flag = 1; // 현재 토큰은 타입
                            break;
                        }
                    }

                    if (!strcasecmp(attr_info[i], "NOT") || !strcasecmp(attr_info[i], "not"))
                    {
                        flag = 0;
                        continue;
                    }

                    if (!strcasecmp(attr_info[i], "NULL") || !strcasecmp(attr_info[i], "null"))
                    {
                        if (flag == 0) // NOT NULL
                            nullable = "NOT NULL";
                        else // NULL
                            nullable = "NULL";

                        // Domain 1개 종료

                        flag = 1, type_flag = 0; // flag 초기화

                        add_domain(domain, column, type, len, nullable); // Domain 추가

                        continue;
                    }

                    if (type_flag == 0) // 현재 토큰이 타입이 아님
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

        else if (!strcmp(command, "desc") || !strcmp(command, "DESC"))
        {
            command = strtok(NULL, ";"); // Table name

            print_all_domain(table);
        }
    }

    return 0;
}