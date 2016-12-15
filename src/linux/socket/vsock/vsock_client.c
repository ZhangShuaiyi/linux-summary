#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/vm_sockets.h>
#include <sys/time.h>

#define DATAS 1000000
#define MAXLEN 65536

static int parse_cid(const char *cid_str)
{
    char *end = NULL;
    long cid = strtol(cid_str, &end, 10);
    if (cid_str != end && *end == '\0') {
        return cid;
    } else {
        fprintf(stderr, "invalid cid: %s\n", cid_str);
        return -1;
    }
}

static int parse_port(const char *port_str)
{
    char *end = NULL;
    long port = strtol(port_str, &end, 10);
    if (port_str != end && *end == '\0') {
        return port;
    } else {
        fprintf(stderr, "invalid port number: %s\n", port_str);
        return -1;
    }
}

static int vsock_connect(const char *cid_str, const char *port_str)
{
    int fd;
    int cid;
    int port;
    struct sockaddr_vm sa = {
        .svm_family = AF_VSOCK,
    };

    cid = parse_cid(cid_str);
    if (cid < 0) {
        return -1;
    }
    sa.svm_cid = cid;

    port = parse_port(port_str);
    if (port < 0) {
        return -1;
    }
    sa.svm_port = port;

    fd = socket(AF_VSOCK, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        return -1;
    }

    if (connect(fd, (struct sockaddr*)&sa, sizeof(sa)) != 0) {
        perror("connect");
        close(fd);
        return -1;
    }

    return fd;
}

static double timeval_diff(struct timeval * tv0, struct timeval * tv1)
{
    double time1, time2;

    time1 = tv0->tv_sec + (tv0->tv_usec / 1000000.0);
    time2 = tv1->tv_sec + (tv1->tv_usec / 1000000.0);

    time1 = time1 - time2;
    if (time1 < 0) {
        time1 = -time1;
    }
    return time1;
}

int main(int argc, char *argv[]) {
    int fd, ret, n;
    unsigned long long num = 0;
    struct timeval start_time, end_time;
    double spend_time, speed;
    char buf[MAXLEN] = {0};

    if (argc < 3) {
        fprintf(stderr, "Usage: %s <cid> <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    fd = vsock_connect(argv[1], argv[2]);
    if (fd < 0) {
        fprintf(stderr, "vsock_connect %s %s\n", argv[1], argv[2]);
        exit(EXIT_FAILURE);
    }

    num = 0;
    gettimeofday(&start_time, NULL);
    for (n = 0; n < DATAS; n++) {
        /*net/socket.c SYSCALL_DEFINE4(send, int, fd, ...) */
        ret = send(fd, buf, MAXLEN, 0);
        if (ret <= 0) {
            fprintf(stderr, "send error ret:%d\n", ret);
            break;
        }
        num += ret;
    }
    gettimeofday(&end_time, NULL);
    spend_time = timeval_diff(&end_time, &start_time);
    speed = num/spend_time;
    fprintf(stdout, "total send %lld bytes use %fs speed:%fMB/s\n",
            num, spend_time, speed/1000/1000);

    close(fd);
    exit(EXIT_SUCCESS);
}