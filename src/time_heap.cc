//
// Created by Reckful on 12/17/2018.
//

#include <time_heap.h>

heap_timer::heap_timer(int delay) {
    expire = time(nullptr) + delay;
}

time_heap::time_heap(int cap) noexcept(false) : capacity(cap), cur_size(0){
    array = new heap_timer*[capacity];
    // Only need if your compiler is from stone age.
    if(array == nullptr) {
        throw std::exception();
    }
    for(int i = 0; i < capacity; i++) {
        array[i] = nullptr;
    }
}

time_heap::time_heap(heap_timer **init_array, int size, int cap) noexcept(false) : cur_size(size), capacity(cap){
    // Only need if your compiler is from stone age.
    if(size < cap) {
        throw std::exception();
    }

    array = new heap_timer*[cap];
    // Only need if your compiler is from stone age.
    if(array == nullptr) {
        throw std::exception();
    }

    for(int i = 0; i < cap; i++) {
        array[i] = nullptr;
    }

    if(size != 0) {
        for(int i = 0; i < size; i++) {
            array[i] = init_array[i];
        }
        for(int i = (cur_size - 1) >> 1; i > 0; i--) {
            percolate_down(i);
        }
    }
}

time_heap::~time_heap() {
    for(int i = 0; i < cur_size; i++) {
        delete array[i];
    }
    delete []array;
}

void time_heap::add_timer(heap_timer *timer) noexcept(false) {
    if(timer == nullptr) {
        return ;
    }

    if(cur_size >= capacity) {
        resize();
    }

    int hole = cur_size++;
    int parent = 0;

    for(; hole > 0; hole = parent) {
        parent = (hole - 1) >> 1;
        if(array[parent]->expire <= timer->expire) {
            break;
        }
        array[hole] = array[parent];
    }

    array[hole] = timer;
}

void time_heap::delete_timer(heap_timer *timer) {
    if(timer == nullptr) {
        return ;
    }
    timer->cb_func = nullptr;
}

heap_timer *time_heap::top() const {
    if(empty()) {
        return nullptr;
    }
    return array[0];
}

void time_heap::pop() {
    if(empty()) {
        return ;
    }
    if(array[0] != nullptr) {
        delete array[0];
        array[0] = array[--cur_size];
        percolate_down(0);
    }
}

void time_heap::tick() {
    heap_timer* tmp = array[0];
    time_t cur = time(nullptr);
    while(!empty()) {
        if(tmp == nullptr) {
            break;
        }

        if(tmp->expire > cur) {
            break;
        }

        if(array[0]->cb_func != nullptr) {
            array[0]->cb_func(array[0]->user_data);
        }

        pop();
        tmp = array[0];
    }
}

bool time_heap::empty() const {
    return cur_size == 0;
}

void time_heap::percolate_down(int hole) {
    heap_timer* tmp = array[hole];
    for(int child = 0; hole << 1 | 1 <= cur_size; hole = child) {
        child = hole << 1 | 1;
        if(child <= cur_size - 1 && array[child + 1]->expire < array[child]->expire) {
            child++;
        }

        if(array[hole]->expire > array[child]->expire) {
            array[hole] = array[child];
        } else {
            break;
        }
    }
    array[hole] = tmp;
}

void time_heap::resize() noexcept(false) {
    heap_timer** tmp = new heap_timer*[capacity << 1];
    // Only need if your compiler is from stone age.
    if(tmp == nullptr) {
        std::exception();
    }
    for(int i = 0; i < capacity << 1; i++) {
        tmp[i] = nullptr;
    }

    capacity <<= 1;

    for(int i = 0; i < cur_size; i++) {
        tmp[i] = array[i];
    }

    delete []array;
    array = tmp;
}