//
// Created by Reckful on 2018/12/4.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

#include <assert.h>
#include <iostream>

static bool stop_flag = false;

static void HandleTerm(int sig) {
    stop_flag = true;
}

int main(int argc, char* argv[]) {
    std::ios::sync_with_stdio(false);

    signal(SIGTERM, HandleTerm);

    if(argc <= 3) {
        std::cout << "usage: " << argv[0] << "ip address port_number backlog\n";
        return 1;
    }

    const char* ip = argv[1];
    int port = atoi(argv[2]);
    int backlog = atoi(argv[3]);

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    assert(sock >= 0);

    struct sockaddr_in address{};
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);

    int ret = bind(sock, (struct sockaddr*)&address, sizeof(address));
    assert(ret != -1);

    ret = listen(sock, backlog);
    assert(ret != -1);

    while(!stop_flag) {
        sleep(1);
    }

    close(sock);
    return 0;
}