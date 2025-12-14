#define _GNU_SOURCE

#include "mythread.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void *mythread_func(void *arg) {
    int val = *(int *)arg;
    int result = val * 2;
    return (void *)(intptr_t)result;
}

int main() {
    int thread_number = 10;
    int err;
    mythread_t threads[thread_number];

    int data[thread_number];
    for (int i = 0; i < thread_number; i++) {
        data[i] = i;
        err = mythread_create(&threads[i], mythread_func, &data[i]);
        if (err != 0) {
            printf("mypthread_create() failed: %d\n", err);
            return -1;
        }
    }

    for (int i = 0; i < thread_number; i++) {
        if (i % 2 == 0) {
            intptr_t res;
            err = mythread_join(&threads[i], (void **)&res);
            if (err != 0) {
                printf("mypthread_join() failed: %d\n", err);
                return -1;
            }

            data[i] = (int)res;
            printf("thread: %d -- %d\n", i, data[i]);
        }
        else {
            err = mythread_detach(&threads[i]);
            if (err != 0) {
                printf("mypthread_join() failed: %d\n", err);
                return -1;
            }
        }
    }

    return 0;
}