//
// Created by Reckful on 12/19/2018.
//

#include <thread_pool.h>

template<class T>
thread_pool<T>::thread_pool(int thread_number, int max_requests) noexcept(false) : m_thread_number(thread_number), m_max_request(max_requests), m_stop(
        false), m_threads(nullptr) {
    if(thread_number <= 0 || max_requests <= 0) {
        throw std::exception();
    }

    m_threads = new pthread_t[m_thread_number];
    if(m_threads == nullptr) {
        throw std::exception();
    }

    for(int i = 0; i < m_thread_number; i++) {
        std::cout << "create the " << i << "th thread\n";
        if(pthread_create(m_threads + i, nullptr, worker, this) != 0) {
            delete [] m_threads;
            throw std::exception();
        }

        if(pthread_detach(m_threads[i]) == 0) {
            delete [] m_threads;
            throw std::exception();
        }
    }
}

template<class T>
thread_pool<T>::~thread_pool() {
    delete [] m_threads;
    m_stop = true;
}

template<class T>
bool thread_pool<T>::append(T *request) {
    m_queue_locker.lock();

    if(m_work_queue.size() > m_max_request) {
        m_queue_locker.unlock();
        return false;
    }

    m_work_queue.push_back(request);

    m_queue_locker.unlock();

    m_queue_stat.post();

    return true;
}

template<class T>
void *thread_pool<T>::worker(void *arg) {
    thread_pool* pool = (thread_pool*) arg;
    pool->run();
    return pool;
}

template<class T>
void thread_pool<T>::run() {
    while(m_stop != false) {
        m_queue_stat.wait();
        m_queue_locker.lock();
        if(m_work_queue.empty()) {
            m_queue_locker.unlock();
            continue;
        }

        T* request = m_work_queue.front();
        m_work_queue.pop_front();
        m_queue_locker.unlock();
        if(request == nullptr) {
            continue;
        }
        request->process();
    }
}
