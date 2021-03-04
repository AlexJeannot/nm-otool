
#include "stdlib.h"
#include "stdio.h"

int global = 0;

int add_five(int nb)
{
    nb += 5;
    return (nb);
}

void add_five_addr(int *nb)
{
    static int lala;
    *nb += 5;
}

int main()
{
    int int_main;
    int *m;
    int *m2;
    char oui[5000];
    char *test = "test";
    const int c = 2;
    const char cchar[3] = "er";
    static int a = 1;
    static int b;

    int_main = 10;
    int_main = 22;
    int_main = add_five(int_main);
    add_five_addr(&int_main);

    // if (!(m = (int*)malloc(sizeof(int))))
    //     return (0);

    // if (!(m2 = (int*)malloc(sizeof(int) * 1500)))
    //     return (0);

    // free(m);
    // free(m2);
    printf("int_main = %d\n", int_main);

}