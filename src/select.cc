//
// Created by Reckful on 2018/12/9.
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
#include <iostream>

int main(int argc, char* argv[]) {
    if(argc <= 2) {
        std::cout << "usage:  " << argv[0] << " ip_address port_number\n";
        return 1;
    }

    const char* ip = argv[1];
    int port = atoi(argv[2]);

    struct sockaddr_in address{};
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

    struct sockaddr_in client_address{};
    socklen_t client_address_length = sizeof(client_address);

    int connection_fd = accept(listen_fd, (struct sockaddr*)&client_address, &client_address_length);
    if(connection_fd < 0) {
        std::cout << "errno is: " << errno << "\n";
        close(listen_fd);
    }

    char buffer[1024];
    fd_set read_fds;
    fd_set exception_fds;
    FD_ZERO(&read_fds);
    FD_ZERO(&exception_fds);

    while(true) {
        memset(buffer, 0, sizeof(buffer));
        FD_SET(connection_fd, &read_fds);
        FD_SET(connection_fd, &exception_fds);

        ret = select(connection_fd + 1, &read_fds, nullptr, &exception_fds, nullptr);
        if(ret < 0) {
            std::cout << "selection failure\n";
            break;
        }

        if(FD_ISSET(connection_fd, &read_fds)) {
            ret = recv(connection_fd, buffer, sizeof(buffer) - 1, 0);
            if(ret <= 0) {
                break;
            }
            std::cout << "get " << ret << " bytes of normal data: " << buffer << "\n";
        } else if(FD_ISSET(connection_fd, &exception_fds)) {
            ret = recv(connection_fd, buffer, sizeof(buffer) - 1, MSG_OOB);
            if(ret <= 0) {
                break;
            }
            std::cout << "get " << ret << " bytes of oob data: " << buffer << "\n";
        }
    }
    close(connection_fd);
    close(listen_fd);
    return 0;
}