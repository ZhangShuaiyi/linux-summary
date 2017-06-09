/**
 * call get_mempolicy
 * build: gcc numaget.c -lnuma
 * test: numactl --membind=0 ./a.out
 */
#include <stdio.h>
#include <stdlib.h>
#include <numaif.h>

int main(int argc, char *argv[]) {
    /*
      #define MPOL_DEFAULT     0
      #define MPOL_PREFERRED    1
      #define MPOL_BIND        2
      #define MPOL_INTERLEAVE  3
     */
    int mode = -1;
    unsigned long *mask = NULL;
    unsigned long maxnode = 1025;
    int ret = 0;
    int perlength = 0;

    if (argc > 1) {
        /* maxnode: read from /proc/self/status, Line 'Mems_allowed:' bit length + 1 */
        maxnode = atoi(argv[1]);
        printf("maxnode:%d\n", maxnode);
    }
    ret = get_mempolicy(&mode, NULL, 0, 0, 0);
    if (ret == -1) {
        perror("get_mempolicy");
        return 1;
    }
    printf("mode:%d\n", mode);
    perlength = sizeof(long);
    mask = malloc(((maxnode + perlength - 1) / perlength) * perlength);
    ret = get_mempolicy(&mode, mask, maxnode, 0, 0);
    if (ret == -1) {
        perror("get_mempolicy");
        return 1;
    }
    printf("mode:%d mask:0x%x\n", mode, *mask);
    free(mask);
    return 0;
}
