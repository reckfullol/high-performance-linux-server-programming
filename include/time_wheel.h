//
// Created by Reckful on 12/16/2018.
//

#ifndef HIGH_PERFORMANCE_LINUX_SERVER_PROGRAMMING_TW_TIMER_H
#define HIGH_PERFORMANCE_LINUX_SERVER_PROGRAMMING_TW_TIMER_H

#include <time.h>
#include <netinet/in.h>
#include <iostream>

#define BUFFER_SIZE 64

class tw_timer;

struct client_data {
    sockaddr_in address;
    int sock_fd;
    char buf[BUFFER_SIZE];
    tw_timer* timer;
};

class tw_timer {
public:
    int rotation;
    int time_slot;
    void (*cb_func)(client_data*){};
    client_data* user_data{};
    tw_timer* next;
    tw_timer* prev;

public:
    tw_timer(int rot, int ts) : next(nullptr), prev(nullptr), rotation(rot), time_slot(ts) {};
};

class time_wheel {
public:
    time_wheel();
    ~time_wheel();
    tw_timer* add_timer(int timeout);
    void delete_timer(tw_timer* timer);
    void tick();
private:
    static const int N = 60;
    static const int SI = 1;
    tw_timer* slots[N];
    int cur_slot;
};


#endif //HIGH_PERFORMANCE_LINUX_SERVER_PROGRAMMING_TW_TIMER_H
