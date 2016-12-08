#include <stdio.h>

int main(int argc, char *argv[]) {
    int a = 0;
    int b = 0;

    a = argc;
    b = a + 0x5;
    fprintf(stdout, "a:%d b:%d\n", a, b);
    return 0;
}