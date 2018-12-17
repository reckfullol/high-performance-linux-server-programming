//
// Created by Reckful on 2018/12/7.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <strings.h>

#include <iostream>

int main(int argc, char* argv[]) {
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
        int pipe_fd[2];
        ret = pipe(pipe_fd);
        assert(ret != -1);

        ret = splice(conn_fd, NULL, pipe_fd[1], NULL, 32768, SPLICE_F_MORE | SPLICE_F_MOVE);
        assert(ret != -1);

        ret = splice(pipe_fd[0], NULL, conn_fd, NULL, 32768, SPLICE_F_MORE | SPLICE_F_MOVE);
        assert(ret = -1);

        close(conn_fd);
    }

    close(sock);
    return 0;
}