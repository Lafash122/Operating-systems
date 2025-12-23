#define _GNU_SOURCE

#include "list.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <string.h>
#include <unistd.h>

//#define CHECKING
//#define SWAPPING

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
    if (list->size < 2)
        return NULL;

    #ifdef CHECKING
    int frst_check = 1;
    #endif

    while(atomic_load(&running)) {
        Node *head = list->first;
        if (!head) {
            atomic_fetch_add(&iters_incr, 1);
            continue;
        }

        pthread_mutex_lock(&(head->sync));
        Node *curr = head->next;
        if (!curr) {
            atomic_fetch_add(&iters_incr, 1);
            pthread_mutex_unlock(&(head->sync));
            continue;
        }

        #ifdef CHECKING
        usleep(100000);
        int checked = 0;
        #endif

        pthread_mutex_lock(&(curr->sync));
        pthread_mutex_unlock(&(head->sync));
        while(curr && curr->next) {
            Node *next = curr->next;

            #ifdef CHECKING
            usleep(10000);
            #endif

            pthread_mutex_lock(&(next->sync));

            if (curr->strlen < next->strlen)
                atomic_fetch_add(&cnt_incr, 1);

            pthread_mutex_unlock(&(curr->sync));

            curr = next;

            #ifdef CHECKING
            checked++;
            #endif
        }
        pthread_mutex_unlock(&(curr->sync));

        atomic_fetch_add(&iters_incr, 1);

        #ifdef CHECKING
        if ((checked < list->size - 1) & frst_check) {
            printf("INCR: Checked nodes: %d, but needed to check: %d. Bug on %lu iteration\n", checked, list->size - 1, iters_incr);
            frst_check = 0;
        }
        #endif
    }

    return NULL;
}

void *check_decr(void *arg) {
    Storage *list = (Storage *)arg;
    if (list->size < 2)
        return NULL;

    #ifdef CHECKING
    int frst_check = 1;
    #endif

    while(atomic_load(&running)) {
        Node *head = list->first;
        if (!head) {
            atomic_fetch_add(&iters_decr, 1);
            continue;
        }

        pthread_mutex_lock(&(head->sync));
        Node *curr = head->next;
        if (!curr) {
            atomic_fetch_add(&iters_decr, 1);
            pthread_mutex_unlock(&(head->sync));
            continue;
        }

        #ifdef CHECKING
        usleep(100000);
        int checked = 0;
        #endif

        pthread_mutex_lock(&(curr->sync));
        pthread_mutex_unlock(&(head->sync));
        while(curr && curr->next) {
            Node *next = curr->next;

            #ifdef CHECKING
            usleep(10000);
            #endif

            pthread_mutex_lock(&(next->sync));

            if (curr->strlen > next->strlen)
                atomic_fetch_add(&cnt_decr, 1);

            pthread_mutex_unlock(&(curr->sync));

            curr = next;

            #ifdef CHECKING
            checked++;
            #endif
        }
        pthread_mutex_unlock(&(curr->sync));

        atomic_fetch_add(&iters_decr, 1);

        #ifdef CHECKING
        if ((checked < list->size - 1) & frst_check) {
            printf("DECR: Checked nodes: %d, but needed to check: %d. Bug on %lu iteration\n", checked, list->size - 1, iters_decr);
            frst_check = 0;
        }
        #endif
    }

    return NULL;
}

void *check_comp(void *arg) {
    Storage *list = (Storage *)arg;
    if (list->size < 2)
        return NULL;

    #ifdef CHECKING
    int frst_check = 1;
    #endif

    while(atomic_load(&running)) {
        Node *head = list->first;
        if (!head) {
            atomic_fetch_add(&iters_comp, 1);
            continue;
        }

        pthread_mutex_lock(&(head->sync));
        Node *curr = head->next;
        if (!curr) {
            atomic_fetch_add(&iters_comp, 1);
            pthread_mutex_unlock(&(head->sync));
            continue;
        }

        #ifdef CHECKING
        usleep(100000);
        int checked = 0;
        #endif

        pthread_mutex_lock(&(curr->sync));
        pthread_mutex_unlock(&(head->sync));
        while(curr && curr->next) {
            Node *next = curr->next;

            #ifdef CHECKING
            usleep(10000);
            #endif

            pthread_mutex_lock(&(next->sync));

            if (curr->strlen == next->strlen)
                atomic_fetch_add(&cnt_comp, 1);

            pthread_mutex_unlock(&(curr->sync));

            curr = next;

            #ifdef CHECKING
            checked++;
            #endif
        }
        pthread_mutex_unlock(&(curr->sync));

        atomic_fetch_add(&iters_comp, 1);

        #ifdef CHECKING
        if ((checked < list->size - 1) & frst_check) {
            printf("COMP: Checked nodes: %d, but needed to check: %d. Bug on %lu iteration\n", checked, list->size - 1, iters_comp);
            frst_check = 0;
        }
        #endif
    }

    return NULL;
}

