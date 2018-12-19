//
// Created by Reckful on 12/19/2018.
//

#ifndef HIGH_PERFORMANCE_LINUX_SERVER_PROGRAMMING_THREAD_POOL_H
#define HIGH_PERFORMANCE_LINUX_SERVER_PROGRAMMING_THREAD_POOL_H

#include <list>
#include <exception>
#include <pthread.h>
#include <locker.h>
#include <iostream>

template <class T>
class thread_pool {
public:
    explicit thread_pool(int thread_number = 8, int max_requests = 10000);
    ~thread_pool();
    bool append(T* request);
private:
    static void* worker(void* arg);
    void run();
private:
    int m_thread_number;
    int m_max_request;
    pthread_t* m_threads;
    std::list<T*> m_work_queue;
    locker m_queue_locker;
    sem m_queue_stat;
    bool m_stop;
};

#endif //HIGH_PERFORMANCE_LINUX_SERVER_PROGRAMMING_THREAD_POOL_H
