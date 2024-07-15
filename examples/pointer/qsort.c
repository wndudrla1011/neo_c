#include <stdio.h>
#define SIZE 1000

void swap(int *a, int *b);
void quickSort(int start, int end);

int a[SIZE];

int main(void)
{
    int size;
    printf("Array size : ");
    scanf("%d", &size);

    // 배열 값 입력
    printf("Input array values >> ");
    for (int i = 0; i < size; i++)
    {
        scanf("%d", &a[i]);
    }

    quickSort(0, size - 1);

    printf("Result: ");
    for (int i = 0; i < size; i++)
    {
        printf("%d ", a[i]);
    }

    return 0;
}

void quickSort(int start, int end)
{
    if (start >= end)
        return;

    int key = start, i = start + 1, j = end; // 가장 좌측 값을 키로

    while (i <= j)
    {
        while (i <= end && a[i] <= a[key])
            i++;
        while (j > start && a[j] >= a[key])
            j--;

        if (i > j)
            swap(&a[key], &a[j]);
        else
            swap(&a[i], &a[j]);
    }

    quickSort(start, j - 1);
    quickSort(j + 1, end);
}

void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}
