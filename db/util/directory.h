#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "substring.h"

DIR *dir;

// Comparator function for qsort
int compare(const void *a, const void *b)
{
    return strcmp(*(const char **)a, *(const char **)b);
}

int directoryExists(const char *dirName)
{ // 디렉토리 존재 여부 확인
    struct stat info;

    if (stat(dirName, &info) != 0)
    {
        return 0; // 디렉토리가 존재하지 않음
    }
    else if (info.st_mode & S_IFDIR)
    {
        return 1; // 디렉토리가 존재함
    }
    else
    {
        return 0; // 다른 종류의 파일이 존재함
    }
}

int createDirectory(const char *dirName)
{ // 디렉토리 생성
    if (mkdir(dirName, 0777) == 0)
    {
        printf("디렉토리 생성 성공: %s\n", dirName);
        return 0;
    }
    else
    {
        perror("디렉토리 생성 실패");
        return -1;
    }
}

int renameDirectory(const char *oldName, const char *newName) // 디렉토리 이름 변경
{
    if (!rename(oldName, newName))
    {
        printf("디렉토리 이름 변경 성공: %s -> %s\n", oldName, newName);
        return 0;
    }
    else
    {
        perror("디렉토리 이름 변경 실패");
        return -1;
    }
}

char *init_dir(const char *parent)
{
    char path[1024] = {0};
    char *ptr = (char *)malloc(1024 * sizeof(char));

    sprintf(path, "%s/head", parent);

    if (directoryExists(path))
    {
        printf("디렉토리가 이미 존재합니다: %s\n", path);
    }
    else
    {
        createDirectory(path);
    }

    strcpy(ptr, path);
    return ptr;
}

int get_cnt_dir(const char *parent) // 디렉토리 내 폴더 개수 카운팅
{
    DIR *dir;
    struct dirent *entry;
    struct stat info;
    int count = 0;
    char path[1024];

    if ((dir = opendir(parent)) == NULL)
    {
        perror("디렉토리를 열 수 없습니다");
        closedir(dir);
        return -1;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        sprintf(path, "%s/%s", parent, entry->d_name);

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

char *read_dir(char *name, char *parent) // 폴더명으로 폴더 찾기
{
    struct dirent *entry;
    char *path = NULL;
    char *lt = NULL, *rt = NULL;

    if ((dir = opendir(parent)) == NULL)
    {
        perror("디렉토리를 열 수 없습니다");
        return NULL;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) // '.' 및 '..' 디렉토리 무시
        {
            if (strstr(entry->d_name, "_") != NULL) // Leaf folder가 아닌 경우 ("_" 존재)
            {
                lt = strtok(entry->d_name, "_"); // 폴더명만 토큰화
                rt = strtok(NULL, "_");          // 다음 폴더명 토큰화
                if (!strcmp(name, lt))           // 동일 폴더인지 비교
                {
                    path = (char *)malloc(sizeof(strlen(parent) + strlen(lt) + strlen(rt) + 3));
                    sprintf(path, "%s/%s_%s", parent, lt, rt);
                    break;
                }
            }
            else // Leaf folder
            {
                if (!strcmp(name, entry->d_name)) // 동일 폴더인지 비교
                {
                    path = (char *)malloc(sizeof(strlen(parent) + strlen(name) + 3));
                    sprintf(path, "%s/%s", parent, name);
                    break;
                }
            }
        }
    }

    closedir(dir);

    if (path != NULL && directoryExists(path)) // 폴더 존재
    {
        return path;
    }
    else
    {
        return NULL;
    }
}

char *find_end_dir(const char *dirName) // 가장 최근에 생성한 폴더 찾기
{
    DIR *dir = NULL;
    struct dirent *entry;
    char *head;
    head = (char *)malloc(10 * sizeof(char));
    strcpy(head, "head");

    char tmp[100000];

    strcpy(tmp, dirName);
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
                closedir(dir);
                return entry->d_name;
            }
        }
    }

    closedir(dir);
    return head;
}

