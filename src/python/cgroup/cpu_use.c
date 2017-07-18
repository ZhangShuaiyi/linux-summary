#include <stdio.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <stdlib.h>

static void *pthread_func(void *args) {
    int *running = args;
    printf("pid:%d spid:%d thread id:%lu start!\n", getpid(), syscall(SYS_gettid), pthread_self());
    while (*running) {
        
    }
    printf("pid:%d spid:%d thread id:%lu start!\n", getpid(), syscall(SYS_gettid), pthread_self());
}

int main(int argc, char *argv[]) {
    int ret = 0;
    int running = 1;
    pthread_t thread_id0;
    pthread_t thread_id1;

    ret = pthread_create(&thread_id0, NULL, pthread_func, &running);
    ret = pthread_create(&thread_id1, NULL, pthread_func, &running);

    getchar();
    running = 0;

    pthread_join(thread_id0, NULL);
    pthread_join(thread_id1, NULL);
    exit(EXIT_SUCCESS);
}
