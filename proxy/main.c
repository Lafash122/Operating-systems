#define _GNU_SOURCE

#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#define PROXY_PORT 80
#define CONN_Q_SIZE 256

int main (int argc, char** argv) {
    int cli_list_sock;
    if ((cli_list_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Cannot create socket");
        return 1;
    }

    struct sockaddr_in listen_addr;
    memset(&cli_list_sock, 0, sizeof(cli_list_sock));
    listen_addr.sin_family = AF_INET;
    listen_addr.sin_addr.s_addr = INADDR_ANY;
    listen_addr.sin_port = htons(PROXY_PORT);

    int is_reuse_addr = 1;
    if (setsockopt(cli_list_sock, SOL_SOCKET, SO_REUSEADDR, &is_reuse_addr, sizeof(is_reuse_addr)) < 0) {
        perror("Cannot set reuse option");
        close(cli_list_sock);
        return 1;
    }

    if (bind(cli_list_sock, (const struct sockaddr *)&listen_addr, sizeof(listen_addr)) < 0) {
        perror("Cannot bind socket");
        close(cli_list_sock);
        return 1;
    }

    if (listen(cli_list_sock, CONN_Q_SIZE) < 0) {
        perror("Cannot set listenning on socket");
        close(cli_list_sock);
        return 1;
    }

    printf("HTTP proxy-server listening at the following port %d", PROXY_PORT);

    while(1) {
        pthread_t connection_tid;
        pthread_attr_t connection_attr;

        pthread_attr_init(&connection_attr);
        pthread_attr_setdetachstate(&connection_attr, PTHREAD_CREATE_DETACHED);



        pthread_attr_destroy(&connection_attr);
    }

    return 0;
}