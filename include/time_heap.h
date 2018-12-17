//
// Created by Reckful on 12/17/2018.
//

#ifndef HIGH_PERFORMANCE_LINUX_SERVER_PROGRAMMING_TIME_HEAP_H
#define HIGH_PERFORMANCE_LINUX_SERVER_PROGRAMMING_TIME_HEAP_H

#include <netinet/in.h>
#include <time.h>

#include <iostream>

#define BUFFER_SIZE 64

class heap_timer;

struct client_data {
    sockaddr_in address;
    int sock_fd;
    char buf[BUFFER_SIZE];
    heap_timer* timer;
};

class heap_timer {
public:
    explicit heap_timer(int delay);

public:
    time_t expire;
    void (*cb_func) (client_data*);
    client_data* user_data;
};

class time_heap {
public:
    explicit time_heap(int cap) noexcept(false);
    time_heap(heap_timer** init_array, int size, int cap) noexcept(false);
    ~time_heap();

    void add_timer(heap_timer* timer) noexcept(false);

    void delete_timer(heap_timer* timer);
    heap_timer* top() const;
    void pop();
    void tick();
    bool empty() const;

private:
    void percolate_down(int hole);
    void resize() noexcept(false);
private:
    heap_timer** array;
    int capacity;
    int cur_size;
};


#endif //HIGH_PERFORMANCE_LINUX_SERVER_PROGRAMMING_TIME_HEAP_H
