#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXLEN 10240
#define MAX_EVENTS 10
#define PORT 6667

static int do_use_fd(int fd) {
    int n ;
    char buf[MAXLEN];

    /* net/socket.c SYSCALL_DEFINE4(recv, int, fd, ...) */
    n = recv(fd, buf, MAXLEN, 0);
    if (n) {
        fprintf(stdout, "recv from %d len:%d\n", fd, n);
    }
    return n;
}

int main(int argc, char *argv[]) {
    int listen_sock = 0;
    int conn_sock = 0;
    struct sockaddr_in serv_addr, client_addr;
    struct epoll_event ev, events[MAX_EVENTS];
    int n, nfds, epollfd;
    socklen_t addr_len;
    int ret;

    epollfd = epoll_create1(0);
    if (epollfd == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }
    /* net/socket.c SYSCALL_DEFINE3(socket, int, family, int, type, int protocol) */
    listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == -1) {
        perror("socket SOCK_STREAM");
        exit(EXIT_FAILURE);
    }

    memset(&serv_addr, 0, sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    if (argc > 1) {
        serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    } else {
        serv_addr.sin_addr.s_addr = htons(INADDR_ANY);
    }
    serv_addr.sin_port = PORT;

    /* net/socket.c SYSCALL_DEFINE3(bind, int, fd, struct sockaddr __user *, umyaddr, int, addrlen) */
    if (bind(listen_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    /* net/socket.c SYSCALL_DEFINE2(listen, int, fd, int, backlog) */
    if (listen(listen_sock, 10) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    ev.events = EPOLLIN;
    ev.data.fd = listen_sock;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev) == -1) {
        perror("epoll_ctl");
        exit(EXIT_FAILURE);
    }

    addr_len = sizeof(client_addr);
    for (;;) {
        nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }

        for (n = 0; n < nfds; n++) {
            if (events[n].data.fd == listen_sock) {
                /* net/socket.c SYSCALL_DEFINE3(accept, int, fd, ...) */
                conn_sock = accept(listen_sock, (struct sockaddr *)&client_addr, &addr_len);
                if (conn_sock == -1) {
                    perror("accept");
                    exit(EXIT_FAILURE);
                }
                fprintf(stdout, "accept from %s fd:%d\n", inet_ntoa(client_addr.sin_addr), conn_sock);
                ev.events = EPOLLIN;
                ev.data.fd = conn_sock;
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &ev) == -1) {
                    perror("epoll_ctl: conn_sock");
                    exit(EXIT_FAILURE);
                }
            } else {
                ret = do_use_fd(events[n].data.fd);
                if (!ret) {
                    fprintf(stdout, "close fd %d\n", events[n].data.fd);
                    ev.data.fd = events[n].data.fd;
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, events[n].data.fd, &ev);
                    close(events[n].data.fd);
                }
            }
        }
    }

    exit(EXIT_SUCCESS);
}