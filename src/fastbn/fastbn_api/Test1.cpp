//
// Created by jjt on 17/03/24.
//
#include "fastbn/fastbn_api/Test1.h"

Test1::Test1(int a, int b) {
    this->a = a;
    this->b = b;
}

int Test1::add1(int a, int b) {
    return a + b;
}

int Test1::add2(int c) {
    return a + b + c;
}