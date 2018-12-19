//
// Created by Reckful on 12/19/2018.
//

#ifndef HIGH_PERFORMANCE_LINUX_SERVER_PROGRAMMING_LOCKER_H
#define HIGH_PERFORMANCE_LINUX_SERVER_PROGRAMMING_LOCKER_H

#include <exception>
#include <pthread.h>
#include <semaphore.h>

class sem {
public:
    sem() noexcept(false);
    ~sem();
    bool wait();
    bool post();

private:
    sem_t m_sem;
};

class locker {
public:
    locker() noexcept(false);
    ~locker();
    bool lock();
    bool unlock();

private:
    pthread_mutex_t m_mutex;
};

class cond {
public:
    cond() noexcept(false);
    ~cond();
    bool wait();
    bool signal();
private:
    pthread_mutex_t m_mutex;
    pthread_cond_t m_cond;
};

#endif //HIGH_PERFORMANCE_LINUX_SERVER_PROGRAMMING_LOCKER_H
