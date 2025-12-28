#define _GNU_SOURCE

#include "list.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

size_t gen_rand_str(char *str, int strsize) {
    size_t len = rand() % (strsize - 1) + 1;
    for(size_t i = 0; i < len; i++)
        str[i] = 'a' + (rand() % 26);
    str[len] = '\0';

    return len;
}

Node *node_init() {
    Node *node = (Node *)malloc(sizeof(Node));
    if (!node) {
        printf("Cannot allocate memory for node of the list\n");
        abort();
    }

    node->strlen = gen_rand_str(node->value, 100);
    pthread_rwlock_init(&(node->sync), NULL);
    node->next = NULL;

    return node;
}

Node *first_init() {
    Node *node = (Node *)malloc(sizeof(Node));
    if (!node) {
        printf("Cannot allocate memory for node of the list\n");
        abort();
    }

    node->value[0] = '\0';
    node->strlen = 0;
    pthread_rwlock_init(&(node->sync), NULL);
    node->next = NULL;

    return node;
}

Storage *list_init(int size) {
    Storage *list = (Storage *)malloc(sizeof(Storage));
    if (!list) {
        printf("Cannot allocate memory for list\n");
        abort();
    }

    list->first = first_init();
    list->size = size;

    Node *curr = list->first;
    for (int i = 0; i < size; i++) {
        curr->next = node_init();
        curr = curr->next;
    }

    return list;
}

void list_destroy(Storage *list) {
    if (!list)
        return;

    Node *curr = list->first;
    while (curr) {
        Node *next = curr->next;

        pthread_rwlock_destroy(&(curr->sync));
        free(curr);

        curr = next;
    }

    free(list);
}