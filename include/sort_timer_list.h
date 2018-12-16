//
// Created by Reckful on 12/16/2018.
//

#ifndef HIGH_PERFORMANCE_LINUX_SERVER_PROGRAMMING_SORT_TIMER_LIST_H
#define HIGH_PERFORMANCE_LINUX_SERVER_PROGRAMMING_SORT_TIMER_LIST_H

#include <time.h>
#include <netinet/in.h>
#include <iostream>

#define BUFFER_SIZE 64

class util_timer;

struct client_data {
    sockaddr_in address;
    int sock_fd;
    char buf[BUFFER_SIZE];
    util_timer* timer;
};

class util_timer {
public:
    util_timer() : prev(nullptr), next(nullptr) {};

public:
    time_t expire{};
    void(*cb_func)(client_data*){};
    client_data* user_data{};
    util_timer* prev;
    util_timer* next;
};

class sort_timer_list {

private:

    void add_timer(util_timer* timer, util_timer* list_head);

public:

    sort_timer_list() : head(nullptr), tail(nullptr) {};

    ~sort_timer_list();

    void add_timer(util_timer* timer);


    void adjust_timer(util_timer* timer);

    void delete_timer(util_timer* timer);

    void tick();

private:
    util_timer* head;
    util_timer* tail;

};

#endif //HIGH_PERFORMANCE_LINUX_SERVER_PROGRAMMING_SORT_TIMER_LIST_H
