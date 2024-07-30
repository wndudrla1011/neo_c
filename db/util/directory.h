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

#endif