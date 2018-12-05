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
    std::ios::sync_with_stdio(false);
    if(argc <= 2) {
        std::cout << "usage : " << argv[0] << " ip_address port_number\n";
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

    int ret = bind(sock, (struct sockaddr*)&address, sizeof(address));
    assert(ret != -1);

    ret= listen(sock, 2);
    assert(ret != -1);

    sleep(20);

    struct sockaddr_in client{};
    socklen_t client_address_length = sizeof(client);
    int conn_fd = accept(sock, (struct sockaddr*)&client, &client_address_length);
    if(conn_fd < 0) {
        std::cout << "errno is: " << errno << "\n";
    } else {
        char remote[INET_ADDRSTRLEN];
        std::cout << "connected with ip: " << inet_ntop(AF_INET, &client.sin_addr, remote, INET_ADDRSTRLEN) << " and port: " << ntohs(client.sin_port) << "\n";
        close(conn_fd);
    }

    close(sock);
    return 0;
}