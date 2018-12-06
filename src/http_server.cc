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

static const char* status_line[2] = {
        "200 OK",
        "500 Internal server error"
};

int main(int argc, char* argv[]) {
    if(argc <= 3) {
        std::cout << "usage : " << argv[0] << " ip_address port_number filename\n";
        return 1;
    }

    const char* ip = argv[1];
    int port = atoi(argv[2]);
    const char* file_name = argv[3];
    std::string file_path = "data/http_server_data/" + std::string(file_name);

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
        char* header_buf = nullptr;
        char* file_buf = nullptr;
        struct stat file_stat{};
        bool valid;
        int len = 0;

        if(stat(file_path.c_str(), &file_stat) < 0) {
            valid = false;
        } else {
            if(S_ISDIR(file_stat.st_mode)) {
                valid = false;
            } else if(file_stat.st_mode & S_IROTH) {
                int fd = open(file_path.c_str(), O_RDONLY);
                file_buf = new char[file_stat.st_size + 1];
                memset(file_buf, 0, static_cast<size_t>(file_stat.st_size + 1));

                valid = read(fd, file_buf, static_cast<size_t>(file_stat.st_size)) >= 0;
            } else {
                valid = false;
            }
        }

        if(valid) {
            header_buf = new char[BUF_SIZE]();

            ret = snprintf(header_buf, BUF_SIZE - 1, "%s %s\r\n", "HTTP/1.1", status_line[0]);
            len += ret;

            ret = snprintf(header_buf + len, static_cast<size_t>(BUF_SIZE - len - 1), "Content-Length: %d\r\n",
                           static_cast<int>(file_stat.st_size));
            len += ret;

            snprintf(header_buf + len, static_cast<size_t>(BUF_SIZE - len - 1), "%s", "\r\n");
            struct iovec iv[2];
            iv[0].iov_base = header_buf;
            iv[0].iov_len = strlen(header_buf);
            iv[1].iov_base = file_buf;
            iv[1].iov_len = static_cast<size_t>(file_stat.st_size);

            writev(conn_fd, iv, 2);
        } else {
            ret = snprintf(header_buf, BUF_SIZE - 1, "%s %s\r\n", "HTTP/1.1", status_line[1]);
            len += ret;

            snprintf(header_buf + len, static_cast<size_t>(BUF_SIZE - len - 1), "%s", "\r\n");
            send(conn_fd, header_buf, sizeof(header_buf), 0);
        }

        close(conn_fd);

        delete []file_buf;
        delete []header_buf;
    }

    close(sock);
    return 0;
}