#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include "zpage.h"

#define DEV_FILE "/dev/zpage"

static void usage() {
    fprintf(stdout, "\tt: test\n"
            "\ti: get mm_struct info\n"
            "\th: this info\n"
            "\tq: quit\n");
}

static void enter_pid(int *pid) {
    fprintf(stdout, ">Enter pid:");
    scanf("%d", pid);
}

static void enter_addr(__u64 *addr) {
    fprintf(stdout, ">Enter addr:");
    scanf("%lx", addr);
}

int main(int argc, char *argv[]) {
    int fd = 0;
    int ret = 0;
    char c;
    void *mmap_addr;

    fd = open(DEV_FILE, O_RDWR);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    mmap_addr = mmap(NULL, MMAP_LEN, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if (mmap_addr == (void *)-1) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    usage();
    fprintf(stdout, "=>cmd:");
    while ((c = getchar()) != 'q') {
        if (isspace(c)) {
            fprintf(stdout, "=>cmd:");
            continue;
        }
        switch (c) {
        case 't': {
            struct zpage_region zpage;
            enter_pid(&zpage.pid);
            enter_addr(&zpage.addr);
            ret = ioctl(fd, ZPAGE_GET_PID_ADDR, &zpage);
            if (ret == -1) {
                perror("ioctl ZPAGE_GET_PID_ADDR");
                exit(EXIT_FAILURE);
            }
            fprintf(stdout, "ioctl ZPAGE_GET_PID_ADDR ret:0x%x\n", ret);
            break;
        }
        case 'i': {
            int pid = 0;
            enter_pid(&pid);
            ret = ioctl(fd, ZPAGE_GET_MM_INFO, (long)pid);
            if (ret == -1) {
                perror("ioctl ZPAGE_GET_MM_INFO");
                exit(EXIT_FAILURE);
            }
            fprintf(stdout, "ioctl ZPAGE_GET_MM_INFO ret:0x%x\n", ret);
            fprintf(stdout, "%s\n", mmap_addr);
            break;
        }
        case 'h':
            usage();
            break;
        default:
            usage();
        }
    }

    munmap(mmap_addr, MMAP_LEN);
    close(fd);
    exit(EXIT_SUCCESS);
}