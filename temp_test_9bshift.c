#include <stdio.h>

struct test {
    int x:9; // define x as a 9 bit number
    int y:7; // top 7 bits for attrs
};


static inline signed short sext9(unsigned short num) {
    signed short ret = num << 7;
    return ret >> 7;
}

int main(int argc, char *argv[])
{

    struct test t = {.x=-1, .y = -2};
    printf("struct test: %x, %d\n", t.x, t.x);

    short x2 = sext9(0x1FE);
    printf("sext test: %x, %d\n", x2, x2);
    return 0;
}
