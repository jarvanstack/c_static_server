#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include "stdlib.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

#define BUF_SIZE 1024
#define NOT_FOUND_DATA "NOT_FOUND"
char buffer[BUF_SIZE];

char *get_param_form_request(char *request) {
    char *s = strstr(request, "/");
    char *s2 = strstr(s, " ");
    int size = strlen(s) - strlen(s2);
    char *result = malloc(sizeof(char *) * size);
    strncpy(result, s, size);
    return result;
}

char *str_join(char *str1, char *str2) {
    char *result = malloc(sizeof(char *) * (strlen(str1) + strlen(str2)));
    strcat(result, str1);
    strcat(result, str2);
    return result;
}

int send_header_with_content_type(int client_socket_fd, char *content_type) {
    sprintf(buffer, "HTTP/1.1 200 OK\r\nContent-Type:%s:charset=UTF-8\r\n\r\n", content_type);
    int send_n = send(client_socket_fd, buffer, strlen(buffer), 0);
    memset(buffer, 0, strlen(buffer));
    if (send_n < 0) {
        return -1;
    }
    return 0;
}

//socket -> bind -> listen
int main(int argc, char *argv[]) {
    int port = 9000;
    char *base_path = "/root/CLionProjects/c_static_server";
    int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0) {
        perror("socket error\n");
        return -1;
    }
    struct sockaddr_in lst_addr;
    lst_addr.sin_family = AF_INET;
    lst_addr.sin_port = htons(port);
    lst_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    socklen_t len = sizeof(struct sockaddr_in);
    int ret = bind(sockfd, (struct sockaddr *) &lst_addr, len);
    if (ret < 0) {
        perror("bind error\n");
        return -1;
    }
    if (listen(sockfd, 5) < 0) {
        perror("listen error\n");
        return -1;
    }
    while (1) {
        struct sockaddr_in cli_addr;
        int client_accept_fd = accept(sockfd, (struct sockaddr *) &cli_addr, &len);
        if (client_accept_fd < 0) {
            perror("accept error\n");
            continue;
        }
        int ret = recv(client_accept_fd, buffer, BUF_SIZE, 0);
        if (ret > 0) {
            printf("req:%s\n", buffer);
        }
        char *param = get_param_form_request(buffer);
        memset(buffer, 0x00, BUF_SIZE);
        char *path = str_join(base_path, param);
        if (access(path, F_OK | R_OK) == 0) {
            //header.
            char *post_fix = strstr(param, ".");
            post_fix++;
            send_header_with_content_type(client_accept_fd, post_fix);
            //data.
            FILE *p = fopen(path, "rb");
            int read2_n;
            while ((read2_n = fread(buffer, sizeof(char), BUF_SIZE, p)) != 0) {
                send(client_accept_fd, buffer, read2_n, 0);
                memset(buffer, 0, read2_n);
            }
            fclose(p);
        } else {
            //header
            send_header_with_content_type(client_accept_fd, "html");
            //data
            send(client_accept_fd, NOT_FOUND_DATA, strlen(NOT_FOUND_DATA), 0);
        }
        send(client_accept_fd, buffer, strlen(buffer), 0);
        close(client_accept_fd);
    }
}

#pragma clang diagnostic pop