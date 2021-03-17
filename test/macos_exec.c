
int global_classic_int_init = 1;
int global_classic_int_ninit;
// int8_t global_small_int;
const char global_const_char_array[4] = "oui";
char global_char_array[5000];

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
    // int8_t small_int;
    int classic_int;
    int *ptr_int;
    int *m2;
    char char_array[5000];
    char *test = "test";
    const int const_int = 2;
    const char const_char[3] = "AA";
    static int static_int_init = 1;
    static int static_int_ninit;
    char simple_char;

    classic_int = 10;
    classic_int = 22;
    classic_int = add_five(classic_int);
    add_five_addr(&classic_int);

    // if (!(ptr_int = (int*)malloc(sizeof(int))))
    //     return (0);

    // free(ptr_int);
    // printf("classic_int = %d\n", classic_int);

}