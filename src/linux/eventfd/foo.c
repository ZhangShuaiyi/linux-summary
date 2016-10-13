#include <stdio.h>
#include <sys/eventfd.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>

#define MAX_EVENTS 10

/* 读线程参数 */
typedef struct {
    /* 共使用的eventfd的数目 */
    int efd_num;
    /* 使用的eventfd */
    int *efd;
} READ_PARAM;

/* 写线程参数 */
typedef struct {
    /* eventfd值 */
    int efd;
    /* 写时间间隔 */
    int interval;
    /* 进行写操作次数 */
    int write_num;
} WRITE_PARAM;

static void *thread_read(void *arg) {
    int epollfd = 0;
    int nfds = 0;
    int n = 0;
    int ret = 0;
    int event_fd = 0;
    uint64_t count = 0;
    struct timeval tv;
    struct epoll_event event, events[MAX_EVENTS];
    READ_PARAM * param = (READ_PARAM*)arg;

    if (param->efd_num > MAX_EVENTS) {
        fprintf(stderr, "efd nums %d more than MAX_EVENTS %d\n", param->efd_num, MAX_EVENTS);
        return NULL;
    }
    epollfd = epoll_create1(0);
    if (epollfd == -1) {
        perror("epoll_create1");
        return NULL;
    }

    for(int i=0; i<param->efd_num; i++) {
        /* 将epoll实例注册文件描述符 */
        event.events = EPOLLIN;
        event.data.fd = param->efd[i];
        if (epoll_ctl(epollfd, EPOLL_CTL_ADD, param->efd[i], &event)==-1) {
            perror("epoll_ctl");
            return NULL;
        }
    }
    while (1) {
        nfds = epoll_wait(epollfd, events, MAX_EVENTS, 6000);
        if (nfds>0) {
            /* 有事件的文件描述符个数 */
            for (n=0; n<nfds; n++) {
                event_fd = events[n].data.fd;
                /* 对于eventfd读和写都必须为8个字节 */
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
            /* 超时 */
            fprintf(stdout, "epoll wait timeout\n");
            break;
        } else {
            /* 异常 */
            perror("epoll wait error");
            return NULL;
        }
    }
    close(epollfd);
    return NULL;
}

static void *thread_write(void *arg) {
    WRITE_PARAM *param = (WRITE_PARAM*)arg;
    int i = 0;
    int ret;
    struct timeval tv;
    uint64_t count = param->efd;
    int efd = param->efd;

    fprintf(stdout, "thread %lu enter efd:%d\n", pthread_self(), efd);
    for (i=0; i<param->write_num; i++) {
        ret = write(efd, &count, sizeof(count));
        if (ret<0) {
            perror("write eventfd");
            break;
        } else {
            gettimeofday(&tv, NULL);
            fprintf(stdout, "fd %d write %d bytes(%lu) at %lds %ldus i:%d\n",
                    efd, ret, count, tv.tv_sec, tv.tv_usec, i);
        }
        sleep(param->interval);
    }
    fprintf(stdout, "thread %lu end efd:%d\n", pthread_self(), efd);
    return NULL;
}

int main(int argc, char *argv[]) {
    int ret = 0;
    int i = 0;
    pthread_t pid_read = 0;
    int efd_num = 3;
    int write_num = 10;
    int *efds = NULL;
    pthread_t *pid_writes = NULL;
    READ_PARAM read_param;
    WRITE_PARAM *write_params;

    efds = (int *)malloc(sizeof(int)*efd_num);
    if (!efds) {
        perror("malloc efds");
        goto efds_err;
    }
    pid_writes = (pthread_t *)malloc(sizeof(pthread_t)*efd_num);
    if (!pid_writes) {
        perror("malloc pid_writes");
        goto pids_err;
    }
    write_params = (WRITE_PARAM *)malloc(sizeof(WRITE_PARAM)*efd_num);
    if (!write_params) {
        perror("malloc write_params");
        goto params_err;
    }

    for (i=0; i<efd_num; i++) {
        efds[i] = eventfd(0, 0);
        if (efds[i] == -1) {
            perror("eventfd");
            goto errs;
        }
    }

    read_param.efd_num = efd_num;
    read_param.efd = efds;

    ret = pthread_create(&pid_read, NULL, thread_read, (void *)&read_param);
    if (ret!=0) {
        perror("pthread_create");
        goto errs;
    }
    for (i=0; i<efd_num; i++) {
        write_params[i].efd = efds[i];
        write_params[i].write_num = write_num;
        write_params[i].interval = 2*i+1;
        ret = pthread_create(&pid_writes[i], NULL, thread_write, &write_params[i]);
    }
    for (i=0; i<efd_num; i++) {
        pthread_join(pid_writes[i], NULL);
    }
    pthread_join(pid_read, NULL);
    for (i=0; i<efd_num; i++) {
        close(efds[i]);
    }
    free(write_params);
    free(pid_writes);
    free(efds);
    return 0;

errs:
    free(write_params);
params_err:
    free(pid_writes);
pids_err:
    free(efds);
efds_err:
    return 1;
}

