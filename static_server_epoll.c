#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <sys/epoll.h>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
#define EPOLL_MAX_NUM 2048
#define BUF_SIZE 4096
#define LISTEN_N 10
char buffer[BUF_SIZE];
char *get_param_form_request(char *request){
    char *s = strstr(request, "/");
    char *s2 = strstr(s, " ");
    int size = strlen(s) - strlen(s2);
    char *result = malloc(sizeof(char*) * size);
    strncpy(result, s, size);
    return result;
}
char *str_join(char* str1,char* str2){
    char *result = malloc(sizeof(char *) * (strlen(str1) + strlen(str2)));
    strcat(result, str1);
    strcat(result, str2);
    return result;
}
int send_header_with_content_type(int client_socket_fd,char *content_type){
    sprintf(buffer, "HTTP/1.1 200 OK\r\nContent-Type:%s:charset=UTF-8\r\n\r\n", content_type);
    int send_n = send(client_socket_fd, buffer, strlen(buffer), 0);
    memset(buffer, 0, strlen(buffer));
    if (send_n < 0) {
        return  -1;
    }
    return 0;
}
int main(int argc, char **argv) {
    char *base_path = "/root/CLionProjects/c_static_server";
    int port = 9000;
    printf("base_path:%s\nport:%d\n", base_path, port);
    int listen_fd = 0;
    int client_fd = 0;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t client_len;
    int epfd = 0;
    struct epoll_event event, *my_events;
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);
    int bind_i = bind(listen_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (bind_i == -1) {
        printf("bind error\n");
        return  -1;
    }
    int listen_i = listen(listen_fd, LISTEN_N);
    if (listen_i == -1) {
        printf("listen error\n");
        return  -1;
    }
    epfd = epoll_create(EPOLL_MAX_NUM);
    if (epfd < 0) {
        perror("epoll create");
        goto END;
    }
    event.events = EPOLLIN;
    event.data.fd = listen_fd;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, listen_fd, &event) < 0) {
        perror("epoll ctl add listen_fd ");
        goto END;
    }
    my_events = malloc(sizeof(struct epoll_event) * EPOLL_MAX_NUM);
    while (1) {
        int active_fds_cnt = epoll_wait(epfd, my_events, EPOLL_MAX_NUM, -1);
        int i = 0;
        for (i = 0; i < active_fds_cnt; i++) {
            if (my_events[i].data.fd == listen_fd) {
                client_fd = accept(listen_fd, (struct sockaddr *) &client_addr, &client_len);
                if (client_fd < 0) {
                    perror("accept");
                    continue;
                }
                char ip[20];
                printf("new connection[%s:%d]\n", inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof(ip)),ntohs(client_addr.sin_port));
                event.events = EPOLLIN | EPOLLET;
                event.data.fd = client_fd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &event);
            } else if (my_events[i].events & EPOLLIN) {
                client_fd = my_events[i].data.fd;
                int n = read(client_fd, buffer, BUF_SIZE);
                if (n < 0) {
                    perror("read");
                    continue;
                } else if (n == 0) {
                    epoll_ctl(epfd, EPOLL_CTL_DEL, client_fd, &event);
                    close(client_fd);
                } else {
                    printf("[read]: %s\n", buffer);
                    char *param = get_param_form_request(buffer);
                    memset(buffer, 0, BUF_SIZE);
                    char *path = str_join(base_path, param);
                    if (access(path, F_OK|R_OK)==0) {
                        //header.
                        char *post_fix = strstr(param, ".");
                        post_fix++;
                        send_header_with_content_type(client_fd, post_fix);
                        //data.
                        FILE *p = fopen(path, "rb");
                        int read2_n;
                        while ((read2_n = fread(buffer, sizeof(char), BUF_SIZE, p)) != 0) {
                            send(client_fd, buffer, read2_n, 0);
                            memset(buffer, 0x00, read2_n);
                        }
                        fclose(p);
                    } else{
                        //header
                        send_header_with_content_type(client_fd, "html");
                        //data
                        send(client_fd, "NOT FOUND", strlen("NOT FOUND"), 0);
                    }
                    close(client_fd);
                }
            }
        }
    }
    END:
    close(epfd);
    close(listen_fd);
    return 0;
}
#pragma clang diagnostic pop