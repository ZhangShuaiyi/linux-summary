#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_LEN 16
#define PORT 7788

int main(int argc, char *argv[]) {
    int sock;
    char buf[BUF_LEN];
    struct sockaddr_in si_me, si_other;
    int slen = sizeof(si_other);

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&si_me, 0, sizeof(si_me));
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(PORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sock, (struct sockaddr *)&si_me, sizeof(si_me)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    while (1) {
        if (recvfrom(sock, buf, BUF_LEN, 0, (struct sockaddr *)&si_other, &slen) == -1) {
            perror("recvfrom");
            exit(EXIT_FAILURE);
        }
        printf("Received packet from %s:%d\nData:%s\n",
               inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port), buf);
    }

    exit(EXIT_SUCCESS);
}
