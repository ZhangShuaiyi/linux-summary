#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int a = 0x55555555;
    int b = 0xaaaaaaaa;

    fprintf(stdout, "getpid:%d argc:%p argv:%p\n", getpid(), &argc, argv);
    fprintf(stdout, "a addr:%p val:0x%x\n", &a, a);
    fprintf(stdout, "b addr:%p val:0x%x\n", &b, b);
    fprintf(stdout, "Wait input to continue\n");
    getchar();
    fprintf(stdout, "a addr:%p val:0x%x\n", &a, a);
    fprintf(stdout, "b addr:%p val:0x%x\n", &b, b);
    fprintf(stdout, "Wait input to quit\n");
    getchar();
    exit(EXIT_SUCCESS);
}