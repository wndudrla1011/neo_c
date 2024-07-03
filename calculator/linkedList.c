#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

int index;

char *cstrtok(char *str);

struct stack
{
    int top;
    int size;
    int *arr;
    void (*push)(struct stack *, int);
    int (*pop)(struct stack *);
    bool (*isEmpty)(struct stack *);
};

bool isEmpty(struct stack *s)
{
    if (s->top < 0)
        return true;
    else
        return false;
}

void push(struct stack *s, int value)
{
    s->arr[++(s->top)] = value;
}

int pop(struct stack *s)
{
    if (!s->isEmpty(s))
        return s->arr[(s->top)--];
    return -1;
}

struct stack *init(int size)
{
    struct stack *s = (struct stack *)calloc(1, sizeof(struct stack));
    s->top = -1;
    s->size = size;
    s->arr = (int *)calloc(size, sizeof(int));
    s->push = push;
    s->pop = pop;
    s->isEmpty = isEmpty;
}

int main(void)
{
    struct stack *my_stack = init(10); // 스택 초기화

    char *exp = malloc(sizeof(char) * 10); // 수식 입력값
    char *token = exp;                     // 문자열 조각

    printf("계산식을 입력한 후 Enter를 눌러주세요!\n");
    scanf("%s", exp);

    if (exp == NULL)
        return 0;

    while (token != NULL)
    {
        puts(token);
        token = &exp[index++];
    }

    free(exp);
    free(my_stack->arr);
    free(my_stack);

    return 0;
}

int cstrlen(char *str)
{
    int idx = 0;
    while (str[idx] != '\0')
        ++idx;
    return idx;
}

char *cstrtok(char *str)
{
    char *start = 0;  // 문자열 시작 위치
    static char *tmp; // 문자열 주소 저장

    if (str != NULL) // 첫 토큰 받을 때
        start = str;
    else // 두 번째 이후 토큰 받을 때
        start = tmp;

    if (cstrlen(start) < 1) // 문자열 종료
        return NULL;

    tmp = &start[++index]; // 구분자 다음 위치

    return start;
}