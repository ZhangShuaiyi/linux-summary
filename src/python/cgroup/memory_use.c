#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MB 10*1024*1024

int main(int argc, char *argv[]) {
    char *p = NULL;
    int i = 0;
    char c;
    printf("Pid:%d\n", getpid());
    while ((c = getchar()) != 'q') {
        p = (char *)malloc(MB);
        memset(p, 0xff, MB);
        printf("%d memory malloced\n", i++);
    }
    return 0;
}