void add_dir(char *name, const char *parent) // 마지막 노드에 새 폴더 추가
{
    char path[1024] = {0};
    char *end;

    // 디렉토리 처리
    char oldName[1024] = {0};
    char newName[1024] = {0};

    end = find_end_dir(parent);                      // 부모 폴더에서 가장 최근에 생성한 폴더 찾기
    sprintf(oldName, "%s/%s", parent, end);          // 기존 폴더명
    sprintf(newName, "%s/%s_%s", parent, end, name); // 변경할 폴더명
    renameDirectory(oldName, newName);               // 폴더명 변경
    sprintf(path, "%s/%s", parent, name);            // 새 폴더 생성

    createDirectory(path); // 새로운 폴더 생성
}

void print_all_dir(char *parent)
{
    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(parent)) == NULL) // cd joosql
    {
        perror("디렉토리를 열 수 없습니다");
        closedir(dir);
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

    closedir(dir);
}

void save_data_dir(char *path, int index, int row, char *data)
{
    DIR *dir;
    struct dirent *entry;
    char store[MAX_INPUT] = {0};
    char name[MAX_INPUT] = {0};

    if ((dir = opendir(path)) == NULL)
    {
        perror("디렉토리를 열 수 없습니다");
        closedir(dir);
        return;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_name[0] == (index + '0'))
        {
            sprintf(name, "%d_%s", row, data);
            sprintf(store, "%s/%s/%s", path, entry->d_name, name);
            createDirectory(store);
            break;
        }
    }

    closedir(dir);
}

char *find_data_dir(char *path, int row) // Domain에서 row번째 데이터 찾기
{
    DIR *dir = opendir(path);
    struct dirent *entry;

    if (dir == NULL)
    {
        perror("디렉토리를 열 수 없습니다");
        return NULL;
    }

    while ((entry = readdir(dir)) != NULL) // Data dir 순회
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        char *token = (char *)malloc(100 * sizeof(char));
        token = strtok(entry->d_name, "_");
        int row_number = atoi(token); // 행 번호

        if (row_number == row) // 탐색하는 행 데이터
        {
            token = strtok(NULL, "_"); // 데이터 값
            closedir(dir);
            return token;
        }
    }

    closedir(dir);
}

char *find_data_dirName(char *path, int row) // Domain에서 row번째 데이터의 경로 반환
{
    DIR *dir = opendir(path);
    struct dirent *entry;
    char *token = NULL;

    if (dir == NULL)
    {
        perror("디렉토리를 열 수 없습니다");
        return NULL;
    }

    while ((entry = readdir(dir)) != NULL) // Domain 폴더 open
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        char origin[100] = {0}; // 원본 폴더명
        strcpy(origin, entry->d_name);

        token = strtok(entry->d_name, "_");
        int row_number = atoi(token); // 행 번호

        if (row_number == row) // 탐색하는 행 데이터
        {
            char *data_dir = (char *)malloc(1000 * sizeof(char)); // 기존 절대 경로
            sprintf(data_dir, "%s/%s", path, origin);

            closedir(dir);

            return data_dir;
        }
    }

    closedir(dir);
}

char *create_dir(char *path, int row, char *newDirName)
{
    DIR *dir = opendir(path);
    struct dirent *entry;
    char *token = NULL;

    if (dir == NULL)
    {
        perror("디렉토리를 열 수 없습니다");
        return NULL;
    }

    while ((entry = readdir(dir)) != NULL) // Domain 폴더 open
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        token = strtok(entry->d_name, "_");
        int row_number = atoi(token); // 행 번호

        if (row_number == row) // 탐색하는 행 데이터
        {
            char *new_dir = (char *)malloc(100 * sizeof(char)); // 새 절대 경로
            sprintf(new_dir, "%s/%d_%s", path, row_number, newDirName);

            closedir(dir);

            return new_dir;
        }
    }
}

/*
 * 다중 조건에 맞는 Tuple 검색
 * row: 튜플 순서
 * col1: 조건1 컬럼
 * val1: 조건1 값
 * op1: 조건1 연산자
 * col2: 조건2 컬럼
 * val2: 조건2 값
 * op2: 조건2 연산자
 */
