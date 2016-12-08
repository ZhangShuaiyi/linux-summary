#include <stdio.h>

static int testfun(int a, int b) {
    int c = 0;
    c = a + b;
    if (a > 10) {
        c -= 10;
    }
    return c;
}

int main(int argc, char *argv[]) {
    int a = 0;
    int b = 0;
    int c = 0;

    a = argc;
    b = a + 5;
    c = testfun(a, b);
    printf("testfun ret:%d\n", c);

    return 0;
}

