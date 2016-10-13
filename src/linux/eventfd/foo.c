#include <stdio.h>
#include <sys/eventfd.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>

#define MAX_EVENTS 10

static void *thread_read(void *arg) {
    int epollfd = 0;
    int nfds = 0;
    int n = 0;
    int ret = 0;
    int event_fd = 0;
    uint64_t count = 0;
    struct timeval tv;
    struct epoll_event event, events[MAX_EVENTS];
    int efd = *(int*)arg;

    epollfd = epoll_create1(0);
    if (epollfd == -1) {
        perror("epoll_create1");
        return NULL;
    }

    event.events = EPOLLIN;
    event.data.fd = efd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, efd, &event)==-1) {
        perror("epoll_ctl");
        return NULL;
    }
    while (1) {
        nfds = epoll_wait(epollfd, events, MAX_EVENTS, 5000);
        if (nfds>0) {
            for (n=0; n<nfds; n++) {
                event_fd = events[n].data.fd;
                ret = read(event_fd, &count, sizeof(count));
                if (ret==-1) {
                    perror("read event_fd");
                    return NULL;
                } else {
                    gettimeofday(&tv, NULL);
                    fprintf(stdout, "fd %d read %d bytes(%lu) at %lds %ldus\n",
                            event_fd, ret, count, tv.tv_sec, tv.tv_usec);
                }
            }
        } else if (nfds==0) {
            fprintf(stdout, "epoll wait timeout\n");
            break;
        } else {
            perror("epoll wait error");
            return NULL;
        }
    }
    close(epollfd);
    return NULL;
}

int main(int argc, char *argv[]) {
    int efd;
    int ret = 0;
    int i = 0;
    struct timeval tv;
    uint64_t count = 1;
    pthread_t pid = 0;

    efd = eventfd(0, 0);
    if (efd == -1) {
        perror("eventfd");
        return 1;
    }

    ret = pthread_create(&pid, NULL, thread_read, (void *)&efd);
    if (ret!=0) {
        perror("pthread_create");
        return 0;
    }
    for (i=0; i<5; i++) {
        ret = write(efd, &count, sizeof(count));
        if (ret<0) {
            perror("write eventfd");
            return 0;
        } else {
            gettimeofday(&tv, NULL);
            fprintf(stdout, "fd %d write %d bytes(%lu) at %lds %ldus\n",
                    efd, ret, count, tv.tv_sec, tv.tv_usec);
        }
        sleep(1);
    }
    pthread_join(pid, NULL);
    close(efd);
    return 0;
}

