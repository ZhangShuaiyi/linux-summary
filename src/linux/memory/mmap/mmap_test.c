#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>

#define LEN 4096
#define LARGELEN 20 * 1024 * 1024

int main(int argc, char *argv[]) {
    char ptr[LEN] = {0};
    char *lptr = NULL;
    int i = 0;

    // lptr = mmap(NULL, LARGELEN, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    lptr = mmap(NULL, LARGELEN, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (lptr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    printf("before use mmap pid:%d ptr:%p lptr:%p\n", getpid(), ptr, lptr);
    getchar();
    for (i = 0; i < LARGELEN / 2; i++) {
        lptr[i] = i%256;
    }
    printf("after use 0x%x mmap pid:%d ptr:%p lptr:%p\n", LARGELEN / 2, getpid(), ptr, lptr);
    getchar();
    for (i = 0; i < LARGELEN; i++) {
        lptr[i] = i%256;
    }
    printf("after use 0x%x mmap pid:%d ptr:%p lptr:%p\n", LARGELEN, getpid(), ptr, lptr);
    getchar();

    return 0;
}
