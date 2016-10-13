#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/eventfd.h>
#include <sys/epoll.h>

#define MAX_EVENTS 2

int main(int argc, char *argv[]) {
    int efd = 0;
    int epollfd = 0;
    int nfds = 0;
    int n = 0;
    uint64_t count;
    int s = 0;
    struct epoll_event ev, events[MAX_EVENTS];

    efd = eventfd(0, 0);
    if (efd == -1) {
        perror("eventfd");
        exit(EXIT_FAILURE);
    }
    fprintf(stdout, "efd=%d pid=%d\n", efd, getpid());

    epollfd = epoll_create1(0);
    if (epollfd == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    ev.events = EPOLLIN;
    ev.data.fd = efd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, efd, &ev) == -1) {
        perror("epoll_ctl efd");
        exit(EXIT_FAILURE);
    }

    nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
    if (nfds == -1) {
        perror("epoll_wait");
        exit(EXIT_FAILURE);
    } else {
        for (n=0; n<nfds; n++) {
            s = read(events[n].data.fd, &count, sizeof(count));
            if (s != sizeof(count)) {
                fprintf(stderr, "eventfd read error!\n");
                exit(EXIT_FAILURE);
            } else {
                fprintf(stdout, "vaule read = %lu\n", count);
            }
        }
    }

    fprintf(stdout, "close eventfd.\n");
    close(efd);
    exit(EXIT_SUCCESS);
}
