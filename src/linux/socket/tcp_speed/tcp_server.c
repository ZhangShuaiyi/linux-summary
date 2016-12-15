#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXLEN 65536
#define PORT 6667

int main(int argc, char *argv[]) {
    int listen_sock = 0;
    int conn_sock = 0;
    struct sockaddr_in serv_addr, client_addr;
    socklen_t addr_len;
    char buf[MAXLEN];
    int ret;
    unsigned long long num;

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

    addr_len = sizeof(client_addr);

    /* net/socket.c SYSCALL_DEFINE3(accept, int, fd, ...) */
    conn_sock = accept(listen_sock, (struct sockaddr *)&client_addr, &addr_len);
    if (conn_sock == -1) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    fprintf(stdout, "accept from %s fd:%d\n", inet_ntoa(client_addr.sin_addr), conn_sock);

    num = 0;
    /* net/socket.c SYSCALL_DEFINE4(recv, int, fd, ...) */
    while ((ret = recv(conn_sock, buf, MAXLEN, 0)) > 0) {
        num += ret;
    }
    fprintf(stdout, "recv datas:%lld\n", num);
    exit(EXIT_SUCCESS);
}