int find_multi_dir(int row, char *path, char *col1, char *val1, char op1, char *col2, char *val2, char op2, int flag)
{
    int result1 = 0; // 조건 부합 => 1
    int result2 = 0; // 조건 부합 => 1

    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(path)) == NULL)
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

        char *token = (char *)malloc(100 * sizeof(char));
        char origin[100] = {0};
        strcpy(origin, entry->d_name);
        token = strtok(entry->d_name, "-"); // idx
        token = strtok(NULL, "-");          // column

        char *domain_dir = (char *)malloc(1000 * sizeof(char));
        sprintf(domain_dir, "%s/%s", path, origin);

        if (!strcmp(token, col1)) // 컬럼 일치
        {
            token = strtok(NULL, "-"); // type

            if (strstr(token, "int")) // 숫자 타입
            {
                int limit = atoi(val1); // 제한 값
                char *res = find_data_dir(domain_dir, row);
                int item = atoi(res); // 타깃 값

                switch (op1)
                {
                case '<':
                    if (item < limit)
                        result1 = 1;
                    break;
                case '>':
                    if (item > limit)
                        result1 = 1;
                    break;
                case '=':
                    if (item == limit)
                        result1 = 1;
                    break;
                case '!':
                    if (item != limit)
                        result1 = 1;
                    break;
                default:
                    break;
                }
            }

            else // 문자열 타입
            {
                char *limit = substring(1, strlen(val1) - 2, val1); // 제한 값
                char *res = find_data_dir(domain_dir, row);
                char *item = res; // 타깃 값

                switch (op1)
                {
                case '<':
                    if (strcmp(item, limit) == -1)
                        result1 = 1;
                    break;
                case '>':
                    if (strcmp(item, limit) == 1)
                        result1 = 1;
                    break;
                case '=':
                    if (!strcmp(item, limit))
                        result1 = 1;
                    break;
                case '!':
                    if (strcmp(item, limit))
                        result1 = 1;
                    break;
                default:
                    break;
                }
            }

            closedir(dir);

            break;
        } // diff column
    } // readdir

    if (flag == 1) // and 연산
    {
        if (!result1) // false
            return result1;
    }

    if ((dir = opendir(path)) == NULL)
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

        char *token = (char *)malloc(100 * sizeof(char));
        char origin[100] = {0};
        strcpy(origin, entry->d_name);
        token = strtok(entry->d_name, "-"); // idx
        token = strtok(NULL, "-");          // column

        char *domain_dir = (char *)malloc(1000 * sizeof(char));
        sprintf(domain_dir, "%s/%s", path, origin);

        if (!strcmp(token, col2)) // 컬럼 일치
        {
            token = strtok(NULL, "-"); // type

            if (strstr(token, "int")) // 숫자 타입
            {
                int limit = atoi(val2);                          // 제한 값
                int item = atoi(find_data_dir(domain_dir, row)); // 타깃 값

                switch (op2)
                {
                case '<':
                    if (item < limit)
                        result2 = 1;
                    break;
                case '>':
                    if (item > limit)
                        result2 = 1;
                    break;
                case '=':
                    if (item == limit)
                        result2 = 1;
                    break;
                case '!':
                    if (item != limit)
                        result2 = 1;
                    break;
                default:
                    break;
                }
            }

            else // 문자열 타입
            {
                char *limit = substring(1, strlen(val2) - 2, val2); // 제한 값
                char *res = find_data_dir(domain_dir, row);
                char *item = res; // 타깃 값

                switch (op2)
                {
                case '<':
                    if (strcmp(item, limit) == -1)
                        result2 = 1;
                    break;
                case '>':
                    if (strcmp(item, limit) == 1)
                        result2 = 1;
                    break;
                case '=':
                    if (!strcmp(item, limit))
                        result2 = 1;
                    break;
                case '!':
                    if (strcmp(item, limit))
                        result2 = 1;
                    break;
                default:
                    break;
                }
            }

            closedir(dir);

            return flag == 1 ? result1 & result2 : result1 | result2;

        } // diff column
    } // readdir
}

