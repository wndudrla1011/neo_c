#include <stdio.h>
#include <string.h>
#include <direct.h>
#include <dirent.h>

#define MAX_COLUMN 20      // 최대 속성 값 개수
#define MAX_INPUT 20       // 최대 입력 값 길이
#define MAX_CADINALITY 200 // 최대 튜플 개수

typedef struct
{
    char dname[MAX_INPUT]; // DB명
    int tcnt;              // 테이블 개수
    struct DB *next;       // 다음 DB 포인터
    struct Table *thead;   // 테이블 목록 (Table 순차 검색)
} DB;

typedef struct
{
    char tname[MAX_INPUT];               // 테이블명
    int cadinality;                      // 튜블 개수 == tuple 배열의 top
    struct Column tuple[MAX_CADINALITY]; // 튜플 선언
} Table;

struct Column
{
    char nick[MAX_INPUT];     // 닉네임
    char name[MAX_INPUT];     // 이름
    unsigned long long phone; // 휴대폰 번호 (최대 12자리 양수)
};

int main(void)
{

    return 0;
}