void *do_swap_incr(void *arg) {
    Storage *list = (Storage *)arg;
    if (list->size < 2)
        return NULL;

    unsigned int seed = (unsigned int)pthread_self() ^ (unsigned int)time(NULL);

    #ifdef SWAPPING
    int first_swap = 1;
    #endif

    while(atomic_load(&running)) {
        int swap_ind = rand_r(&seed) % (list->size - 1);

        #ifdef SWAPPING
        if (first_swap)
            swap_ind = 0;
        else
            swap_ind = 10;
        #endif

        Node *prev = list->first;
        if (!prev)
            break;

        pthread_mutex_lock(&(prev->sync));
        Node *left = prev->next;
        if (!left) {
            pthread_mutex_unlock(&(prev->sync));
            break;
        }

        pthread_mutex_lock(&(left->sync));
        for (int i = 0; i < swap_ind; i++) {
            Node *right = left->next;
            if (!right)
                break;

            pthread_mutex_lock(&(right->sync));
            pthread_mutex_unlock(&(prev->sync));
            prev = left;
            left = right;
        }

        Node *right = left->next;
        if (!right) {
            pthread_mutex_unlock(&(left->sync));
            pthread_mutex_unlock(&(prev->sync));
            break;
        }
        
        pthread_mutex_lock(&(right->sync));
        if (left->strlen > right->strlen) {
            prev->next = right;
            left->next = right->next;
            right->next = left;

            atomic_fetch_add(&swap_incr, 1);
        }

        pthread_mutex_unlock(&(right->sync));
        pthread_mutex_unlock(&(left->sync));
        pthread_mutex_unlock(&(prev->sync));
    }

    return NULL;
}

void *do_swap_decr(void *arg) {
    Storage *list = (Storage *)arg;
    if (list->size < 2)
        return NULL;

    unsigned int seed = (unsigned int)pthread_self() ^ (unsigned int)time(NULL);

    #ifdef SWAPPING
    sleep(1);
    #endif

    while(atomic_load(&running)) {
        int swap_ind = rand_r(&seed) % (list->size - 1);

        #ifdef SWAPPING
        swap_ind = list->size - 2;
        #endif

        Node *prev = list->first;
        if (!prev)
            break;

        pthread_mutex_lock(&(prev->sync));
        Node *left = prev->next;
        if (!left) {
            pthread_mutex_unlock(&(prev->sync));
            break;
        }

        pthread_mutex_lock(&(left->sync));
        for (int i = 0; i < swap_ind; i++) {
            Node *right = left->next;
            if (!right) {
                #ifdef SWAPPING
                printf("Stop on %d but needed on %d\n", i, swap_ind);
                #endif

                break;
            }

            pthread_mutex_lock(&(right->sync));
            pthread_mutex_unlock(&(prev->sync));
            prev = left;
            left = right;
        }

        Node *right = left->next;
        if (!right) {
            pthread_mutex_unlock(&(left->sync));
            pthread_mutex_unlock(&(prev->sync));
            break;
        }
        
        pthread_mutex_lock(&(right->sync));
        if (left->strlen < right->strlen) {
            prev->next = right;
            left->next = right->next;
            right->next = left;

            atomic_fetch_add(&swap_decr, 1);
        }

        pthread_mutex_unlock(&(right->sync));
        pthread_mutex_unlock(&(left->sync));
        pthread_mutex_unlock(&(prev->sync));
    }

    return NULL;
}

void *do_swap_comp(void *arg) {
    Storage *list = (Storage *)arg;
    if (list->size < 2)
        return NULL;

    unsigned int seed = (unsigned int)pthread_self() ^ (unsigned int)time(NULL);

    while(atomic_load(&running)) {
        int swap_ind = rand_r(&seed) % (list->size - 1);

        #ifdef SWAPPING
        swap_ind = 20;
        #endif

        Node *prev = list->first;
        if (!prev)
            break;

        pthread_mutex_lock(&(prev->sync));
        Node *left = prev->next;
        if (!left) {
            pthread_mutex_unlock(&(prev->sync));
            break;
        }

        pthread_mutex_lock(&(left->sync));
        for (int i = 0; i < swap_ind; i++) {
            Node *right = left->next;
            if (!right)
                break;

            pthread_mutex_lock(&(right->sync));
            pthread_mutex_unlock(&(prev->sync));
            prev = left;
            left = right;
        }

        Node *right = left->next;
        if (!right) {
            pthread_mutex_unlock(&(left->sync));
            pthread_mutex_unlock(&(prev->sync));
            break;
        }
        
        pthread_mutex_lock(&(right->sync));
        if (left->strlen != right->strlen) {
            prev->next = right;
            left->next = right->next;
            right->next = left;

            atomic_fetch_add(&swap_comp, 1);
        }

        pthread_mutex_unlock(&(right->sync));
        pthread_mutex_unlock(&(left->sync));
        pthread_mutex_unlock(&(prev->sync));
    }

    return NULL;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("bad input\n");
        return -1;
    }
    srand(time(NULL));

    int list_size = atoi(argv[1]);

    Storage *list = list_init(list_size);

    pthread_t tincr, tdecr, tcomp;
    pthread_t swincr, swdecr, swcomp;

    pthread_create(&tincr, NULL, check_incr, list);
    pthread_create(&tdecr, NULL, check_decr, list);
    pthread_create(&tcomp, NULL, check_comp, list);

    pthread_create(&swincr, NULL, do_swap_incr, list);
    pthread_create(&swdecr, NULL, do_swap_decr, list);
    pthread_create(&swcomp, NULL, do_swap_comp, list);

    sleep(10);
    atomic_store(&running, 0);

    pthread_join(tincr, NULL);
    pthread_join(tdecr, NULL);
    pthread_join(tcomp, NULL);

    pthread_join(swincr, NULL);
    pthread_join(swdecr, NULL);
    pthread_join(swcomp, NULL);

    printf("Mutex size: %d -- on: 10 seconds\n", list_size);
    printf("iters_incr: %lu, cnt_incr: %lu, swap_incr: %lu\n", iters_incr, cnt_incr, swap_incr);
    printf("iters_decr: %lu, cnt_decr: %lu, swap_decr: %lu\n", iters_decr, cnt_decr, swap_decr);
    printf("iters_comp: %lu, cnt_comp: %lu, swap_comp: %lu\n", iters_comp, cnt_comp, swap_comp);

    list_destroy(list);

    return 0;
}