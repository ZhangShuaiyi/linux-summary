#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include "zpage.h"

#define DEV_FILE "/dev/zpage"

int main(int argc, char *argv[]) {
    int fd = 0;
    int ret = 0;
    struct zpage_region zpage;

    if (argc < 3) {
        fprintf(stdout, "Usage %s <pid> <addr>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    zpage.pid = atoi(argv[1]);
    zpage.addr = strtol(argv[2], NULL, 16);

    fd = open(DEV_FILE, O_RDWR);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    ret = ioctl(fd, ZPAGE_GET_PID_ADDR, &zpage);
    if (ret == -1) {
        perror("ioctl ZPAGE_GET_PID_ADDR");
        exit(EXIT_FAILURE);
    }
    fprintf(stdout, "ioctl ZPAGE_GET_PID_ADDR ret:0x%x\n", ret);

    close(fd);
    exit(EXIT_SUCCESS);
}