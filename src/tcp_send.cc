//
// Created by Reckful on 2018/12/5.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>

#include <iostream>

int main(int argc, char* argv[]) {
    if(argc <= 2) {
        std::cout << "usage : " << argv[0] << " ip_address port_number\n";
        return 1;
    }

    const char* ip = argv[1];
    int port = atoi(argv[2]);

    struct sockaddr_in server_address{};
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &server_address.sin_addr);
    server_address.sin_port = htons(port);

    int sock_fd = socket(PF_INET, SOCK_STREAM, 0);
    assert(sock_fd >= 0);

    if(connect(sock_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        std::cout << "connection failed\n";
    } else {
        const char* oob_data = "abc";
        const char* normal_data = "123";
        send(sock_fd, normal_data, strlen(normal_data), 0);
        send(sock_fd, oob_data, strlen(oob_data), MSG_OOB);
        send(sock_fd, normal_data, strlen(normal_data), 0);
    }
    close(sock_fd);
    return 0;
}