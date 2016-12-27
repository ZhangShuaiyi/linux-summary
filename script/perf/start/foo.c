#include <stdio.h>

static void longa() {
    int i, j;
    for(i = 0; i < 1000000; i++) {
        j = i;
    }
}

static void foo2() {
    int i = 0;
    for(i = 0; i < 10; i++) {
        longa();
    }
}

static void foo1() {
    int i = 0;
    for(i = 0; i < 100; i++) {
        longa();
    }
}

int main(int argc, char *argv[]) {
    foo1();
    foo2();
    return 0;
}
