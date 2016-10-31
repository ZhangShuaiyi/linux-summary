#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>

#if 1
#define MMAP_LEN 256
#else
#define MMAP_LEN 10240
#endif
#define device_name "/dev/ztest"

int main(int argc, char *argv[]) {
    int fd;
    void *mmap_addr;
    char buf[MMAP_LEN];

    fd = open(device_name, O_RDWR);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    mmap_addr = mmap(NULL, MMAP_LEN, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if (mmap_addr == (void *)-1) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    fprintf(stdout, "mmap addr:%p\n", mmap_addr);
    fprintf(stdout, "%s\n", mmap_addr);

    strncpy(buf, mmap_addr, MMAP_LEN);
    fprintf(stdout, "buf:%s\n", buf);
#if 0
    fprintf(stdout, "enter char to continue:\n");
    getchar();
#endif
    munmap(mmap_addr, MMAP_LEN);
    close(fd);
    exit(EXIT_SUCCESS);
}