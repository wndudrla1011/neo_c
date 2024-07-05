struct score
{
    double math;
    double english;
    double total;
    double average;
};

struct student
{
    int no;
    struct score s;
};

int main(void)
{
    struct student stu = {20170121, {90, 80, 0, 0}};

    stu.s.total = stu.s.math + stu.s.english;
    stu.s.average = stu.s.total / 2;

    printf("학번 : %d\n", stu.no);
    printf("총점 : %lf\n", stu.s.total);
    printf("평균 : %lf\n", stu.s.average);

    return 0;
}