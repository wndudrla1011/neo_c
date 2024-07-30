#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

int directoryExists(const char* dirName) { //디렉토리 존재 여부 확인
    struct stat info;
    
    if (stat(dirName, &info) != 0) {
        return 0;  // 디렉토리가 존재하지 않음
    } else if (info.st_mode & S_IFDIR) {
        return 1;  // 디렉토리가 존재함
    } else {
        return 0;  // 다른 종류의 파일이 존재함
    }
}

int createDirectory(const char* dirName) { // 디렉토리 생성
    if (mkdir(dirName, 0777) == 0) {
        printf("디렉토리 생성 성공: %s\n", dirName);
        return 0;
    } else {
        perror("디렉토리 생성 실패");
        return -1;
    }
}

int renameDirectory(const char *oldName, const char *newName) //디렉토리 이름 변경
{
    if (!rename(oldName, newName))
    {
        printf("디렉토리 이름 변경 성공: %s -> %s\n", oldName, newName);
        return 0;
    } else {
        perror("디렉토리 이름 변경 실패");
        return -1;
    }
}

char *init_dir(char *parent)
{
    char path[1024];
    char *ptr;

    sprintf(path, "%s/head", parent);

    if (directoryExists(path))
    {
        printf("디렉토리가 이미 존재합니다: %s\n", path);
    }
    else
    {
        createDirectory(path);
    }

    ptr = path;
    return ptr;
}

int get_cnt_dir(const char *parent)
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

char *fread_dir(char *dname, char *path) // DB 폴더로 DB 찾기
{
    char db_path[1024];
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

#endif