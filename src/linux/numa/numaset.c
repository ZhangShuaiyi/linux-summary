/**
 * build: gcc numaset.c -lnuma
 * test: cat /proc/<PID>/numa_maps
 */
#include <stdio.h>
#include <numaif.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int mode = 2;
    unsigned long mask0 = 0x1;
    unsigned long *mask = NULL;
    unsigned long maxnode = 1025;
    int ret = 0;
    int perlength = 0;

    if (argc > 1) {
        mode = atoi(argv[1]);
    }
    if (argc > 2) {
        mask0 = atoi(argv[2]);
    }
    if (argc > 3) {
        maxnode = atoi(argv[3]);
    }
    printf("mode:%d mask:0x%x maxnode:%d\n", mode, mask0, maxnode);

    perlength = sizeof(unsigned long);
    mask = malloc(((maxnode + perlength - 1) / perlength) * perlength);
    mask[0] = mask0;
    ret = set_mempolicy(mode, mask, maxnode);
    if (ret == -1) {
        perror("set_mempolicy\n");
    }
    printf("pid:%d\n", getpid());
    getchar();
    free(mask);
    return 0;
}
