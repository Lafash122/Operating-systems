#define _GNU_SOURCE

#include "mythread.h"

#include <stdatomic.h>
#include <sys/syscall.h>
#include <linux/futex.h>

static inline int futex_wait(_Atomic int *uaddr, int value) {
    return syscall(SYS_futex, (int *) uaddr, FUTEX_WAIT, value, NULL, NULL, 0);
}

static inline int futex_wake(_Atomic int *uaddr, int value) {
    return syscall(SYS_futex, (int *) uaddr, FUTEX_WAKE, value, NULL, NULL, 0);
}

int mythread_join(mythread_t *thread, void **retval) {
    if (!thread)
        return -1;

    

    return 0;
}

int mythread_detach(mythread_t *thread) {
    if (!thread)
        return -1;

    
    
    return 0;
}

int mythread_create(mythread_t *thread, void *(*start_routine)(void *), void *arg) {
    if (!thread || !start_routine)
        return -1;

    return 0;
}