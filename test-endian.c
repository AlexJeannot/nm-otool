#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"

int main()
{
    unsigned int test = 44444444;
    int bit;

    for (int offset = 31; offset >= 0; offset--)
    {
        bit = (test >> offset) & 1;
        printf("%d", bit);
        if (offset == 7 || offset == 16 || offset == 24)
            printf(" ");
    }
    printf("\n");
    unsigned short *ptr = (unsigned short *)&test;
    printf("ptr = %d\n", *ptr);
    ptr++;
    printf("ptr = %d\n", *ptr);
    printf("\n");
}

/*

00000010 10100110 00101011 00011100

*/