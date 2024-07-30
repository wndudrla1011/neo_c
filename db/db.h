#ifndef DB_H
#define DB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "./util/directory.h"

typedef struct DB
{
    char dname[MAX_INPUT]; // DB명
    int tcnt;              // 테이블 개수
    struct DB *next;       // 다음 DB 포인터
    struct Table *thead;   // 테이블 목록 (Table 순차 검색)
} DB;

DB *init_db(char *parent) // DB 목록의 head 생성
{
    char db_path[MAX_INPUT];
    DB *head;
    head = (DB *)malloc(sizeof(DB));
    head->next = NULL;

    sprintf(db_path, "%s/head", parent);

    if (directoryExists(db_path))
    {
        printf("디렉토리가 이미 존재합니다: %s\n", db_path);
    }
    else
    {
        createDirectory(db_path);
    }

    return (head);
}

int get_cnt_db(DB *h) // 생성된 DB 개수
{
    if (h == NULL) // DB를 생성하기 전 상황
        return 0;

    int cnt = -1;
    DB *cur = h;

    while (cur != NULL)
    {
        cur = cur->next;
        cnt++;
    }

    return cnt;
}

int fget_cnt_db(const char *parent)
{
    DIR *dir;
    struct dirent *entry;
    struct stat info;
    int count = 0;
    char path[1024];

    if ((dir = opendir(parent)) == NULL)
    {
        perror("디렉토리를 열 수 없습니다");
        return -1;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        snprintf(path, sizeof(path), "%s/%s", parent, entry->d_name);

        if (stat(path, &info) != 0)
        {
            perror("stat 실패");
            closedir(dir);
            return -1;
        }

        if (S_ISDIR(info.st_mode))
        {
            count++;
        }
    }

    closedir(dir);
    return count;
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

char *ffind_end_db(const char *dirName) // 가장 최근에 생성한 폴더 찾기
{
    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(dirName)) == NULL)
    {
        perror("디렉토리를 열 수 없습니다");
        return NULL;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) // '.' 및 '..' 디렉토리 무시
        {
            if (strstr(entry->d_name, "_") == NULL) // Leaf Folder
            {
                return entry->d_name;
            }
        }
    }

    return "head";
}

void add_db(DB *db, char *name, char *parent) // 마지막 노드에 새 DB 추가
{
    char db_path[MAX_INPUT];
    char *end;

    // 디렉토리 처리
    char oldName[MAX_INPUT];
    char newName[MAX_INPUT];
    end = ffind_end_db(parent); // 부모 폴더에서 가장 최근에 생성한 DB 폴더 찾기
    sprintf(oldName, "%s/%s", parent, end);
    sprintf(newName, "%s/%s_%s", parent, end, name); // 가장 최근 생성한 DB -> 새로운 DB
    renameDirectory(oldName, newName);               // 연결 네이밍
    sprintf(db_path, "%s/%s", parent, name);         // 새 DB 폴더 생성

    createDirectory(db_path); // 새로운 DB 폴더 생성

    // 메모리 처리
    // DB *end;
    // end = find_end_db(db); // Leaf DB
    // DB *new_db;
    // new_db = (DB *)malloc(sizeof(DB)); // 새 DB 생성
    // strcpy(new_db->dname, name);       // DB명 입력
    // end->next = new_db;                // last db -> new db
    // new_db->next = NULL;
}

void print_all_db(DB *h) // 모든 DB 출력
{
    DB *cur;
    cur = h->next;

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

void fprint_all_db(char *parent)
{
    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(parent)) == NULL) // cd joosql
    {
        perror("디렉토리를 열 수 없습니다");
        return;
    }

    printf("======================================\n");
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) // '.' 및 '..' 디렉토리 무시
        {
            if (strstr(entry->d_name, "head") != NULL) // head는 출력하지 않음
            {
                continue;
            }
            else if (strstr(entry->d_name, "_") != NULL) // next ptr 를 가진 경우
            {
                printf("%s\n", strtok(entry->d_name, "_"));
            }
            else
            {
                printf("%s\n", entry->d_name);
            }
        }
    }
    printf("======================================\n\n");
}

DB *read_db(DB *h, char *dname) // DB 이름으로 DB 찾기
{
    if (h == NULL)
        return (h);

    DB *cur;
    cur = h;

    while (strcmp(cur->dname, dname) && cur->next != NULL)
    {
        cur = cur->next;
    }

    if (cur->next == NULL && strcmp(cur->dname, dname)) // Not found DB
    {
        return NULL;
    }

    return (cur);
}

char *fread_db(DB *h, char *dname, char *path) // DB 폴더로 DB 찾기
{
    char db_path[MAX_INPUT];
    sprintf(db_path, "%s/%s", path, dname);

    char *ptr = db_path; // 반환할 문자열 (db 경로)

    if (directoryExists(db_path)) // DB 폴더 존재
    {
        return ptr;
    }
    else
    {
        return NULL;
    }
}

void delete_db(DB *h, char *name) // DB 삭제
{
    DB *prev;
    DB *cur; // 삭제할 DB
    cur = h;

    while (strcmp(cur->dname, name) && cur->next != NULL)
    {
        if (!strcmp(cur->next->dname, name))
            prev = cur;
        cur = cur->next;
    }

    prev->next = cur->next;
    cur->next = NULL;

    free(cur);
}

#endif