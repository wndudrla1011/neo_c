#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define MAX_SIZE 100

typedef struct
{
    double item;
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

void push(LinkedList *L, double item)
{
    Node *node = (Node *)malloc(sizeof(Node)); // 새 노드 메모리 할당

    // 기존 노드와 연결
    node->item = item;      // 노드 값 셋팅
    node->link = (L->head); // 새 노드 -> 기존 노드
    (L->head) = node;       // 맨 앞 노드이므로 head로 변경
}

double pop(LinkedList *L)
{
    if (isEmpty(L))
    {
        printf("StackIsEmpty Error\n");
        exit(1);
    }
    else
    {
        Node *node = L->head;    // node: 가장 최근에 연결된 노드
        int item = node->item;   // 반환할 값
        L->head = L->head->link; // 바로 앞에 위치한 노드가 head가 됨
        free(node);
        return item;
    }
}

double peek(LinkedList *L)
{
    if (isEmpty(L))
    {
        printf("StackIsEmpty Error\n");
        exit(1);
    }
    else
    {
        return L->head->item; // 가장 뒷 노드의 값 리턴
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
    int p = 0; // p : postfix 배열 인덱스
    int len = cstrlen(exp);
    char ch;
    Node *node = NULL;

    init(&node);

    for (int i = 0; i < len; i++)
    {
        ch = exp[i];

        switch (ch)
        {
        case '*':
        case '/':
        case '+':
        case '-':
            while (!isEmpty(&node) && priority(ch) <= priority(peek(&node)))
            {
                postfix[p++] = ' ';
                postfix[p++] = pop(&node);
            }
            push(&node, ch);
            break;
        case '(':
            push(&node, ch);
            break;
        case ')':
            while (!isEmpty(&node) && peek(&node) != '(') // find '('
            {
                postfix[p++] = ' ';
                postfix[p++] = pop(&node);
            }
            pop(&node); //'(' 제거
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

    while (!isEmpty(&node))
    {
        postfix[p++] = ' ';
        postfix[p++] = pop(&node);
    }
}

double calculate(char postfix[])
{
    int len = cstrlen(postfix);
    double op1 = 0, op2 = 0;
    Node *node = NULL;
    char *delim = " ";
    char *token;
    char ch = '0';
    int val = 0; // 임시 저장: char -> int

    init(&node);

    token = cstrtok(postfix, delim);

    while (token != NULL)
    {
        printf("token: %s\n", token);
        token = cstrtok(NULL, delim);
    }

    for (int i = 0; postfix[i] != '\0'; i++)
    {
        ch = postfix[i];

        if (ch != '*' && ch != '/' && ch != '+' && ch != '-')
        {
            val = ch - '0';
            push(&node, val);
        }

        else
        {
            op2 = pop(&node);
            op1 = pop(&node);

            switch (ch)
            {
            case '+':
                push(&node, op1 + op2);
                break;
            case '-':
                push(&node, op1 - op2);
                break;
            case '*':
                push(&node, op1 * op2);
                break;
            case '/':
                if (op2 == 0) // 나누기 0 예외 처리
                    printf("DivideByZeroException\n");
                push(&node, op1 / op2);
                break;
            }
        }
    }

    return pop(&node);
}

int main(void)
{
    int n = 3;
    double result = 0.0;
    char *exp; // 입력
    char postfix[MAX_SIZE] = {'\0'};
    exp = (char *)malloc(sizeof(char) * 100);

    printf("계산식을 입력한 후 Enter를 눌러주세요!\n");
    scanf("%s", exp);

    toPostfix(exp, postfix);
    printf("\nPostfix: %s\n", postfix);

    result = calculate(postfix);
    // result = result * pow(10, n + 1) + 5; // 소수점 3자리 반올림

    printf("계산한 결과: \t%f\n", result);

    return 0;
}