//
// Created by Reckful on 12/16/2018.
//
#include <sort_timer_list.h>

sort_timer_list::~sort_timer_list() {
    util_timer* tmp = head;
    while(tmp != nullptr) {
        head = tmp->next;
        delete tmp;
        tmp = head;
    }
}

void sort_timer_list::add_timer(util_timer* timer, util_timer* list_head) {
    util_timer* prev = list_head;
    util_timer* tmp = prev->next;

    while(tmp != nullptr) {
        if(timer->expire < tmp->expire) {
            prev->next = timer;
            timer->next = tmp;
            tmp->prev = timer;
            timer->prev = prev;
            break;
        }
        prev = tmp;
        tmp = tmp->next;
    }

    if(tmp == nullptr) {
        prev->next = timer;
        timer->prev = prev;
        timer->next = nullptr;
        tail = timer;
    }
}

void sort_timer_list::add_timer(util_timer *timer) {
    if(timer == nullptr) {
        return ;
    }
    if(head == nullptr) {
        head = tail = timer;
        return ;
    }

    if(timer->expire < head->expire) {
        timer->next = head;
        head->prev = timer;
        head = timer;
        return ;
    }

    add_timer(timer, head);
}

void sort_timer_list::adjust_timer(util_timer *timer)  {
    if(timer == nullptr) {
        return ;
    }

    util_timer* tmp = timer->next;
    if(tmp == nullptr || timer->expire < tmp->expire) {
        return ;
    }

    if(timer == head) {
        head = head->next;
        head->prev = nullptr;
        timer->next = nullptr;
        add_timer(timer, head);
    } else {
        timer->prev->next = timer->next;
        timer->next->prev = timer->prev;
        add_timer(timer, timer->next);
    }
}

void sort_timer_list::delete_timer(util_timer *timer) {
    if(timer == nullptr) {
        return ;
    }

    if(timer == head && timer == tail) {
        delete timer;
        head = nullptr;
        tail = nullptr;
        return ;
    }

    if(timer == head) {
        head = head->next;
        head->prev = nullptr;
        delete timer;
        return ;
    }

    if(timer == tail) {
        tail = tail->prev;
        tail->next = nullptr;
        delete timer;
    }

    timer->prev->next = timer->next;
    timer->next->prev = timer->prev;
    delete timer;
}

void sort_timer_list::tick() {
    if(head == nullptr) {
        return ;
    }

    std::cout << "timer tick\n" << std::endl;

    time_t cur = time(nullptr);
    util_timer* tmp = head;

    while(tmp != nullptr) {
        if(cur < tmp->expire) {
            break;
        }
        tmp->cb_func(tmp->user_data);
        head = tmp->next;
        if(head != nullptr) {
            head->prev = nullptr;
        }
        delete tmp;
        tmp = head;
    }
}