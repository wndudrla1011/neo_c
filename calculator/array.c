#include <stdio.h>
#include <stdlib.h>
#define MAX_SIZE 100

int top = -1;

int isEmpty()
{
    if (top < 0) // empty
        return 1;
    else
        return 0;
}

void push(char stack[], char op)
{
    stack[++top] = op;
}

char pop(char stack[])
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

int cstrlen(char *str)
{
    int idx = 0;
    while (str[idx] != '\0')
        ++idx;
    return idx;
}

void toPostfix(char exp[], char postfix[])
{
    int k, p; // k : stack 배열 인덱스, p : postfix 배열 인덱스
    int len = cstrlen(exp);
    char ch;
    char stack[MAX_SIZE];

    for (int i = 0; i < len; i++)
    {
        ch = exp[i];
        switch (ch)
        {
        case '*':
        case '/':
        case '+':
        case '-':
            while (isEmpty() == 0 && priority(ch) <= priority(stack[top]))
            {
                printf("postfix 추가 + pop() > %c\n", stack[top]);
                postfix[p++] = pop(stack);
            }
            printf("push() > %c\n", ch);
            push(stack, ch);
            break;
        case '(':
            printf("push() > %c\n", ch);
            push(stack, ch);
            break;
        case ')':
            while (isEmpty() == 0 && stack[top] != '(') // find '('
            {
                printf("postfix 추가 + pop() > %c\n", stack[top]);
                postfix[p++] = pop(stack);
            }
            printf("pop() > %c\n", stack[top]);
            pop(stack); //'(' 제거
            break;
        default: // digit
            printf("postfix 추가 > %c\n", ch);
            postfix[p++] = ch;
            break;
        }
    }

    while (!isEmpty())
    {
        postfix[p++] = pop(stack);
    }
}

int main(void)
{
    char *exp; // 입력
    char postfix[MAX_SIZE] = {'\0'};
    int result;
    exp = (char *)malloc(sizeof(char) * 100);

    printf("계산식을 입력한 후 Enter를 눌러주세요!\n");
    scanf("%s", exp);

    toPostfix(exp, postfix);
    printf("%s\n", postfix);

    return 0;
}