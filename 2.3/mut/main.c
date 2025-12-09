#define _GNU_SOURCE

#include "list.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <string.h>

atomic_ulong t_incr = 0;
atomic_ulong t_decr = 0;
atomic_ulong t_comp = 0;

atomic_ulong swap_incr = 0;
atomic_ulong swap_decr = 0;
atomic_ulong swap_comp = 0;

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("bad input\n");
        return -1;
    }

    int list_size = atoi(argv[1]);

    Storage *list = list_init(list_size);



    list_destroy(list);

    return 0;
}