int find_single_dir(int row, char *path, char *col, char *val, char op)
{
    int result = 0; // 조건 부합 => 1

    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(path)) == NULL)
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

        char *token = (char *)malloc(100 * sizeof(char));
        char origin[100] = {0};
        strcpy(origin, entry->d_name);
        token = strtok(entry->d_name, "-"); // idx
        token = strtok(NULL, "-");          // column

        char *domain_dir = (char *)malloc(1000 * sizeof(char));
        sprintf(domain_dir, "%s/%s", path, origin);

        if (!strcmp(token, col)) // 컬럼 일치
        {
            token = strtok(NULL, "-"); // type

            if (strstr(token, "int") != NULL) // 숫자 타입
            {
                int limit = atoi(val); // 제한 값
                char *res = find_data_dir(domain_dir, row);
                int item = atoi(res); // 타깃 값

                switch (op)
                {
                case '<':
                    if (item < limit)
                        result = 1;
                    break;
                case '>':
                    if (item > limit)
                        result = 1;
                    break;
                case '=':
                    if (item == limit)
                        result = 1;
                    break;
                case '!':
                    if (item != limit)
                        result = 1;
                    break;
                default:
                    break;
                }
            }

            else // 문자열 타입
            {
                char *limit = substring(1, strlen(val) - 2, val); // 제한 값
                char *res = find_data_dir(domain_dir, row);
                char *item = res; // 타깃 값

                switch (op)
                {
                case '<':
                    if (strcmp(item, limit) == -1)
                        result = 1;
                    break;
                case '>':
                    if (strcmp(item, limit) == 1)
                        result = 1;
                    break;
                case '=':
                    if (!strcmp(item, limit))
                        result = 1;
                    break;
                case '!':
                    if (strcmp(item, limit))
                        result = 1;
                    break;
                default:
                    break;
                }
            }

            closedir(dir);

            return result;

        } // diff column
    } // readdir
}

void select_all_dir(int row, char *path) // tuple 출력
{
    DIR *dir;
    struct dirent *entry;
    char sub[MAX_INPUT] = {0};
    char result[MAX_INPUT] = {0};

    if ((dir = opendir(path)) == NULL)
    {
        perror("디렉토리를 열 수 없습니다");
        closedir(dir);
        return;
    }

    printf("+--------------------------------------+\n");
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        sprintf(sub, "%s/%s", path, entry->d_name);
        strcpy(result, find_data_dir(sub, row));
        printf("|  %s  |  ", result);
        sub[0] = '\0';
        result[0] = '\0';
    }
    printf("\n+--------------------------------------+\n");

    closedir(dir);
}

void select_cols_dir(int row, char *path, char *col) // 특정 필드 출력
{
    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(path)) == NULL)
    {
        perror("디렉토리를 열 수 없습니다");
        return;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        char *token = (char *)malloc(100 * sizeof(char));
        char origin[100] = {0};
        strcpy(origin, entry->d_name);
        token = strtok(entry->d_name, "-"); // idx
        token = strtok(NULL, "-");          // column

        char *domain_dir = (char *)malloc(1000 * sizeof(char));
        sprintf(domain_dir, "%s/%s", path, origin);

        if (!strcmp(token, col)) // 컬럼 일치
        {
            char *res = find_data_dir(domain_dir, row);

            printf("|  %s  |  ", res);

            closedir(dir);

            break;
        }
    }
}

/*
 * col: 수정하려는 컬럼
 * val: 수정하려는 값
 */
void update_dir(int row, char *path, char *col, char *val)
{
    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(path)) == NULL)
    {
        perror("디렉토리를 열 수 없습니다");
        return;
    }

    while ((entry = readdir(dir)) != NULL) // Table 폴더 open
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        char *token = (char *)malloc(100 * sizeof(char));
        char origin[100] = {0}; // 원본 폴더명
        strcpy(origin, entry->d_name);
        token = strtok(entry->d_name, "-"); // idx
        token = strtok(NULL, "-");          // column

        char *domain_dir = (char *)malloc(1000 * sizeof(char)); // 오픈할 Domain 경로
        sprintf(domain_dir, "%s/%s", path, origin);

        if (!strcmp(token, col)) // 컬럼 일치
        {
            char *old_data_dir = (char *)malloc(1000 * sizeof(char)); // 기존 절대 경로
            old_data_dir = find_data_dirName(domain_dir, row);

            char *new_data_dir = (char *)malloc(1000 * sizeof(char)); // 변경할 절대 경로

            if (val[0] == '\'' && val[strlen(val) - 1] == '\'') // 문자열 데이터
            {
                new_data_dir = create_dir(domain_dir, row, substring(1, strlen(val) - 2, val));
            }

            else // 숫자 데이터
            {
                new_data_dir = create_dir(domain_dir, row, val);
            }

            renameDirectory(old_data_dir, new_data_dir); // 폴더명 변경

            closedir(dir);

            break;
        }
    }
}

