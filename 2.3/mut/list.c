#define _GNU_SOURCE

#include "list.h"

#include <stdlib.h>
#include <stdio.h>

void gen_random_string(char *str, int strsize) {
    unsigned int seed = (unsigned int) pthread_self();
    size_t len = rand_r(&seed) % (strsize - 1) + 1;
    for(int i = 0; i < len; i++)
        str[i] = 'a' + (rand_r(&seed) % 26);
    str[len] = '\0';
}

Node *node_init() {
    Node *node = (Node *)malloc(sizeof(Node));
    if (!node) {
        printf("Cannot allocate memory for node of the list");
        abort();
    }

    gen_random_string(node->value, 100);
    pthread_mutex_init(&(node->sync), NULL);
    node->next = NULL;

    return node;
}

Storage *list_init(int size) {
    Storage *list = (Storage *)malloc(sizeof(Storage));
    if (!list) {
        printf("Cannot allocate memory for list");
        abort();
    }

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