//
// Created by Reckful on 2018/12/6.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/uio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include <iostream>

#define BUF_SIZE 1024


int main(int argc, char* argv[]) {
    if(argc <= 3) {
        std::cout << "usage : " << argv[0] << " ip_address port_number filename\n";
        return 1;
    }

    const char* ip = argv[1];
    int port = atoi(argv[2]);
    const char* file_name = argv[3];
    std::string file_path = "data/ftp_server_data/" + std::string(file_name);

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
        int file_fd = open(file_path.c_str(), O_RDONLY);
        struct stat stat_buf{};
        fstat(file_fd, &stat_buf);
        sendfile(file_fd, conn_fd, 0, &stat_buf.st_size, nullptr, 0);
        close(conn_fd);
    }

    close(sock);
    return 0;
}