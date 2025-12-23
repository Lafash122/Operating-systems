#ifndef LIST_H
#define LIST_H

#define _GNU_SOURCE

#include <pthread.h>

typedef struct _Node {
    char value[100];
    size_t strlen;
    struct _Node *next;
    pthread_rwlock_t sync;
} Node;

typedef struct _Storage {
    Node *first;
    int size;
} Storage;

Storage *list_init(int size);
void list_destroy(Storage *list);

#endif //LIST_H