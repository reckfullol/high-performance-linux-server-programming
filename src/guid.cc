//
// Created by Reckful on 2018/12/7.
//

#include <unistd.h>

#include <iostream>

int main() {
    uid_t uid = getuid();
    uid_t euid = getegid();

    std::cout << "userod is " << uid << ", effective userid is " << euid << "\n";
    return 0;
}
