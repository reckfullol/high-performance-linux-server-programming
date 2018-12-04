//
// Created by Reckful on 2018/12/4.
//

#include <iostream>

void ByteOrder() {
    union {
        short value;
        char union_bytes[sizeof(short)];
    } byte_test{};

    byte_test.value = 0x0102;
    if(byte_test.union_bytes[0] == 1 && byte_test.union_bytes[1] == 2) {
        std::cout << "Big endian\n";
    } else if(byte_test.union_bytes[0] == 2 && byte_test.union_bytes[1] == 1){
        std::cout << "Little endian\n";
    } else {
        std::cout << "Unknown endian\n";
    }
}

int main() {
    ByteOrder();
    return 0;
}