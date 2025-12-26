#define _GNU_SOURCE

#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define HTTP_PORT 80
#define CONN_Q_SIZE 256
#define BUFFER_SIZE 160 * 1024
#define H_NAME_SIZE 256
#define H_VAL_SIZE 2048

int check_meth_line(char *buffer, char *url) {
    char method[16];
    char version[12];

    if (sscanf(buffer, "%15s %2047s %11s", method, url, version) != 3) {
        printf("Failed to parse HTTP query mettod line");
        return 1;
    }

    if (strncmp(method, "GET", 3) != 0) {
        printf("Sorry, but only GET method is supported");
        return 1;
    }

    if (strncmp(version, "HTTP/1.0", 8) != 0) {
        printf("Sorry, but only HTTP/1.0 unput version is supported");
        return 1;
    }

    return 0;
}

int check_headers(char *buffer, char *host) {
    char *headers = strstr(buffer, "\r\n");
    if (!headers) {
        printf("Cannot find headers");
        return 1;
    }

    headers += 2;
    host[0] = '\0';
    while(1) {
        if ((headers[0] == '\r') && (headers[1] == '\n'))
            break;

        char *colon = strchr(headers, ':');
        if (!colon) {
            printf("Incorrect structure of header: cannot find colon");
            return 1;
        }
        size_t h_name_len = colon - headers;
        if ((h_name_len == 0) || (h_name_len >= H_NAME_SIZE)) {
            printf("Incorrect structure of header: invalid colon location");
            return 1;
        }

        char h_name[H_NAME_SIZE];
        memcpy(h_name, headers, h_name_len);
        h_name[h_name_len] = '\0';

        char *val_start = colon + 1;
        while (*val_start == ' ')
            val_start++;
        char *val_end = strstr(val_start, "\r\n");
        if (!val_end) {
            printf("Incorrect structure of header: cannot find header end");
            return 1;
        }
        size_t h_value_len = val_end - val_start;
        if ((h_value_len == 0) || (h_value_len >= H_VAL_SIZE)) {
            printf("Incorrect structure of header: invalid header end location");
            return 1;
        }

        if (strcasecmp(h_name, "host") == 0) {
            memcpy(host, val_start, h_value_len);
            host[h_value_len] = '\0';
        }

        headers = val_end + 2;
    }

    if (host[0] == 0) {
        printf("Could not find Host header");
        return 1;
    }

    return 0;
}

int resolve_host(char *host, struct sockaddr_in *remote) {
    

    return 0;
}

void *handle_client(void *arg) {
    int client_sock_fd = *(int *)arg;
    free(arg);

    char buffer[BUFFER_SIZE];
    ssize_t readed = read(client_sock_fd, buffer, BUFFER_SIZE - 1);
    if (readed == -1) {
        perror("Failed when read HTTP query");
        close(client_sock_fd);
        return NULL;
    }
    else if (readed == 0) {
        printf("Client sent end of file");
        close(client_sock_fd);
        return NULL;
    }

    char url[2048];
    if (check_meth_line(buffer, url)) {
        close(client_sock_fd);
        return NULL;
    }

    char host[H_VAL_SIZE];
    if (check_headers(buffer, host)) {
        close(client_sock_fd);
        return NULL;
    }

    struct sockaddr_in remote_addr;
    memset(&remote_addr, 0, sizeof(remote_addr));
    remote_addr.sin_family = AF_INET;
    if (resolve_host(host, &remote_addr)) {
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
    listen_addr.sin_port = htons(HTTP_PORT);

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

    printf("HTTP proxy-server listening at the following port %d\n", HTTP_PORT);

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