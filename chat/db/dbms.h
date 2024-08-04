#ifndef DBMS_H
#define DBMS_H

#define MAX_COLUMN 20      // 최대 속성 값 개수
#define MAX_INPUT 10000    // 최대 입력 값 길이
#define MAX_CADINALITY 200 // 최대 튜플 개수

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>

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

char *root = "joosql"; // databases 관리

int joosql_init(char *user, char *pw)
{
    if (strcmp(user, "user1") || strcmp(pw, "0000")) // validate
    {
        printf("Unknown user or password...\n");
        return -1;
    }

    // create root
    if (directoryExists(root))
    {
        printf("Found the connection: %s\n", root);
    }
    else
    {
        createDirectory(root);
    }

    return 0;
}

int joosql_connect() // 사용할 DB, Table 설정
{
    char *db = "chatdb";
    char *table = "chat";
    char *res_db = (char *)malloc(100 * sizeof(char));
    char *res_tb = (char *)malloc(100 * sizeof(char));
    char *db_dir = (char *)malloc(100 * sizeof(char));
    char *table_dir = (char *)malloc(100 * sizeof(char));

    if (get_cnt_dir(root) == 0) // 첫 DB 생성
    {
        char *head = init_dir(root); // head 생성
        strcpy(db_dir, head);
        free(head);
    }

    res_db = read_dir(db, root); // db 존재 확인

    if (res_db == NULL) // db 존재x
    {
        add_dir(db, root); // db 생성
    }

    else // db 존재
    {
        free(res_db);
    }

    sprintf(db_dir, "%s/%s", root, db); // db 경로 생성

    if (get_cnt_dir(db_dir) == 0) // 첫 Table 생성
    {
        char *thead = init_dir(db_dir); // DB dir -> Table head
        strcpy(table_dir, thead);
        free(thead);
    }

    res_tb = read_dir(table, db_dir); // table 존재 확인

    if (res_tb == NULL) // table 존재x
    {
        add_dir(table, db_dir);
    }

    else
    {
        free(res_tb);
    }

    char *query = (char *)malloc(100 * sizeof(char));
    sprintf(query, "date bigint not null, contents varchar(100) not null");

    create_table(table, db_dir, query); // DB 폴더에 Table 폴더 생성

    printf("Be ready to use joosql\n");

    free(query);
    return 0;
}

