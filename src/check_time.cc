//
// Created by Reckful on 2018/12/5.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <assert.h>

#include <iostream>
#include <arpa/inet.h>

int main(int argc, char* argv[]) {
    assert(argc == 2);

    char* host = argv[1];
    struct hostent* host_info = gethostbyname(host);
    assert(host_info);

    struct servent* serv_info = getservbyname("daytime", "tcp");
    assert(serv_info);

    std::cout << "daytime port is " << ntohs(serv_info->s_port) << '\n';


    struct sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = static_cast<in_port_t>(serv_info->s_port);

    address.sin_addr = *(struct in_addr*)*host_info->h_addr_list;

    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    long ret = connect(sock_fd, (struct sockaddr*)&address, sizeof(address));
    assert(ret != -1);

    char buffer[128];
    ret = read(sock_fd, buffer, sizeof(buffer));
    assert(ret > 0);

    buffer[ret] = '\0';
    std::cout << "the day time is: " << buffer << '\n';
    close(sock_fd);

    return 0;

}