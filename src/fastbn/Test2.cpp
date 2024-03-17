//
// Created by jjt on 17/03/24.
//
#include "fastbn/Test2.h"

Test2::Test2() {
    a = 1;
    b = 2;
}

int Test2::sub1(int a, int b) {
    return a - b;
}

int Test2::sub2(int c) {
    Test1 *t1 = new Test1(3,2);
    return a - b + c + t1->a + t1->b;
}