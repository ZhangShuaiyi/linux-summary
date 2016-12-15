#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/vm_sockets.h>

#define MAXLEN 65536

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

static int vsock_listen(const char *port_str)
{
    int listen_fd;
    int client_fd;
    struct sockaddr_vm sa_listen = {
        .svm_family = AF_VSOCK,
        .svm_cid = VMADDR_CID_ANY,
    };
    struct sockaddr_vm sa_client;
    socklen_t socklen_client = sizeof(sa_client);
    int port = parse_port(port_str);
    if (port < 0) {
        return -1;
    }

    sa_listen.svm_port = port;

    listen_fd = socket(AF_VSOCK, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        perror("socket");
        return -1;
    }

    if (bind(listen_fd, (struct sockaddr*)&sa_listen, sizeof(sa_listen)) != 0) {
        perror("bind");
        close(listen_fd);
        return -1;
    }

    if (listen(listen_fd, 1) != 0) {
        perror("listen");
        close(listen_fd);
        return -1;
    }

    client_fd = accept(listen_fd, (struct sockaddr*)&sa_client, &socklen_client);
    if (client_fd < 0) {
        perror("accept");
        close(listen_fd);
        return -1;
    }

    fprintf(stderr, "Connection from cid %u port %u...\n", sa_client.svm_cid, sa_client.svm_port);

    close(listen_fd);
    return client_fd;
}


int main(int argc, char *argv[]) {
    int fd, ret;
    unsigned long long num;
    char buf[MAXLEN] = {0};

    if (argc < 2) {
        fprintf(stdout, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    fd = vsock_listen(argv[1]);
    if (fd < 0) {
        fprintf(stderr, "vsock_listen %s error\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    num = 0;
    while ((ret = recv(fd, buf, MAXLEN, 0)) > 0) {
        num += ret;
    }
    fprintf(stdout, "recv %lld bytes\n", num);

    close(fd);
    exit(EXIT_SUCCESS);
}
