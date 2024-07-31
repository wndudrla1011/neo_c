#ifndef CREATE_TABLE_H
#define CREATE_TABLE_H

#include "../util/getLen.h"
#include "../util/directory.h"
#include "../db.h"
#include "../table.h"
#include "../domain.h"
#include <string.h>
#include <ctype.h>

char *types[] = {"int", "INT", "bigint", "BIGINT", "varchar", "VARCHAR", "text", "TEXT"};

void create_table(char *name, char *parent, DB *db, Table *table, Domain *domain)
{
    char table_dir[MAX_INPUT] = {0}; // 테이블 경로
    char domainName[MAX_INPUT] = {0};
    char domain_dir[MAX_INPUT] = {0};

    add_table(db, table, name); // 연결 리스트 -> New Table

    add_dir(name, parent); // Table 폴더 생성

    table = read_table(db->thead, name); // 현재 Table 탐색

    strcpy(table_dir, read_dir(name, parent)); // 현재 Table 경로 저장

    domain = init_domain(table); // Table을 찾은 후 Domain 초기화 및 연결

    char *attr_info[MAX_INPUT]; // create table 을 Tokenizer 한 결과 저장
    char *column = NULL;
    char type[MAX_INPUT];
    char *nullable = NULL;

    int cnt = 0;             // create token 개수
    int flag = 1;            // NOT NULL, NULL 구분용
    int type_flag = 0;       // 현재 데이터가 type인지 여부
    int len = 0;             // 데이터 길이
    char *token = NULL;      // 데이터 도메인 토큰
    char *type_token = NULL; // 타입 + 길이

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

                int k = 0;
                for (k = 0; k < strlen(type_token); k++)
                {
                    type[k] = tolower(type_token[k]);
                }
                type[k] = '\0'; // 마지막 문자 NULL

                len = getLen(type_token, types[j]); // 타입에 맞는 데이터 길이를 구함

                type_flag = 1; // 현재 토큰은 타입
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
                nullable = "NOTNULL";
            else // NULL
                nullable = "NULL";

            // Domain 1개 종료

            flag = 1, type_flag = 0; // flag 초기화

            add_domain(domain, column, type, len, nullable); // Domain 추가

            char *res = init_dir(table_dir);

            free(res);

            strcat(domainName, "_");
            strcat(domainName, column);
            strcat(domainName, "_");
            strcat(domainName, type);
            strcat(domainName, "_");

            char len_str[1024];
            sprintf(len_str, "%d", len);

            strcat(domainName, len_str);
            strcat(domainName, "_");
            strcat(domainName, nullable);

            domain = table->dhead->next; // 첫 번째 column 이동

            if (domain != NULL) // 각 Domain 마다 Data head 생성
            {
                while (1)
                {
                    init_data(domain);
                    if (domain->next == NULL)
                    {
                        break;
                    }
                    domain = domain->next;
                }
            }

            sprintf(domain_dir, "%s/%s", table_dir, domainName); // 경로 생성
            createDirectory(domain_dir);                         // 폴더 생성
            domainName[0] = '\0';                                // domainName 초기화

            continue;
        }

        if (type_flag == 0) // 현재 토큰이 타입이 아님
            column = attr_info[i];
    }
}

#endif