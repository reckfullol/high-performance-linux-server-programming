//
// Created by Reckful on 12/19/2018.
//

#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <cstdio>
#include <iostream>

#define handle_error_en(en, msg) \
    do { errno = en; perror(msg); exit(EXIT_FAILURE); } while(0)

static void* sig_thread(void* arg) {
    sigset_t* set = (sigset_t*) arg;
    int s, sig;

    while(true) {
        s = sigwait(set, &sig);
        if(s != 0) {
            handle_error_en(s, "sigwait");
        }
        std::cout << "Signal handling thread gto singnal " << sig << "\n";
    }
}

int main(int argc, char* argv[]) {
    pthread_t thread;
    sigset_t set;
    int s;

    sigemptyset(&set);
    sigaddset(&set, SIGQUIT);
    sigaddset(&set, SIGUSR1);

    s = pthread_sigmask(SIG_BLOCK, &set, nullptr);
    if(s != 0) {
        handle_error_en(s, "pthread_sigmask");
    }

    s = pthread_create(&thread, nullptr, &sig_thread, (void*)&set);
    if(s != 0) {
        handle_error_en(s, "pthread_create");
    }

    pause();
}