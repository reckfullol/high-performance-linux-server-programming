# only work on linux

if(!${APPLE})

    add_executable(epoll epoll.cc)

    add_executable(splice splice.cc)

    add_executable(epoll_one_shot epoll_one_shot.cc)

endif()

add_executable(check_endian check_endian.cc)

add_executable(backlog_parameter listen_backlog.cc)

add_executable(accept_connection accept_connection.cc)

add_executable(tcp_send tcp_send.cc)

add_executable(tcp_recv tcp_recv.cc)

add_executable(tcp_send_buffer_size tcp_send_buffer_size.cc)

add_executable(tcp_recv_buffer_size tcp_recv_buffer_size.cc)

add_executable(check_time get_server_by_name.cc)

add_executable(cgi_server dup.cc)

add_executable(http_server writev.cc)

add_executable(ftp_server sendfile.cc)

add_executable(guid guid.cc)

add_executable(http_parse http_parse.cc)

add_executable(select select.cc)

add_executable(unblock_connection unblock_connection.cc)