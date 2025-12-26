#define _GNU_SOURCE

#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define PROXY_PORT 80
#define CONN_Q_SIZE 256
#define BUFFER_SIZE 160 * 1024
#define METHODS_NUM 128
#define H_NAME_SIZE 256
#define H_VAL_SIZE 1024

typedef struct HTTP_HEADER {
    char h_name[H_NAME_SIZE];
    char h_value[H_VAL_SIZE];
} HEADER;

int check_meth_line(char *buffer, char *url) {
    char method[16];
    char version[16];

    if (sscanf(buffer, "%15s %2047s %15s", method, url, version) != 3) {
        printf("Failed to parse HTTP query mettod line");
        free(buffer);

        return 1;
    }

    if (strncmp(method, "GET", 3) != 0) {
        printf("Sorry, but only GET method is supported");
        free(buffer);

        return 1;
    }

    if (strncmp(version, "HTTP/1.0", 8) != 0) {
        printf("Sorry, but only HTTP/1.0 unput version is supported");
        free(buffer);

        return 1;
    }

    return 0;
}

void *handle_client(void *arg) {
    int client_sock_fd = *(int *)arg;
    free(arg);

    char *buffer = (char *)calloc(BUFFER_SIZE, sizeof(char));
    if (!buffer) {
        perror("Failed to allocate memory for connection");
        close(client_sock_fd);
        return NULL;
    }

    ssize_t readed = read(client_sock_fd, buffer, BUFFER_SIZE - 1);
    if (readed == -1) {
        perror("Failed when read HTTP query");
        close(client_sock_fd);
        free(buffer);
        return NULL;
    }
    else if (readed == 0) {
        printf("Client sent end of file");
        close(client_sock_fd);
        free(buffer);
        return NULL;
    }

    char url[2048];
    if (check_meth_line(buffer, url)) {
        close(client_sock_fd);
        return NULL;
    }

    

    close(client_sock_fd);
    return NULL;
}

int main (int argc, char** argv) {
    int cli_list_sock_fd;
    if ((cli_list_sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Cannot create socket");
        return 1;
    }

    struct sockaddr_in listen_addr;
    memset(&listen_addr, 0, sizeof(listen_addr));
    listen_addr.sin_family = AF_INET;
    listen_addr.sin_addr.s_addr = INADDR_ANY;
    listen_addr.sin_port = htons(PROXY_PORT);

    int is_reuse_addr = 1;
    if (setsockopt(cli_list_sock_fd, SOL_SOCKET, SO_REUSEADDR, &is_reuse_addr, sizeof(is_reuse_addr)) < 0) {
        perror("Cannot set reuse option for socket");
        close(cli_list_sock_fd);
        return 1;
    }

    if (bind(cli_list_sock_fd, (const struct sockaddr *)&listen_addr, sizeof(listen_addr)) < 0) {
        perror("Cannot bind socket");
        close(cli_list_sock_fd);
        return 1;
    }

    if (listen(cli_list_sock_fd, CONN_Q_SIZE) < 0) {
        perror("Cannot set listenning on socket");
        close(cli_list_sock_fd);
        return 1;
    }

    printf("HTTP proxy-server listening at the following port %d\n", PROXY_PORT);

    while(1) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_sock_fd;
        if ((client_sock_fd = accept(cli_list_sock_fd, (struct sockaddr *)&client_addr, &client_addr_len)) < 0) {
            perror("Cannot accept new client");
            continue;
        }

        int *arg = (int *)malloc(sizeof(int));
        if (!arg) {
            perror("Cannot allocate memory for thread function argument");
            continue;
        }
        *arg = client_sock_fd;

        pthread_t connection_tid;
        pthread_attr_t connection_attr;

        pthread_attr_init(&connection_attr);
        pthread_attr_setdetachstate(&connection_attr, PTHREAD_CREATE_DETACHED);

        if (pthread_create(&connection_tid, &connection_attr, handle_client, arg) != 0) {
            perror("Cannot create thread for client handling");
            free(arg);
            close(client_sock_fd);
        }

        pthread_attr_destroy(&connection_attr);
    }

    close(cli_list_sock_fd);

    return 0;
}