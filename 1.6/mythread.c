#define _GNU_SOURCE

#include "mythread.h"

#include <sys/syscall.h>
#include <linux/futex.h>
#include <sys/mman.h>
#include <errno.h>
#include <sched.h>

static inline int futex_wait(_Atomic int *uaddr, int value) {
    return syscall(SYS_futex, (int *) uaddr, FUTEX_WAIT, value, NULL, NULL, 0);
}

static inline int futex_wake(_Atomic int *uaddr, int value) {
    return syscall(SYS_futex, (int *) uaddr, FUTEX_WAKE, value, NULL, NULL, 0);
}

static int mythread_routine(void *arg) {
    return 0;
}

int mythread_create(mythread_t *thread, void *(*start_routine)(void *), void *arg) {
    if (!thread || !start_routine)
        return ESRCH;

    thread->start_routine = start_routine;
    thread->arg = arg;

    void *stack = mmap(NULL, STACK_SIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_STACK, -1, 0);
    if (stack == MAP_FAILED)
        return EAGAIN;

    void *stack_head = (char *)stack + STACK_SIZE;

    pid_t mythread_tid = clone(mythread_routine, stack_head,
                            CLONE_VM|CLONE_FS|CLONE_FILES|CLONE_SIGHAND|CLONE_THREAD|CLONE_SYSVSEM|CLONE_SETTLS,
                            thread);

    if (mythread_tid == -1) {
        munmap(stack, STACK_SIZE);
        return EAGAIN;
    }

    thread->tid = mythread_tid;
    thread->stack_size = STACK_SIZE;
    thread->stack = stack;

    atomic_store(&thread->done, 0);
    atomic_store(&thread->detached, 0);
    atomic_store(&thread->joined, 0);

    return 0;
}

int mythread_join(mythread_t *thread, void **retval) {
    if (!thread)
        return ESRCH;

    

    return 0;
}

int mythread_detach(mythread_t *thread) {
    if (!thread)
        return ESRCH;

    
    
    return 0;
}