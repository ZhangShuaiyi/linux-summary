/**
 * Use ptrace and /proc/{pid}/mem read memory data
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ptrace.h>
#include <sys/wait.h>

#define BUFLEN 32

int main(int argc, char *argv[]) {
    int mem_fd = 0;
    int pid, len;
    off_t addr;
    int ret, i;
    unsigned char *buf = NULL;
    char fname[BUFLEN] = {0};

    if (argc < 4) {
        fprintf(stderr, "Usage: %s <pid> <addr> <len>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    pid = atoi(argv[1]);
    sscanf(argv[2], "%lx", &addr);
    sscanf(argv[3], "%x", &len);
    fprintf(stdout, "Try to get [%d] 0x%lx (0x%x) bytes\n", pid, addr, len);
    snprintf(fname, BUFLEN, "/proc/%u/mem", pid);

    mem_fd = open(fname, O_RDWR);
    if (mem_fd < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    fprintf(stdout, "open %s success\n", fname);

    /* Attach to pid */
    ret = ptrace(PTRACE_ATTACH, pid, NULL, NULL);
    if (ret == -1) {
        perror("ptrace");
        goto out;
    }
    ret = waitpid(pid, NULL, 0);
    if (ret == -1) {
        perror("waitpid");
        goto out2;
    }

    if (lseek(mem_fd, addr, SEEK_SET) == -1) {
        perror("lseek");
        goto out2;
    }
    buf = (unsigned char *)malloc(len);
    if (!buf) {
        perror("malloc");
        goto out2;
    }

    read(mem_fd, buf, len);
    fprintf(stdout, "[%d] 0x%lx data:\n", pid, addr);
    for (i = 0; i < len; i++) {
        fprintf(stdout, "0x%x ", buf[i]);
    }
    fprintf(stdout, "\n");

    free(buf);
out2:
    ptrace(PTRACE_DETACH, pid, NULL, NULL);
out:
    close(mem_fd);
    exit(EXIT_SUCCESS);
}