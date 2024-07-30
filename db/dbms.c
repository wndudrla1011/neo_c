#define MAX_COLUMN 20      // 최대 속성 값 개수
#define MAX_INPUT 100      // 최대 입력 값 길이
#define MAX_CADINALITY 200 // 최대 튜플 개수

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

#include "db.h"
#include "table.h"
#include "domain.h"
#include "data.h"
#include "./hooks/create_table.h"
#include "./hooks/search_table.h"
#include "./hooks/update_table.h"
#include "./hooks/delete_table.h"
#include "./hooks/insert_table.h"
#include "./util/substring.h"
#include "./util/directory.h"

char *root = "/home/jooyoungkim/joosql";

int main(void)
{
    DB *db = NULL;
    DB *head = NULL; // DB head
    Table *table = NULL;
    Domain *domain = NULL;
    Data *data = NULL;
    int is_where = 0;       // where절 존재 여부 > delete에서 사용
    char input[MAX_INPUT];  // 입력 값
    char *command;          // 명령어
    char *db_dir = NULL;    // DB 폴더
    char *table_dir = NULL; // Table 폴더

    if (directoryExists(root))
    {
        printf("디렉토리가 이미 존재합니다: %s\n", root);
    }
    else
    {
        createDirectory(root);
    }

    while (1)
    {
        fgets(input, MAX_INPUT, stdin);

        if (strstr(input, "where") != NULL) // where절 존재
        {
            is_where = 1;
        }

        command = strtok(input, " ");

        if (!strcasecmp(command, "show"))
        {
            command = strtok(NULL, ";");

            if (!strcasecmp(command, "databases")) // Query > show databases
            {
                if (get_cnt_dir(root) == 0) // 생성한 DB 폴더가 없는 경우
                {
                    printf("No database exist\n");
                    continue;
                }

                else
                {
                    print_all_dir(root); // 생성된 DB 폴더명 출력
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

            else // handling syntax error
            {
                printf("You have an error in your SQL syntax;\n");
            }
        }

        else if (!strcasecmp(command, "create"))
        {
            command = strtok(NULL, " "); // database or table

            if (!strcasecmp(command, "database")) // Query > create database
            {
                command = strtok(NULL, ";"); // database name

                if (get_cnt_dir(root) == 0) // 첫 DB 생성
                {
                    db_dir = init_dir(root); // DB 초기화
                    // head = db;               // DB head 설정
                }

                if (read_dir(command, root) != NULL) // 같은 이름의 DB 폴더가 이미 존재하는 경우
                {
                    printf("Can't create database '%s'; database exists\n", command);
                    continue;
                }

                add_dir(command, root); // 연결 리스트 -> New DB

                printf("Query Success!\n");
            }

            else if (!strcasecmp(command, "table")) // Query > create table
            {
                command = strtok(NULL, "("); // table name

                if (command == NULL) // handling syntax error
                {
                    printf("You have an error in your SQL syntax;\n");
                    continue;
                }

                if (db_dir == NULL) // use database 를 하지 않은 상태
                {
                    printf("No database selected\n");
                    continue;
                }

                if (get_cnt_dir(db_dir) == 0) // Table head 없음
                {
                    table_dir = init_dir(db_dir); // DB 폴더에 Table head 생성
                    // db->thead = table;            // table head 설정
                }

                if (read_dir(command, db_dir) != NULL) // 같은 이름의 Table 폴더가 이미 존재하는 경우
                {
                    printf("Table '%s' already exists\n", command);
                    continue;
                }

                // create_table(command, db, table, domain); // 테이블 생성
                create_dir(command, db_dir); // 테이블 폴더 생성
                printf("Query Success!\n");
            }

            else // handling syntax error
            {
                printf("You have an error in your SQL syntax;\n");
            }
        }

        else if (!strcasecmp(command, "use")) // Query > use database
        {
            command = strtok(NULL, ";"); // DB name

            if (read_dir(command, root) == NULL) // not found db
                printf("Unknown database '%s'\n", command);
            else // found db
                printf("Database changed\n");
        }

        else if (!strcasecmp(command, "drop"))
        {
            command = strtok(NULL, " "); // database

            if (!strcasecmp(command, "database")) // Query > drop database
            {
                command = strtok(NULL, ";"); // database name

                if (read_db(head, command) == NULL) // not found db
                {
                    printf("Now current DB is NULL\nPlease use another DB\n");
                    continue;
                }

                delete_db(head, command); // DB 삭제
                printf("Query Success!\n");
            }

            else if (!strcasecmp(command, "table")) // Query > drop table
            {
                command = strtok(NULL, ";"); // table name

                if (read_table(db->thead, command) == NULL) // not found table
                {
                    printf("Table '%s' doesn't exist\n", command);
                    continue;
                }

                delete_table(db, command); // Table 삭제
                printf("Query Success!\n");
            }
        }

        else if (!strcasecmp(command, "desc")) // Query > desc table
        {
            command = strtok(NULL, ";"); // Table name

            if (db == head) // use database 를 하지 않은 상태
            {
                printf("No database selected\n");
                continue;
            }

            table = read_table(db->thead, command); // 찾는 테이블로 이동

            if (table == NULL) // Not found Table
            {
                printf("Table '%s' doesn't exist\n", command);
                continue;
            }

            print_all_domain(table); // Table의 Domain 출력
        }

        else if (!strcasecmp(command, "insert")) // Query > insert table
        {
            if (db == head) // use database 하지 않은 상태
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

            table = read_table(db->thead, command); // 테이블명으로 테이블 찾기

            if (table == NULL) // Not found Table
            {
                printf("Table '%s' doesn't exist\n", command);
                continue;
            }

            command = strtok(NULL, "("); // values

            query_insert(db, table, domain, data);
            printf("Query Success!\n");
        }

        else if (!strcasecmp(command, "select")) // Query > select table
        {
            if (db == head) // not found db
            {
                printf("No database selected\n");
                continue;
            }

            query_select(db, table, domain, data);
        }

        else if (!strcasecmp(command, "update")) // Query > update table
        {
            command = strtok(NULL, " "); // table name

            if (db == head) // not found db
            {
                printf("No database selected\n");
                continue;
            }

            table = read_table(db->thead, command);

            if (table == NULL) // not found table
            {
                printf("Table '%s' doesn't exist\n", command);
                continue;
            }

            command = strtok(NULL, " "); // set

            query_update(db, table, domain, data);
            printf("Query Success!\n");
        }

        else if (!strcasecmp(command, "delete")) // Query > delete table
        {
            command = strtok(NULL, " "); // from

            if (db == head) // not found db
            {
                printf("No database selected\n");
                continue;
            }

            if (is_where) // where절 존재
            {
                command = strtok(NULL, " "); // table name
            }

            else // where절 존재x
            {
                command = strtok(NULL, ";"); // table name
            }

            table = read_table(db->thead, command);

            if (table == NULL) // not found table
            {
                printf("Table '%s' doesn't exist\n", command);
                continue;
            }

            query_delete(db, table, domain, data, is_where);
            printf("Query Success!\n");
        }

        else if (!strcasecmp(command, "exit\n"))
        {
            printf("Bye~\n");
            break;
        }

        else if (command[0] == '\n')
        {
            continue;
        }

        else if (strstr(input, ";") == NULL) // 세미콜론 포함x
        {
            printf("Complete the command, including the ';'\n");
            continue;
        }

        else // not exists command
        {
            printf("You have an error in your SQL syntax;\n");
        }

    } // while(1)*/

    return 0;
}