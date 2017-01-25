#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    unsigned long tmp = 0x12345678;

    printf("Pid is:%d\n", getpid());
    printf("tmp address:%p\n", &tmp);

    getchar();
    return 0;
}