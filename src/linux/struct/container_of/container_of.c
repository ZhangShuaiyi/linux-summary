#include <stdio.h>
#include <stdlib.h>

#define offsetof(type, member) (size_t)&(((type *)0)->member)

#define container_of(ptr, type, member) ({ \
            const typeof(((type *)0)->member) *__mptr = (ptr); \
            (type *)((char *)__mptr - offsetof(type, member));})

typedef struct foo_info {
    int i;
    char c;
    int t;
} FOO_INFO, pFOO_INFO;

int main(int argc, char *argv[]) {
    FOO_INFO info = {0x55, 'c', 0x5555};
    pFOO_INFO *tmp = NULL;

    fprintf(stdout, "info address:%p\n", &info);
    tmp = container_of(&info.t, FOO_INFO, t);
    fprintf(stdout, "tmp address:%p\n", tmp);
    exit(EXIT_SUCCESS);
}