#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "db.h"
#include "table.h"
#include "domain.h"
#include "data.h"
#include "./hooks/create_table.h"
#include "./util/substring.h"

#define MAX_COLUMN 20      // 최대 속성 값 개수
#define MAX_INPUT 100      // 최대 입력 값 길이
#define MAX_CADINALITY 200 // 최대 튜플 개수

int main(void)
{
    DB *db = NULL;
    DB *head = NULL; // DB head
    Table *table = NULL;
    Table *thead = NULL;
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
                command = strtok(NULL, ";");

                if (get_cnt_db(head) == 0) // 첫 DB 생성
                {
                    db = init_db(); // DB 초기화
                    head = db;
                }

                if (read_db(db, command) != NULL) // 같은 이름의 DB가 이미 존재하는 경우
                {
                    printf("Can't create database '%s'; database exists\n", command);
                    continue;
                }

                add_db(db, command); // 연결 리스트 -> New DB
                printf("Query Success!\n");
            }

            else if (!strcasecmp(command, "table")) // Query > create table
            {
                command = strtok(NULL, "("); // table name

                if (db == head) // use database 를 하지 않은 상태
                {
                    printf("No database selected\n");
                    continue;
                }

                if (db->thead == NULL) // Table head 없음
                {
                    table = init_table(db); // Table 초기화
                    thead = table;
                }

                if (read_table(thead, command) != NULL) // 같은 이름의 Table이 이미 존재하는 경우
                {
                    printf("Table '%s' already exists\n", command);
                    continue;
                }

                domain = init_domain(table); // Domain 초기화

                create_table(command, db, table, table->dhead);

                print_all_domain(table);

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
            if (db->thead == NULL) // Not found Table
            {
                printf("Table '%s' doesn't exist\n", command);
                continue;
            }

            command = strtok(NULL, ";"); // Table name

            table = read_table(thead, command);

            print_all_domain(table);
        }

        else if (!strcasecmp(command, "insert")) // Query > insert table
        {
            if (db == head)
            {
                printf("No database selected\n");
                continue;
            }

            command = strtok(NULL, " "); // into

            int cnt = 0; // cnt: '(' 개수

            for (int i = 0; i < sizeof(input); i++) // insert 쿼리에 columns 목록 존재 여부 확인
            {
                if (input[i] == '(')
                    cnt++;
            }

            // command = 테이블명
            if (cnt > 1) // columns(fields) 입력
                command = strtok(NULL, "(");
            else // columns(fields) 생략
                command = strtok(NULL, " ");

            table = read_table(thead, command); // 테이블명으로 테이블 찾기

            if (table == NULL) // Not found Table
            {
                printf("Table '%s' doesn't exist\n", command);
                continue;
            }

            command = strtok(NULL, "(");

            int token_cnt = 0;
            char *values[MAX_INPUT];
            char *token;
            char *pos;    // 첫 single quote가 등장하는 위치 == 문자열 데이터 시작 위치
            char *unpack; // substring 결과

            while ((token = strtok(NULL, ",);")) != NULL)
            {
                values[token_cnt++] = token;
            }

            for (int i = 0; i < token_cnt; i++)
            {
                if ((pos = strstr(values[i], "\'")) != NULL) // 문자열 데이터
                {
                    unpack = substring(1, strlen(pos) - 2, pos); // 맨 앞뒤 single quote 제거
                    values[i] = unpack;
                }
            }

            // add_data(table, data);
        }

    } // while(1)

    return 0;
}