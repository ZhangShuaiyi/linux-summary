#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include "ztest_ioctl.h"

#define FILE_NAME "/dev/ztest0"

int main(int argc, char *argv[]) {
    int fd = 0;

    fd = open(FILE_NAME, O_RDWR);
    if (fd == -1) {
        perror("open /dev/ztest0");
        exit(EXIT_FAILURE);
    }

    /* 内核中for_each_process可以遍历进程 */
    if (ioctl(fd, ZTEST_LIST_PROCESS) == -1) {
        perror("ztest ioctl list process");
    }

    if (ioctl(fd, ZTEST_LIST_FILES, (unsigned long)0) == -1) {
        perror("ztest ioctl list files");
    }

    close(fd);

    exit(EXIT_SUCCESS);
}