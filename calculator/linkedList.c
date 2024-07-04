#include <stdio.h>
#include <stdlib.h>
#define MAX_SIZE 100

int top = -1;     // top index of char stack
int top_int = -1; // top index of int stack

typedef struct
{
    int item;
    struct Node *link;
} Node;

typedef struct
{
    Node *head;
} LinkedList;

void init(LinkedList *L)
{
    L->head = NULL;
}

int isEmpty(LinkedList *L)
{
    return (L->head == NULL);
}

void push(LinkedList *L, int item)
{
    Node *node = (Node *)malloc(sizeof(Node)); // 새 노드 메모리 할당

    // 기존 노드와 연결
    node->item = item;      // 노드 값 셋팅
    node->link = (L->head); // 기존 노드 -> 새 노드
    (L->head) = node;       // 현재 node가 추가됨
}

int pop(LinkedList *L)
{
    if (isEmpty(L))
    {
        printf("StackIsEmpty Error\n");
        exit(1);
    }
    else
    {
        }
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

char *cstrtok(char *postfix, char *delim)
{
    char *start = 0; // 문자열 시작 위치
    int i = 0;
    static char *tmp; // 문자열 주소 저장

    if (postfix != NULL) // 첫 토큰 받을 때
        start = postfix;
    else // 두 번째 이후 토큰 받을 때
        start = tmp;

    if (cstrlen(start) < 1) // 문자열 종료
        return NULL;

    for (i = 0; i < cstrlen(start); i++)
    {
        if (start[i] == (*delim) || start[i] == '\0')
        {
            start[i] = '\0';
            break;
        }
    }

    tmp = &start[i + 1]; // 다음 배열 시작 위치

    return start;
}

void toPostfix(char exp[], char postfix[])
{
    int k, p; // k : 임시 배열 인덱스, p : postfix 배열 인덱스
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
                postfix[p++] = ' ';
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
                postfix[p++] = ' ';
                postfix[p++] = pop(stack);
            }
            pop(stack); //'(' 제거
            break;
        default:                              // digit
            if (i > 0 && exp[i - 1] - 48 < 0) // 수식 숫자 앞 연산자 -> 공백
            {
                postfix[p++] = ' ';
            }
            postfix[p++] = ch;
            break;
        }
    }

    while (!isEmpty())
    {
        postfix[p++] = ' ';
        postfix[p++] = pop(stack);
    }
}

int calculate(char postfix[])
{
    int len = cstrlen(postfix);
    int op1, op2;
    int stack[MAX_SIZE];
    char *delim = " ";
    char *token;

    token = cstrtok(postfix, delim);
    while (token != NULL)
    {
        printf("token: %s\n", token);

        if (token[0] != '*' && token[0] != '/' && token[0] != '+' && token[0] != '-')
        {
            int i = 0;
            int val = 0; // 두자리 수 이상 결과

            // 각 토큰 -> 스택 저장
            while (token[i] != '\0')
            {
                val = val * 10 + (token[i] - 48);
                i++;
            }
            push_int(stack, val);
        }

        else
        {
            op2 = pop_int(stack);
            op1 = pop_int(stack);

            switch (token[0])
            {
            case '+':
                push_int(stack, op1 + op2);
                break;
            case '-':
                push_int(stack, op1 - op2);
                break;
            case '*':
                push_int(stack, op1 * op2);
                break;
            case '/':
                if (op2 == 0) // 나누기 0 예외 처리
                    printf("DivideByZeroException\n");
                push_int(stack, op1 / op2);
                break;
            }
        }

        token = cstrtok(NULL, delim);
    }

    return pop_int(stack);
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