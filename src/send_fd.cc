//
// Created by Reckful on 12/19/2018.
//

#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <iostream>

static const int CONTROL_LEN = CMSG_LEN(sizeof(int));

void send_fd(int fd, int fd_to_send) {
    struct iovec iov[1];
    struct msghdr msg{};
    char buf[0];

    iov[0].iov_base = buf;
    iov[0].iov_len = 1;

    msg.msg_name = nullptr;
    msg.msg_namelen = 0;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;

    cmsghdr cm{};
    cm.cmsg_len = (size_t) CONTROL_LEN;
    cm.cmsg_level = SOL_SOCKET;
    cm.cmsg_type = SCM_RIGHTS;
    *(int*)CMSG_DATA(&cm) = fd_to_send;

    msg.msg_control = &cm;
    msg.msg_controllen = (size_t) CONTROL_LEN;

    sendmsg(fd, &msg, 0);
}

int recv_fd(int fd) {
    struct iovec iov[1];
    struct msghdr msg{};
    char buf[0];

    iov[0].iov_base = buf;
    iov[0].iov_len = 1;

    msg.msg_name = nullptr;
    msg.msg_namelen = 0;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;

    cmsghdr cm{};
    msg.msg_control = &cm;
    msg.msg_controllen = (size_t) CONTROL_LEN;

    recvmsg(fd, &msg, 0);

    int fd_to_read = *(int*)CMSG_DATA(&cm);
    return fd_to_read;
}

int main(int argc, char* argv[]) {
    int pipe_fd[2];
    int fd_to_pass = 0;

    int ret = socketpair(PF_UNIX, SOCK_DGRAM, 0, pipe_fd);
    assert(ret != -1);

    pid_t pid = fork();
    assert(pid >= 0);

    if(pid == 0) {
        close(pipe_fd[0]);
        fd_to_pass = open("data/ftp_server_data/fuck.txt", O_RDWR, 0666);

        send_fd(pipe_fd[1], fd_to_pass > 0 ? fd_to_pass : 0);
        close(fd_to_pass);
        exit(0);
    }

    close(pipe_fd[0]);
    fd_to_pass = recv_fd(pipe_fd[0]);

    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    read(fd_to_pass, buffer, sizeof(buffer));

    std::cout << "I got fd " << fd_to_pass <<" and data " << buffer << std::endl;

    close(fd_to_pass);

    return 0;
}