//
// Created by jjt on 2021/6/23.
//

#include "IndependenceTest.h"


bool IndependenceTest::IsIndependent(int x_idx, int y_idx, set<int> s, string metric) {
    if (metric.compare("g square") == 0) {}
    else if (metric.compare("mutual information") == 0) {}
    else {}

    int random_number = rand()%10000;
    if (random_number > 9990) {
        return true;
    } else {
        return false;
    }
}