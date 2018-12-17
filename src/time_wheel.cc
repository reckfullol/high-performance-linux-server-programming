//
// Created by Reckful on 12/16/2018.
//

#include <time_wheel.h>

time_wheel::time_wheel() : cur_slot(0) {
    for (auto& slot : slots) {
        slot = nullptr;
    }
}

time_wheel::~time_wheel() {
    for (auto& slot : slots) {
        tw_timer* tmp = slot;
        while(tmp != nullptr) {
            slot = tmp->next;
            delete tmp;
            tmp = slot;
        }
    }
}

tw_timer *time_wheel::add_timer(int timeout) {
    if(timeout < 0) {
        return nullptr;
    }

    int ticks = 0;

    if(timeout < SI) {
        ticks = 1;
    } else {
        ticks = timeout / SI;
    };

    int rotation = ticks / N;
    int ts = (cur_slot + (ticks % N)) % N;

    tw_timer* timer = new tw_timer(rotation, ts);
    if(slots[ts] == nullptr) {
        std::cout << "add timer, rotation is " << rotation << ", ts is " << ts << ", cur_slot is " << cur_slot << "\n";
        slots[ts] = timer;
    } else {
        timer->next = slots[ts];
        slots[ts]->prev = timer;
        slots[ts] = timer;
    }
    return timer;
}

void time_wheel::delete_timer(tw_timer* timer) {
    if(timer == nullptr) {
        return ;
    }
    int ts = timer->time_slot;

    if(timer == slots[ts]) {
        slots[ts] = slots[ts]->next;
        if(slots[ts] != nullptr) {
            slots[ts]->prev = nullptr;
        }
        delete timer;
    } else {
        timer->prev->next = timer->next;
        if(timer->next != nullptr) {
            timer->next->prev = timer->prev;
        }
        delete timer;
    }
}

void time_wheel::tick() {
    tw_timer* tmp = slots[cur_slot];
    std::cout << "current slot is " << cur_slot <<"\n";
    while(tmp != nullptr) {
        std::cout << "tick the timer once\n";
        if(tmp->rotation > 0) {
            tmp->rotation--;
            tmp = tmp->next;
        } else {
            tmp->cb_func(tmp->user_data);
            if(tmp == slots[cur_slot]) {
                std::cout << "delete header in cur_slot\n";
                slots[cur_slot] = tmp->next;
                delete tmp;
                if(slots[cur_slot] != nullptr) {
                    slots[cur_slot]->prev = nullptr;
                }
                tmp = slots[cur_slot];
            } else {
                tmp->prev->next = tmp->next;
                if(tmp->next != nullptr) {
                    tmp->next->prev = tmp->prev;
                }
                tw_timer* tmp2 = tmp->next;
                delete tmp;
                tmp = tmp2;
            }
        }
    }
    cur_slot = (cur_slot + 1) % N;
}