int joosql_query(char *query)
{
    DB *db = NULL;
    DB *head = NULL; // DB head
    Table *table = NULL;
    Domain *domain = NULL;
    Data *data = NULL;
    int is_where = 0;     // where절 존재 여부 > delete에서 사용
    char *command = NULL; // 명령어
    char input[MAX_INPUT] = {0};
    char db_dir[MAX_INPUT] = {0};    // DB 폴더
    char table_dir[MAX_INPUT] = {0}; // Table 폴더
    char origin_query[MAX_INPUT] = {0};

    strcpy(input, query);        // 원본 쿼리 저장 > Tokenizing
    strcpy(origin_query, query); // 원본 쿼리 저장 > sending

    if (strstr(query, "where") != NULL) // where절 존재
    {
        is_where = 1;
    }

    command = strtok(input, " ");

    if (!strcasecmp(command, "show")) // Query > show
    {
        command = strtok(NULL, ";");

        if (!strcasecmp(command, "databases")) // Query > show databases
        {
            if (get_cnt_dir(root) == 0) // 생성한 DB 폴더가 없는 경우
            {
                printf("No database exist\n");
                return 0;
            }

            else
            {
                print_all_dir(root); // 생성된 DB 폴더명 출력
            }
        }

        else if (!strcasecmp(command, "tables")) // Query > show tables
        {
            if (get_cnt_dir(db_dir) == 0) // 생성한 Table이 없을 때
            {
                printf("Entry empty\n");
                return 0;
            }
            else
            {
                print_all_dir(db_dir); // 생성된 Table 출력
            }
        }

        else // handling syntax error
        {
            printf("You have an error in your SQL syntax;\n");
        }
    }

    /*else if (!strcasecmp(command, "create")) // Query > create
    {
        command = strtok(NULL, " "); // database or table

        if (!strcasecmp(command, "database")) // Query > create database
        {
            command = strtok(NULL, ";"); // database name

            if (get_cnt_dir(root) == 0) // 첫 DB 생성
            {
                char *ret = init_dir(root);
                strcpy(db_dir, ret);
                free(ret);
                db = init_db(); // DB 초기화
                head = db;      // DB head 설정
            }

            char *res = read_dir(command, root);

            if (res != NULL) // 같은 이름의 DB 폴더가 이미 존재하는 경우
            {
                printf("Can't create database '%s'; database exists\n", command);
                free(res);
                return 0;
            }

            add_dir(command, root); // root 폴더에 추가

            printf("Query Success!\n");
        }

        else if (!strcasecmp(command, "table")) // Query > create table
        {
            command = strtok(NULL, "("); // table name

            if (command == NULL) // handling syntax error
            {
                printf("You have an error in your SQL syntax;\n");
                return 0;
            }

            if (db == head) // use database 를 하지 않은 상태
            {
                printf("No database selected\n");
                return 0;
            }

            char *res = read_dir("head", db_dir); // Table head 탐색

            if (res == NULL) // Table head 없음
            {
                char *ret = init_dir(db_dir); // DB dir -> Table head
                if (ret != NULL)
                {
                    strcpy(table_dir, ret);
                    free(ret);
                }
                table = init_table(db); // Table 초기화
                db->thead = table;      // DB 구조체와 연결
            }
            else // Table head 존재
            {
                if (table == NULL) // Table dir 존재 && Table 존재x
                {
                    table = init_table(db); // Table 초기화
                    db->thead = table;      // DB 구조체와 연결
                }
                free(res);
            }

            res = read_dir(command, db_dir); // 현재 Table 탐색

            if (res != NULL) // 같은 이름의 Table 폴더가 이미 존재하는 경우
            {
                printf("Table '%s' already exists\n", command);
                free(res);
                return 0;
            }

            create_table(command, db_dir); // DB 폴더에 Table 폴더 생성

            printf("Query Success!\n");
        }

        else // handling syntax error
        {
            printf("You have an error in your SQL syntax;\n");
        }
    }*/

    else if (!strcasecmp(command, "use")) // Query > use database
    {
        command = strtok(NULL, ";"); // DB name

        char *res = read_dir(command, root); // 현재 DB 탐색

        if (res == NULL) // not found db
        {
            printf("Unknown database '%s'\n", command);
            return 0;
        }
        else // found db
        {
            if (db == NULL)
            {
                db = init_db();
                head = db;
            }

            add_db(db, command);
            db = read_db(head, command);

            strcpy(db_dir, res);
            free(res);
            printf("Database changed\n");
        }
    }

    else if (!strcasecmp(command, "drop")) // Query > drop
    {
        command = strtok(NULL, " "); // database

        if (!strcasecmp(command, "database")) // Query > drop database
        {
            command = strtok(NULL, ";"); // database name

            if (get_cnt_dir(root) == 0) // not found db
            {
                printf("Now current DB is NULL\nPlease use another DB\n");
                return 0;
            }

            delete_dir_name(root, command); // DB 삭제

            if (get_cnt_dir(root) == 1) // head만 남은 상태
            {
                char *head = (char *)malloc(100 * sizeof(char));
                sprintf(head, "head_%s", command);
                delete_dir_name(root, head); // head 삭제
            }

            printf("Query Success!\n");
        }

        else if (!strcasecmp(command, "table")) // Query > drop table
        {
            command = strtok(NULL, ";"); // table name

            if (read_dir(command, db_dir) == NULL) // not found table
            {
                printf("Table '%s' doesn't exist\n", command);
                return 0;
            }

            delete_dir_name(db_dir, command); // Table 삭제

            if (get_cnt_dir(db_dir) == 1) // head만 남은 상태
            {
                char *head = (char *)malloc(100 * sizeof(char));
                sprintf(head, "head_%s", command);
                delete_dir_name(db_dir, head); // head 삭제
            }

            printf("Query Success!\n");
        }
    }

    else if (!strcasecmp(command, "desc")) // Query > desc table
    {
        command = strtok(NULL, ";"); // Table name

        if (db == head) // use database 를 하지 않은 상태
        {
            printf("No database selected\n");
            return 0;
        }

        table = read_table(db->thead, command); // 찾는 테이블로 이동

        if (table == NULL) // Not found Table
        {
            printf("Table '%s' doesn't exist\n", command);
            return 0;
        }

        print_all_domain(table); // Table의 Domain 출력
    }

    else if (!strcasecmp(command, "insert")) // Query > insert table
    {
        char table_dir[1024] = {0}; // 테이블 경로

        if (db == head) // use database 하지 않은 상태
        {
            printf("No database selected\n");
            return 0;
        }

        command = strtok(NULL, " "); // into

        int cnt = 0; // cnt: '(' 개수

        for (int i = 0; i < sizeof(query); i++) // insert 쿼리에 columns 목록 존재 여부 확인
        {
            if (query[i] == '(')
                cnt++;
        }

        // command = 테이블명
        if (cnt > 1) // columns(fields) 입력
            command = strtok(NULL, "(");
        else // columns(fields) 생략
            command = strtok(NULL, " ");

        char *res = read_dir(command, db_dir); // 현재 Table 폴더 탐색

        strcpy(table_dir, res); // 현재 Table 폴더 경로

        if (res != NULL)
        {
            if (table == NULL) // Table dir 존재 && Table 존재x
            {
                table = init_table(db); // Table 초기화
                db->thead = table;      // DB 구조체와 연결
            }

            free(res);
        }
        else
        {
            printf("Table '%s' doesn't exist\n", command);
            return 0;
        }

        add_table(db, table, command); // 연결 리스트 -> New Table

        table = read_table(db->thead, command); // Table 구조체 찾기

        char *start = (char *)malloc(1000 * sizeof(char));
        start = strcasestr(origin_query, "values");

        query_insert(table_dir, db, table, start);

        printf("Query Success!\n");
    }

    else if (!strcasecmp(command, "select")) // Query > select table
    {
        char *db_dir = (char *)malloc(1000 * sizeof(char));
        sprintf(db_dir, "%s/chatdb", root);

        query_select(db_dir, origin_query);
    }

    else if (!strcasecmp(command, "update")) // Query > update table
    {
        command = strtok(NULL, " "); // table name

        if (db == head) // not found db
        {
            printf("No database selected\n");
            return 0;
        }

        char *res = read_dir(command, db_dir); // DB 폴더에서 table 폴더 찾기

        if (res == NULL) // not found table
        {
            printf("Table '%s' doesn't exist\n", command);
            return 0;
        }

        else
        {
            if (table == NULL) // Table dir 존재 && Table 존재x
            {
                table = init_table(db); // Table 초기화
                db->thead = table;      // DB 구조체와 연결
            }

            free(res);
        }

        add_table(db, table, command);

        table = read_table(db->thead, command);

        command = strtok(NULL, " "); // set

        query_update(db_dir, db, table, domain, data);
        printf("Query Success!\n");
    }

    else if (!strcasecmp(command, "delete")) // Query > delete table
    {
        command = strtok(NULL, " "); // from

        if (db == head) // not found db
        {
            printf("No database selected\n");
            return 0;
        }

        if (is_where) // where절 존재
        {
            command = strtok(NULL, " "); // table name
        }

        else // where절 존재x
        {
            command = strtok(NULL, ";"); // table name
        }

        char *res = read_dir(command, db_dir);

        if (res == NULL) // not found table
        {
            printf("Table '%s' doesn't exist\n", command);
            return 0;
        }

        else
        {
            if (table == NULL) // Table dir 존재 && Table 존재x
            {
                table = init_table(db); // Table 초기화
                db->thead = table;      // DB 구조체와 연결
            }

            free(res);
        }

        add_table(db, table, command);

        table = read_table(db->thead, command);

        char *start = (char *)malloc(1000 * sizeof(char));

        if (is_where)
        {
            start = strstr(origin_query, "where");
        }

        query_delete(db_dir, db, table, start, is_where);
        printf("Query Success!\n");
    }

    else if (!strcasecmp(command, "exit\n"))
    {
        printf("Bye~\n");
        return 0;
    }

    else if (command[0] == '\n')
    {
        return 0;
    }

    else if (strstr(query, ";") == NULL) // 세미콜론 포함x
    {
        printf("Complete the command, including the ';'\n");
        return 0;
    }

    else // not exists command
    {
        printf("You have an error in your SQL syntax;\n");
    }

    return 0;
}

#endif