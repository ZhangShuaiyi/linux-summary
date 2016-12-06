#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXLEN 8
#define PORT 6667

int main(int argc, char *argv[]) {
    int sockfd, n;
    char buf[MAXLEN] = {0};
    struct sockaddr_in serv_addr;

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

    fprintf(stdout, "buf addr:%p\n", buf);
    for (n = 0; n < MAXLEN; n++) {
        buf[n] = 0x55;
    }
    /*net/socket.c SYSCALL_DEFINE4(send, int, fd, ...) */
    send(sockfd, buf, MAXLEN, 0);

    close(sockfd);
    exit(EXIT_SUCCESS);
}