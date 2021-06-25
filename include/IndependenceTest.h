//
// Created by jjt on 2021/6/23.
//

#ifndef BAYESIANNETWORK_INDEPENDENCETEST_H
#define BAYESIANNETWORK_INDEPENDENCETEST_H

#include <set>
#include <string>

using namespace std;

class IndependenceTest {

    bool IsIndependent(int x_idx, int y_idx, set<int> s, string metric);
};

#endif //BAYESIANNETWORK_INDEPENDENCETEST_H
