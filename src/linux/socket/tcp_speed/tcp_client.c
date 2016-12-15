#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>

#define DATAS 1000000
#define MAXLEN 65536
#define PORT 6667

double
timeval_diff(struct timeval * tv0, struct timeval * tv1)
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
    int sockfd, n, ret;
    unsigned long long num = 0;
    char buf[MAXLEN] = {0};
    struct sockaddr_in serv_addr;
    struct timeval start_time, end_time;
    double spend_time, speed;

    /* net/socket.c SYSCALL_DEFINE3(socket, int, family, int, type, int, protocol) */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = PORT;
    if (argc < 2) {
        serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    } else {
        serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    }

    /* net/socket.c SYSCALL_DEFINE3(connect, int, fd, ...) */
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    num = 0;
    gettimeofday(&start_time, NULL);
    for (n = 0; n < DATAS; n++) {
        /*net/socket.c SYSCALL_DEFINE4(send, int, fd, ...) */
        ret = send(sockfd, buf, MAXLEN, 0);
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

    close(sockfd);
    exit(EXIT_SUCCESS);
}