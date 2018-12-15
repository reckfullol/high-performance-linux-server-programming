//
// Created by Reckful on 12/16/2018.
//

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <iostream>

int timeout_connect(const char *ip, int port, int time) {
    struct sockaddr_in address{};
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    inet_pton(AF_INET, ip, &address.sin_addr);

    int sock_fd = socket(PF_INET, SOCK_STREAM, 0);
    assert(sock_fd >= 0);

    struct timeval timeout{};
    timeout.tv_sec = time;
    timeout.tv_usec = 0;
    socklen_t len = sizeof(timeout);

    int ret = setsockopt(sock_fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, len);
    assert(ret != -1);

    ret = connect(sock_fd, (struct sockaddr*)&address, sizeof(address));
    if(ret == -1) {
        if(errno == EINPROGRESS) {
            std::cout << "connecting timeout, process timeout logic\n";
            return -1;
        }
        else {
            std::cout << "error occur when connecting to server\n";
            return -1;
        }
    }
    return sock_fd;
}

int main(int argc, char* argv[]) {
    if(argc <= 2) {
        std::cout << "usage: " << basename(argv[0]) << " ip_address port_number\n";
        return 1;
    }

    const char* ip = argv[1];
    int port = atoi(argv[2]);

    int socket_fd = timeout_connect(ip, port, 10);
    if(socket_fd < 0) {
        return 1;
    }
    return 0;
}