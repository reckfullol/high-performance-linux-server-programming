//
// Created by Reckful on 2018/12/10.
//

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>

#include <iostream>
#include <libgen.h>

#define BUFFER_SIZE 1023

int set_non_blocking(int fd) {
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

int unblock_connect(const char *ip, int port, int time) {
    struct sockaddr_in address{};
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);

    int sock_fd = socket(PF_INET, SOCK_STREAM, 0);
    int fd_opt = set_non_blocking(sock_fd);
    int ret = connect(sock_fd, (struct sockaddr*)&address, sizeof(address));
    if(ret == 0) {
        std::cout << "connect with server immediately\n";
        fcntl(sock_fd, F_SETFL, fd_opt);
        return sock_fd;
    } else if(errno != EINPROGRESS) {
        std::cout << "unblock connect not support\n";
        return -1;
    }

    fd_set read_fds;
    fd_set write_fds;
    struct timeval timeout{};

    FD_ZERO(&read_fds);
    FD_SET(sock_fd, &write_fds);

    timeout.tv_sec = time;
    timeout.tv_usec = 0;

    ret = select(sock_fd + 1, nullptr, &write_fds, nullptr, &timeout);
    if(ret <= 0) {
        std::cout << "connection time out\n";
        close(sock_fd);
        return -1;
    }

    if(!FD_ISSET(sock_fd, &write_fds)) {
        std::cout << "no events on sock_fd found\n";
        close(sock_fd);
        return -1;
    }

    int error = 0;
    socklen_t length = sizeof(error);
    if(getsockopt(sock_fd, SOL_SOCKET, SO_ERROR, &error, &length) < 0) {
        std::cout << "get socket option failed\n";
        close(sock_fd);
        return -1;
    }

    if(error != 0) {
        std::cout << "connection failed after select with the error: " << error << "\n";
        close(sock_fd);
        return -1;
    }

    std::cout << "connection ready after select with the socket: " << sock_fd << "\n";
    fcntl(sock_fd, F_SETFL, fd_opt);
    return sock_fd;
}

int main(int argc, char* argv[]) {
    if(argc <= 2) {
        std::cout << "usage: " << basename(argv[0]) << " ip_address port_number\n";
        return 1;
    }

    const char* ip = argv[1];
    int port = atoi(argv[2]);

    int sock_fd = unblock_connect(ip, port, 10);
    if(sock_fd < 0) {
        return 1;
    }
    close(sock_fd);
    return 0;
}