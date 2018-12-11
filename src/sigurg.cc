//
// Created by Reckful on 2018/12/11.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <libgen.h>

#include <fcntl.h>
#include <iostream>

#define BUF_SIZE 1024

static int connection_fd;

void sig_urg(int sig) {
    int save_errno = errno;

    char buffer[BUF_SIZE];
    memset(buffer, 0, sizeof(buffer));

    long ret = recv(connection_fd, buffer, BUF_SIZE - 1, MSG_OOB);
    std::cout << "got " << ret << " bytes of oob data " << buffer << "\n";
    errno = save_errno;
}

void add_sig(int sig, void(*sig_handler)(int)) {
    struct sigaction sa{};
    memset(&sa, 0, sizeof(sa));

    sa.__sigaction_u.__sa_handler = sig_handler;
    sa.sa_flags |= SA_RESTART;
    sigfillset(&sa.sa_mask);

    assert(sigaction(sig, &sa, nullptr) != -1);
}

int main(int argc, char* argv[]) {
    if(argc <= 2) {
        std::cout << "usage: " << basename(argv[0]) << " ip_address port_number\n";
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
    socklen_t client_address_length = sizeof(client);
    connection_fd = accept(sock, (struct sockaddr*)&client, &client_address_length);

    if(connection_fd <= 0) {
        std::cout << "errno is: " << errno << "\n";
    } else {
        add_sig(SIGURG, sig_urg);
        fcntl(connection_fd, F_SETOWN, getpid());

        char buffer[BUF_SIZE];

        while(true) {
            memset(buffer, 0, sizeof(buffer));
            ret = recv(connection_fd, buffer, BUF_SIZE - 1, 0);
            if(ret <= 0) {
                break;
            }
            std::cout << "got " << ret << " bytes of normal data " << buffer << "\n";
        }
        close(connection_fd);
    }
    close(sock);
}