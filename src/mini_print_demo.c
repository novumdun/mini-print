#include <stdio.h>
#include "mini_print.h"

static void mini_print(void)
{
    char t0 = 01;
    unsigned char t1 = 2;
    short t2 = 3;
    unsigned short t3 = 4;
    int t4 = 5;
    unsigned int t5 = 6;

    MINI_STR_BUF_WRITE_STRU("hello");
    MINI_STR_BUF_WRITE_STRU("hello %d %d %d %d %d", 0, 1, 2, 3, 4);
    MINI_STR_BUF_WRITE_STRU("hello %d %d %d %d %d %d", t0, t1, t2, t3, t4, t5);
    mini_print_str_stru_buf();
    return;
}

int main(int argc, char **argv)
{
    mini_print();
    return 0;
}
