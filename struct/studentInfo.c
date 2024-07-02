#include <stdio.h>

struct Student
{
    int id;     // 학번
    int grade;  // 학년
    int total;  // 총학점
    char *name; // 이름
};

int main(void)
{
    struct Student student[3] = {{1, 3, 88, "Mike"}, {2, 4, 116, "Tony"}, {3, 1, 38, "Justin"}};
    struct Student *ptr;
    ptr = &student;

    printf(" **** Student Info ****\n");
    for (int i = 0; i < 3; i++)
    {
        printf("\nID: %d\n", ptr->id);
        printf("Grade: %d\n", ptr->grade);
        printf("Total score: %d\n", ptr->total);
        printf("Name: %s\n", ptr->name);
        printf("------------------------------");
        ptr++; // 구조체 시작 부분을 가리킴
    }

    return 0;
}