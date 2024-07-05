#include <stdio.h>

int main(void)
{
    char c1 = 'A';
    char *cp = NULL;
    char **cpp = NULL;

    cp = &c1;
    cpp = &cp;

    printf("%c %p %p\n", c1, cp, cpp);
    printf("%p %p %p\n", &c1, &cp, &cpp);
    printf("%c %c %c\n", c1, *cp, **cpp);

    return 0;
}