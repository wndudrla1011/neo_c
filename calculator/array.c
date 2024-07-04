#include <stdio.h>
#include <stdlib.h>
#define MAX_SIZE 100

int top = -1; // stack (연산자 배열) top index

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
                postfix[p++] = pop(stack);
            }
            push(stack, ch);
            break;
        case '(':
            push(stack, ch);
            break;
        case ')':
            while (isEmpty() == 0 && stack[top] != '(') // find '('
            {
                postfix[p++] = pop(stack);
            }
            pop(stack); //'(' 제거
            break;
        default: // digit
            postfix[p++] = ch;
            break;
        }
    }

    while (!isEmpty())
    {
        postfix[p++] = pop(stack);
    }
}

int calculate(char postfix[])
{
    int len = cstrlen(postfix);
    char ch;
    int v1, v2;    // 계산할 값
    char op1, op2; // 계산할 값 (숫자 변환 전)
    char stack[MAX_SIZE];

    for (int i = 0; i < len; i++)
    {
        ch = postfix[i];
        if (ch != '*' && ch != '/' && ch != '+' && ch != '-')
        {
            push(stack, ch);
        }

        else
        {
            op2 = pop(stack);
            op1 = pop(stack);

            // char -> int
            v1 = op1 - 48;
            v2 = op2 - 48;

            switch (ch)
            {
            case '+':
                push(stack, (v1 + v2) + 48);
                break;
            case '-':
                push(stack, (v1 - v2) + 48);
                break;
            case '*':
                push(stack, (v1 * v2) + 48);
                break;
            case '/':
                push(stack, (v1 / v2) + 48);
                break;
            }
        }
    }

    return pop(stack) - 48;
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
    printf("\nPostfix: %s\n", postfix);

    printf("계산한 결과: \t%d\n", calculate(postfix));

    return 0;
}