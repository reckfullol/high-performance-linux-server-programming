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

#define BUF_SIZE 1024

int main(int argc, char* argv[]) {
    if(argc <= 2) {
        std::cout << "usage : " << argv[0] << " ip_address port_number recv_buffer_size\n";
        return 1;
    }

    const char* ip = argv[1];
    int port = atoi(argv[2]);

    struct sockaddr_in address{};
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    assert(sock >= 0);

    int recv_buf = atoi(argv[3]);
    int len = sizeof(recv_buf);

    setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &recv_buf, sizeof(recv_buf));
    getsockopt(sock, SOL_SOCKET, SO_RCVBUF, &recv_buf, (socklen_t*)&len);
    std::cout << "the tcp receive buffer size after setting is " << recv_buf << '\n';

    long ret = bind(sock, (struct sockaddr*)&address, sizeof(address));
    assert(ret != -1);

    ret = listen(sock, 5);
    assert(ret != -1);

    struct sockaddr_in client{};
    socklen_t client_addr_length = sizeof(client);

    int conn_fd = accept(sock, (struct sockaddr*)&client, &client_addr_length);
    if(conn_fd < 0) {
        std::cout << "errno is: " << errno << "\n";
    } else {
        char buffer[BUF_SIZE];

        memset(buffer, 0, BUF_SIZE);
        while(recv(conn_fd, buffer, BUF_SIZE - 1, 0) > 0) {

        }

        close(conn_fd);
    }

    close(sock);
    return 0;
}