void delete_dir(int row, char *path)
{
    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(path)) == NULL)
    {
        perror("디렉토리를 열 수 없습니다");
        return;
    }

    while ((entry = readdir(dir)) != NULL) // Domain dir 순회
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        char origin[100] = {0}; // 원본 폴더명
        strcpy(origin, entry->d_name);

        char *domain_dir = (char *)malloc(100 * sizeof(char)); // 오픈할 Domain 경로
        sprintf(domain_dir, "%s/%s", path, origin);

        int last = get_cnt_dir(domain_dir); // 마지막 행 번호

        char *data_dir = (char *)malloc(100 * sizeof(char)); // Data 경로
        data_dir = find_data_dirName(domain_dir, row);       // 삭제하려는 Data 경로

        char *data = (char *)malloc(100 * sizeof(char)); // 현재 행의 Data
        data = find_data_dir(domain_dir, row);

        if (rmdir(data_dir) == 0) // 데이터 삭제 성공
        {
            printf("성공적으로 %s가 삭제되었습니다.\n", data_dir);
        }
        else // 데이터 삭제 실패
        {
            perror("삭제 에러");
            return;
        }

        for (int i = row + 1; i < last; i++) // 행 번호 조정
        {
            char *origin_dir = (char *)malloc(1000 * sizeof(char)); // i번째 행의 기존 경로
            origin_dir = find_data_dirName(domain_dir, i);

            char *data = (char *)malloc(100 * sizeof(char)); // i번째 행의 Data
            data = find_data_dir(domain_dir, i);

            if (directoryExists(origin_dir))
            {
                rmdir(origin_dir);
            }

            char *new_dir = (char *)malloc(1000 * sizeof(char)); // i번째 행의 new 경로
            sprintf(new_dir, "%s/%d_%s", domain_dir, i - 1, data);

            createDirectory(new_dir); // 뒷 번호를 당긴 폴더 생성
        }
    }

    closedir(dir);
}

int delete_recur_dir(char *path) // 하위 디렉터리 모두 삭제
{
    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(path)) == NULL)
    {
        perror("디렉토리를 열 수 없습니다");
        return;
    }

    while ((entry = readdir(dir)) != NULL) // path 디렉터리 순회
    {
        char fullpath[1024];
        struct stat statbuf;

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

        if (stat(fullpath, &statbuf) == -1)
        {
            perror("stat");
            closedir(dir);
            return -1;
        }

        if (S_ISDIR(statbuf.st_mode)) // 폴더인 경우 재귀적으로 삭제
        {
            if (delete_recur_dir(fullpath) == -1)
            {
                closedir(dir);
                return -1;
            }
        }

        else // 파일인 경우 삭제
        {
            if (remove(fullpath) == -1)
            {
                perror("remove");
                closedir(dir);
                return -1;
            }
        }
    }

    closedir(dir);

    if (rmdir(path) == -1)
    {
        perror("rmdir");
        return -1;
    }

    return 0;
}

void delete_dir_name(char *path, char *name) // name과 동일 이름의 폴더 삭제
{
    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(path)) == NULL)
    {
        perror("디렉토리를 열 수 없습니다");
        return;
    }

    while ((entry = readdir(dir)) != NULL) // DB 디렉터리 순회
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        if (!strcmp(entry->d_name, name)) // 동일 이름
        {
            char *db_dir = (char *)malloc(100 * sizeof(char));
            sprintf(db_dir, "%s/%s", path, name);

            delete_recur_dir(db_dir); // 하위 디렉터리 모두 삭제

            closedir(dir);

            break;
        }
    }
}

#endif