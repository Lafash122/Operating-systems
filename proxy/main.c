#define _GNU_SOURCE

#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

#define PROXY_PORT 80

int main (int argc, char** argv) {
    while(1) {
        pthread_t connection_tid;
        pthread_attr_t connection_attr;

        pthread_attr_init(&connection_attr);
        pthread_attr_setdetachstate(&connection_attr, PTHREAD_CREATE_DETACHED);

    }

    return 0;
}