#define _GNU_SOURCE

#include "list.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <string.h>
#include <unistd.h>

atomic_int running = 1;

atomic_ulong iters_incr = 0;
atomic_ulong iters_decr = 0;
atomic_ulong iters_comp = 0;

atomic_ulong cnt_incr = 0;
atomic_ulong cnt_decr = 0;
atomic_ulong cnt_comp = 0;

atomic_ulong swap_incr = 0;
atomic_ulong swap_decr = 0;
atomic_ulong swap_comp = 0;

void *check_incr(void *arg) {
    Storage *list = (Storage *)arg;
    while(atomic_load(&running)) {
        Node *curr = list->first;

        while(curr && curr->next) {
            Node *next = curr->next;

            pthread_mutex_lock(&(curr->sync));
            pthread_mutex_lock(&(next->sync));

            if (curr->strlen < next->strlen)
                atomic_fetch_add(&cnt_incr, 1);

            pthread_mutex_unlock(&(next->sync));
            pthread_mutex_unlock(&(curr->sync));

            curr = next;
        }

        atomic_fetch_add(&iters_incr, 1);
    }

    return NULL;
}

void *check_decr(void *arg) {
    Storage *list = (Storage *)arg;
    while(atomic_load(&running)) {
        Node *curr = list->first;

        while(curr && curr->next) {
            Node *next = curr->next;

            pthread_mutex_lock(&(curr->sync));
            pthread_mutex_lock(&(next->sync));

            if (curr->strlen > next->strlen)
                atomic_fetch_add(&cnt_decr, 1);

            pthread_mutex_unlock(&(next->sync));
            pthread_mutex_unlock(&(curr->sync));

            curr = next;
        }

        atomic_fetch_add(&iters_decr, 1);
    }

    return NULL;
}

void *check_comp(void *arg) {
    Storage *list = (Storage *)arg;
    while(atomic_load(&running)) {
        Node *curr = list->first;

        while(curr && curr->next) {
            Node *next = curr->next;

            pthread_mutex_lock(&(curr->sync));
            pthread_mutex_lock(&(next->sync));

            if (curr->strlen == next->strlen)
                atomic_fetch_add(&cnt_comp, 1);

            pthread_mutex_unlock(&(next->sync));
            pthread_mutex_unlock(&(curr->sync));

            curr = next;
        }

        atomic_fetch_add(&iters_comp, 1);
    }

    return NULL;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("bad input\n");
        return -1;
    }

    int list_size = atoi(argv[1]);

    Storage *list = list_init(list_size);

    pthread_t tincr, tdecr, tcomp;
    pthread_create(&tincr, NULL, check_incr, list);
    pthread_create(&tdecr, NULL, check_decr, list);
    pthread_create(&tcomp, NULL, check_comp, list);

    usleep(1000);
    atomic_store(&running, 0);

    pthread_join(tincr, NULL);
    pthread_join(tdecr, NULL);
    pthread_join(tcomp, NULL);

    printf("size: %d\n", list_size);
    printf("iters_incr: %lu, cnt_incr: %lu\n", iters_incr, cnt_incr);
    printf("iters_decr: %lu, cnt_decr: %lu\n", iters_decr, cnt_decr);
    printf("iters_comp: %lu, cnt_comp: %lu\n", iters_comp, cnt_comp);

    list_destroy(list);

    return 0;
}