#include <stdio.h>

int main(void)
{
    int val1 = 10;
    int val2 = 20;
    int val3 = 30;

    printf("메모리상 val1 위치 : %d\n", &val1);
    printf("메모리상 val2 위치 : %d\n", &val2);
    printf("메모리상 val3 위치 : %d\n\n", &val3);

    int *cursor = &val1;

    //포인터 변수 cursor를 통해 val1 접근
    printf("val1의 주소: %d, 값: %d\n", &val1, val1);
    printf("cursor의 주소: %d, 값: %d\n\n", cursor, *cursor);

    *cursor = *cursor + 5;

    //포인터 변수는 가리키는 값을 공유한다.
    printf("val1의 주소: %d, 값: %d\n", &val1, val1);
    printf("cursor의 주소: %d, 값: %d\n", cursor, *cursor);

    return 0;
}