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
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <iostream>

#define MAX_EVENT_NUMBER 1024
#define BUFFER_SIZE 1024

struct fds {
    int epoll_fd;
    int sock_fd;
};

int set_non_blocking(int fd) {
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

void add_fd(int epoll_fd, int fd, bool one_shot) {
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;
    if(one_shot) {
        event.events |= EPOLLONESHOT;
    }
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event);
    set_non_blocking(fd);
}

void reset_one_shot(int epoll_fd, int fd) {
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
    epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &event);
}

void* worker(void* arg) {
    int sock_fd = ((fds*)arg)->sock_fd;
    int epoll_fd = ((fds*)arg)->epoll_fd;
    std::cout << "start new thread to receive data on fd :" << sock_fd << "\n";
    char buf[BUFFER_SIZE];
    memset(buf, '\0', BUFFER_SIZE);

    while(true) {
        int ret = recv(sock_fd, buf, BUFFER_SIZE - 1, 0);
        if(ret == 0) {
            std::cout << "client closed the connection\n";
            close(sock_fd);
            break;
        } else if(ret < 0) {
            if(errno == EAGAIN) {
                reset_one_shot(epoll_fd, sock_fd);
                std::cout << "read later\n";
                break;
            }
        } else {
            std::cout << "get content: " << buf <<"\n";
            sleep(5);
        }
    }

    std::cout << "end thread receiving data on fd: " << sock_fd << "\n";
}

int main(int argc, char* argv[]) {
    if(argc <= 2) {
        std::cout << "usage " << basename(argv[0]);
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

    int ret = bind(listen_fd, (struct sockaddr*)&address, sizeof(address));
    assert(ret != -1);

    ret = listen(listen_fd, 5);
    assert(ret != -1);

    epoll_event events[MAX_EVENT_NUMBER];
    int epoll_fd = epoll_create(5);
    assert(epoll_fd);

    add_fd(epoll_fd, listen_fd, false);

    while(true) {
        int ret = epoll_wait(epoll_fd, events, MAX_EVENT_NUMBER, -1);
        if(ret < 0) {
            std::cout << "epoll failure\n";
            break;
        }
        for(int i = 0; i < ret; i++) {
            int sock_fd = events[i].data.fd;
            if(sock_fd == listen_fd) {
                struct sockaddr_in client_address;
                socklen_t client_address_length = sizeof(client_address);
                int connection_fd = accept(listen_fd, (struct sockaddr*)&client_address, &client_address_length);
                add_fd(epoll_fd, connection_fd, true);
            } else if(events[i].events & EPOLLIN) {
                pthread_t thread;
                fds fds_for_new_worker;
                fds_for_new_worker.epoll_fd = epoll_fd;
                fds_for_new_worker.sock_fd = sock_fd;
                pthread_create(&thread, NULL, worker, (void*)&fds_for_new_worker);
            } else {
                std::cout << "something else happened\n";
            }
        }
    }

    close(listen_fd);
    return 0;

}