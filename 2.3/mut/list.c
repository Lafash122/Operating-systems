#define _GNU_SOURCE

#include "list.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

size_t gen_random_string(char *str, int strsize) {
    unsigned int seed = rand();
    size_t len = rand_r(&seed) % (strsize - 1) + 1;
    for(size_t i = 0; i < len; i++)
        str[i] = 'a' + (rand_r(&seed) % 26);
    str[len] = '\0';

    return len;
}

Node *node_init() {
    Node *node = (Node *)malloc(sizeof(Node));
    if (!node) {
        printf("Cannot allocate memory for node of the list\n");
        abort();
    }

    node->strlen = gen_random_string(node->value, 100);
    pthread_mutex_init(&(node->sync), NULL);
    node->next = NULL;

    return node;
}

Storage *list_init(int size) {
    srand(time(NULL));
    Storage *list = (Storage *)malloc(sizeof(Storage));
    if (!list) {
        printf("Cannot allocate memory for list\n");
        abort();
    }

    list->size = size;
    list->first = node_init();

    Node *curr = list->first;
    for (int i = 1; i < size; i++) {
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

        pthread_mutex_destroy(&(curr->sync));
        free(curr);

        curr = next;
    }

    free(list);
}

int swap_nodes(Node *prev, Node *left, Node *right) {
    if (!left || !right)
        return 0;
    
    if (prev)
        pthread_mutex_lock(&(prev->sync));
    pthread_mutex_lock(&(left->sync));
    pthread_mutex_lock(&(right->sync));

    if (prev)
        prev->next = right;
    left->next = right->next;
    right->next = left;

    if (prev)
        pthread_mutex_unlock(&(prev->sync));
    pthread_mutex_unlock(&(left->sync));
    pthread_mutex_unlock(&(right->sync));

    return 1;
}