#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int *arr;
    arr = (int *)malloc(sizeof(int) * 4);

    arr[0] = 100;
    arr[1] = 200;
    arr[2] = 300;
    arr[3] = 400;

    printf("%d\n", sizeof(arr));

    for (int i = 0; i < 4; i++)
    {
        printf("arr[%d] : %d\n", i, arr[i]);
    }

    free(arr); // 동적할당 해제

    return 0;
}