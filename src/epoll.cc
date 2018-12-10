//
// Created by Reckful on 2018/12/10.
//

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

#include <iostream>

#define MAX_EVENT_NUMBER 1024
#define BUFFER_SIZE 10

int set_non_blocking(int fd) {
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

void add_fd(int epoll_fd, int fd, bool enable_et) {
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN;
    if(enable_et) {
        event.events |= EPOLLET;
    }
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event);
    set_non_blocking(fd);
}

void lt(epoll_event* events, int number, int epoll_fd, int listen_fd) {
    char buf[BUFFER_SIZE];
    for(int i = 0; i < number; i++) {
        int sock_fd = events[i].data.fd;
        if(sock_fd == listen_fd) {
            struct sockaddr_in client_address;
            socklen_t client_address_length = sizeof(client_address);
            int connection_fd = accept(listen_fd, (struct sockaddr*)&client_address, &client_address_length);
            add_fd(epoll_fd, connection_fd, false);
        } else if(events[i].events & EPOLLIN) {
            std::cout << "event trigger once\n";
            memset(buf, '\0', BUFFER_SIZE);
            int ret = recv(sock_fd, buf, BUFFER_SIZE - 1, 0);
            if(ret <= 0) {
                close(sock_fd);
                continue;
            }
            std::cout << "get " << ret << " byte of content: " << buf << "\n";
        } else {
            std::cout << "something else happened\n";
        }
    }

}

void et(epoll_event* events, int number, int epoll_fd, int listen_fd) {
    char buf[BUFFER_SIZE];
    for(int i = 0; i < number; i++) {
        int sock_fd = events[i].data.fd;
        if(sock_fd == listen_fd) {
            struct sockaddr_in client_address;
            socklen_t client_address_length = sizeof(client_address);
            int connection_fd = accept(listen_fd, (struct sockaddr*)&client_address, &client_address_length);
            add_fd(epoll_fd, connection_fd, false);
        } else if(events[i].events & EPOLLIN) {
            std::cout << "event trigger once\n";
            while(true) {
                memset(buf, '\0', BUFFER_SIZE);
                int ret = recv(sock_fd, buf, BUFFER_SIZE - 1, 0);
                if(ret < 0) {
                    if(errno == EAGAIN || errno == EWOULDBLOCK) {
                        std::cout << "reader later\n";
                        break;
                    }
                    close(sock_fd);
                    break;
                } else if(ret == 0) {
                    close(sock_fd);
                } else {
                    std::cout << "get " << ret << " bytes of contents: " << buf << "\n";
                }
            }
        } else {
            std::cout << "something else happened\n";
        }
    }

}

int main(int argc, char* argv[]) {

    if(argc <= 2) {
        std::cout << "usage : " << argv[0] << " ip_address port_number\n";
        return 1;
    }

    const char* ip = argv[1];
    int port = atoi(argv[2]);

    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);

    int listen_fd = socket(PF_INET, SOCK_STREAM, 0);
    assert(listen_fd >= 0);

    long ret = bind(listen_fd, (struct sockaddr*)&address, sizeof(address));
    assert(ret != -1);

    ret = listen(listen_fd, 5);
    assert(ret != -1);

    epoll_event events[MAX_EVENT_NUMBER];
    int epoll_fd = epoll_create(5);
    assert(epoll_fd != -1);
    add_fd(epoll_fd, listen_fd, true);

    while(true) {
        int ret = epoll_wait(epoll_fd, events, MAX_EVENT_NUMBER, -1);
        if(ret < 0) {
            std::cout << "epoll failure\n";
            break;
        }
//        lt(events, ret, epoll_fd, listen_fd);
        et(events, ret, epoll_fd, listen_fd);
    }

    close(listen_fd);
    return 0;
}