//
// Created by Reckful on 2018/12/7.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <strings.h>
#include <iostream>

#define BUFFER_SIZE 4096

enum CHECK_STATE {
    CHECK_STATE_REQUEST_LINE = 0,
    CHECK_STATE_HEADER
};

enum LINE_STATUS {
    LINE_OK = 0,
    LINE_BAD,
    LINE_OPEN
};

enum HTTP_CODE {
    NO_REQUEST,
    GET_REQUEST,
    BAD_REQUEST,
    FORBIDDEN_REQUEST,
    INTERNAL_ERROR,
    CLOSED_CONNECTION
};

static const char* szret[] = {
        "I get a correct result\n",
        "Something wrong\n"
};

LINE_STATUS parse_line(char* buffer, int& checked_index, int& read_index) {
    char temp;
    for(; checked_index < read_index; checked_index++) {
        temp = buffer[checked_index];
        if(temp == '\r') {
            if(checked_index + 1 == read_index) {
                return LINE_OPEN;
            } else if(buffer[checked_index + 1] == '\n') {
                buffer[checked_index++] = '\0';
                buffer[checked_index++] = '\0';
                return LINE_OK;
            }
            return LINE_BAD;
        } else if(temp == '\n') {
            if(checked_index > 1 && buffer[checked_index - 1] == '\r') {
                buffer[checked_index - 1] = '\0';
                buffer[checked_index++] = '\0';
                return LINE_OK;
            }
            return LINE_BAD;
        }
    }
    return LINE_OPEN;
}

HTTP_CODE parse_request_line(char* temp, CHECK_STATE& check_state) {
    char* url = strpbrk(temp, " \t");
    if(!url) {
        return BAD_REQUEST;
    }
    *url++ = '\0';

    char* method = temp;
    if(strcasecmp(method, "GET") == 0) {
        std::cout << "The request method is GET\n";
    } else {
        return BAD_REQUEST;
    }

    url += strspn(url, " \t");

    char* version = strpbrk(url, " \t");
    if(!version) {
        return BAD_REQUEST;
    }
    *version++ = '\0';
    version += strspn(version, " \t");

    if(strcasecmp(version, "HTTP/1.1") != 0) {
        return BAD_REQUEST;
    }

    if(strncasecmp(url, "http://", 7) == 0) {
        url += 7;
        url = strchr(url, '/');
    }

    if(!url || url[0] != '/') {
        return BAD_REQUEST;
    }

    std::cout << "The request URL is: " << url << "\n";

    check_state = CHECK_STATE_HEADER;
    return NO_REQUEST;
}

HTTP_CODE parse_headers(char* temp) {
    if(temp[0] == '\0') {
        return GET_REQUEST;
    } else if(strncasecmp(temp, "Host:", 5) == 0) {
        temp += 5;
        temp += strspn(temp, " \t");
        std::cout << "the request host is: " << temp << "\n";
    } else {
        std::cout << "I can not handle this header\n";
    }
    return NO_REQUEST;
}

HTTP_CODE parse_content(char* buffer, int& checked_index, CHECK_STATE& check_state, int& read_index, int& start_line) {
    LINE_STATUS line_status = LINE_OK;
    HTTP_CODE ret_code = NO_REQUEST;

    while((line_status = parse_line(buffer, checked_index, read_index)) == LINE_OK) {
       char* temp = buffer + start_line;
       start_line = checked_index;

       switch(check_state) {
            case CHECK_STATE_REQUEST_LINE: {
               ret_code = parse_request_line(temp, check_state);
               if(ret_code == BAD_REQUEST) {
                   return BAD_REQUEST;
               }
               break;
            }
            case CHECK_STATE_HEADER: {
                ret_code = parse_headers(temp);
                if(ret_code == BAD_REQUEST) {
                    return BAD_REQUEST;
                } else if(ret_code == GET_REQUEST) {
                    return GET_REQUEST;
                }
                break;
            }
            default: {
                return INTERNAL_ERROR;
            }
       }
    }

    if(line_status == LINE_OPEN) {
        return NO_REQUEST;
    } else {
        return BAD_REQUEST;
    }
}


int main(int argc, char* argv[]) {
    if(argc <= 2) {
        std::cout << "usage " << argv[0] << " ip_address port_number\n";
        return 1;
    }

    const char* ip = argv[1];
    int port = atoi(argv[2]);

    struct sockaddr_in address{};
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);

    int listen_fd = socket(PF_INET, SOCK_STREAM, 0);
    assert(listen_fd >= 0);

    int ret = bind(listen_fd, (struct sockaddr*)&address, sizeof(address));
    assert(ret != -1);

    ret = listen(listen_fd, 5);
    assert(ret != -1);

    struct sockaddr_in client_address{};
    socklen_t client_address_length = sizeof(client_address);
    int client_fd = accept(listen_fd, (struct sockaddr*)&client_address, &client_address_length);
    if(client_fd < 0) {
        std::cout << "errno is:" << errno << "\n";
    } else {
        char buffer[BUFFER_SIZE];
        memset(buffer, 0, BUFFER_SIZE);

        int data_read = 0;
        int read_index = 0;
        int checked_index = 0;
        int start_line = 0;

        CHECK_STATE check_state = CHECK_STATE_REQUEST_LINE;
        while(true) {
            data_read = static_cast<int>(recv(client_fd, buffer + read_index,
                                              static_cast<size_t>(BUFFER_SIZE - read_index), 0));
            if(data_read == -1) {
                std::cout << "reading failed\n";
                break;
            } else if(data_read == 0) {
                std::cout << "remote client has closed this connection\n";
            }
            read_index = data_read;

            HTTP_CODE result = parse_content(buffer, checked_index, check_state, read_index, start_line);

            if(result == NO_REQUEST) {
                continue;
            } else if(result == GET_REQUEST) {
                send(client_fd, szret[0], strlen(szret[0]), 0);
                break;
            } else {
                send(client_fd, szret[0], strlen(szret[1]), 0);
                break;
            }
        }
        close(client_fd);
    }
    close(listen_fd);

    return 0;
}