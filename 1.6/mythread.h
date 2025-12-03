#ifndef MY_THREAD
#define MY_THREAD

#define _GNU_SOURCE

#include <sys/types.h>
#include <stddef.h>

typedef struct mythread
{
    pid_t tid;
    void *stack;
    size_t stack_size;

    void *retval;

    void *(*start_routine)(void *);

    void *arg;
} mythread_t;

int mythread_create(mythread_t *thread, void *(*start_routine)(void *), void *arg);

int mythread_join(mythread_t *thread, void **retval);

int mythread_detach(mythread_t *thread);

#endif //MY_THREAD