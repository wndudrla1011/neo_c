#include <stdio.h>

typedef struct
{
    int id;
    char *name;
    int price;
} Product;

int main(void)
{
    Product product1;

    // 초기화 - 방법 1
    product1.id = 1;
    product1.name = "Server";
    product1.price = 50000;

    // 초기화 - 방법 2
    Product product2 = {.id = 1, .name = "Server", .price = 50000};

    // 초기화 - 방법 3
    Product product3 = {1, "Server", 50000};

    printf("id: %d\n", product2.id);
    printf("name: %s\n", product2.name);
    printf("price: %d\n", product2.price);

    return 0;
}