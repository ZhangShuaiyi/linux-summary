#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <sys/syscall.h>
#include "ztest_ioctl.h"

#define FILE_NAME "/dev/ztest0"

static int fd = 0;

static void print_info() {
    fprintf(stdout, "pid:%u tid:%ld\n", getpid(), syscall(SYS_gettid));
    if (ioctl(fd, ZTEST_GET_THREAD, (unsigned long)0) == -1) {
        perror("ztest get thread");
    }
}

static void *thread_func(void *arg) {
    print_info();
}

int main(int argc, char *argv[]) {
    pthread_t tid;
    fd = open(FILE_NAME, O_RDWR);
    if (fd == -1) {
        perror("open /dev/ztest0");
        exit(EXIT_FAILURE);
    }

#if 0
    /* 内核中for_each_process可以遍历进程 */
    if (ioctl(fd, ZTEST_LIST_PROCESS) == -1) {
        perror("ztest ioctl list process");
    }

    if (ioctl(fd, ZTEST_LIST_FILES, (unsigned long)0) == -1) {
        perror("ztest ioctl list files");
    }
#endif

    pthread_create(&tid, NULL, thread_func, NULL);
    pthread_create(&tid, NULL, thread_func, NULL);
    print_info();
    sleep(1);
    close(fd);

    exit(EXIT_SUCCESS);
}