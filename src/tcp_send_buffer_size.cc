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
#include <string.h>
#include <string.h>

#include <iostream>

#define BUFFER_SIZE 512

int main(int argc, char* argv[]) {
    if(argc <= 3) {
        std::cout << "usage : " << argv[0] << " ip_address port_number send_buffer_size\n";
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

    int send_buf = atoi(argv[3]);
    int len = sizeof(send_buf);

    setsockopt(sock_fd, SOL_SOCKET, SO_SNDBUF, &send_buf, sizeof(send_buf));
    getsockopt(sock_fd, SOL_SOCKET, SO_SNDBUF, &send_buf, (socklen_t*)&len);
    std::cout << "the tcp send buffer size after setting is " << send_buf << "\n";



    if(connect(sock_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        std::cout << "connection failed\n";
    } else {
        char buffer[BUFFER_SIZE];
        memset(buffer, 'a', BUFFER_SIZE);
        send(sock_fd, buffer, BUFFER_SIZE, 0);
    }
    close(sock_fd);
    return 0;
}