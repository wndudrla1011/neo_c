#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define MAX_SIZE 100

void push(int value);
int pop();

int priority(char op);

int main(void)
{
    char *exp; // 입력
    char postfix[MAX_SIZE] = {'\0'};
    int result;
    exp = (char *)malloc(sizeof(char) * 100);

    printf("계산식을 입력한 후 Enter를 눌러주세요!\n");
    scanf("%s", exp);

    return 0;
}

void push(int value)
{
    stack[++top] = value;
}

int pop()
{
    return stack[top--];
}

int priority(char op)
{
    switch (op)
    {
    case '(':
    case ')':
        return 0;
    case '+':
    case '-':
        return 1;
    case '*':
    case '/':
        return 2;
    }
    return -1;
}