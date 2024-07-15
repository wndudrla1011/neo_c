#include <stdio.h>

typedef struct student
{
    char name[20];
    int money;
    struct student *link;
} Student_List;

int main(void)
{
    Student_List stu1 = {"박효원", 5000, NULL};
    Student_List stu2 = {"라태인", 6000, NULL};
    Student_List stu3 = {"강건욱", 4000, NULL};

    stu1.link = &stu2;
    stu2.link = &stu3;

    printf("%s %d\n", stu1.name, stu1.money);
    printf("%s %d\n", stu1.link->name, stu1.link->money);
    printf("%s %d\n", stu1.link->link->name, stu1.link->link->money);

    return 0;
}
