#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUF_LEN 16
#define PORT 7788

int main(int argc, char *argv[]) {
    int sock;
    ssize_t ret;
    struct sockaddr_in addr;
    char buf[BUF_LEN] = "Hello World";
    int addr_len = sizeof(struct sockaddr_in);

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, addr_len);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    if (argc < 2) {
        addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    } else {
        addr.sin_addr.s_addr = inet_addr(argv[1]);
    }

    ret = sendto(sock, buf, BUF_LEN, 0, (struct sockaddr *)&addr, addr_len);
    printf("sendto ret:%ld\n", ret);

    close(sock);
}
