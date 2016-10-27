#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_THREAD 10
typedef struct {
    int running;
    pthread_t tid;
} thread_arg;

static void *thread_func(void *arg) {
    thread_arg *this_arg = (thread_arg *)arg;
    fprintf(stdout, "thread %lu enter pid:%u\n", pthread_self(), getpid());
    while (this_arg->running) {
        sleep(1);
    }
    fprintf(stdout, "thread %lu exit pidL%u\n", pthread_self(), getpid());
}

int main(int argc, char *argv[]) {
    int ch;
    int ret;
    int running = 1;
    int current_nums = 0;
    thread_arg args[MAX_THREAD] = {0};

    fprintf(stdout, "Enter:\n'c' create thread.\n'k' kill thread\n'q' quit\n\n");
    while(1) {
        ch = getchar();
        switch(ch) {
        case 'c':
            if (current_nums>=MAX_THREAD) {
                fprintf(stdout, "pthread nums is :%d, cannot kill\n", current_nums);
                continue;
            }
            args[current_nums].running = 1;
            ret = pthread_create(&args[current_nums].tid, NULL, thread_func, &args[current_nums]);
            if (ret!=0) {
                perror("pthread_create");
                continue;
            }
            current_nums++;
            break;
        case 'k':
            if (current_nums<=0) {
                fprintf(stdout, "pthread nums is %d, cannot kill\n", current_nums);
                continue;
            }
            args[current_nums-1].running = 0;
            ret = pthread_join(args[current_nums-1].tid, NULL);
            if (ret!=0) {
                fprintf(stderr, "pthread_join\n");
                continue;
            }
            current_nums--;
            break;
        case 'q':
            goto exit_loop;
        }
    }
exit_loop:
    fprintf(stdout, "main exit!\n");
    exit(EXIT_SUCCESS);
}