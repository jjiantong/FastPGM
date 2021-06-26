//
// Created by jjt on 2021/6/23.
//

#ifndef BAYESIANNETWORK_INDEPENDENCETEST_H
#define BAYESIANNETWORK_INDEPENDENCETEST_H

#include <set>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <ctime>


using namespace std;

/**
 * Conditional Independence Test (CI test): checks the conditional independence X _||_ Y | S
 * S is a set of discrete variable; X and Y are discrete variable not in S
 * available tests include:
 */
class IndependenceTest {
public:
    /**
     * Stores a map from pairs of nodes (key) to separating sets (value) --
     * for each unordered pair of nodes {node1, node2} in a graph,
     * stores a set of nodes which node1 and node2 are independent conditional on
     * or stores null if the pair are independent conditional on the empty set
     */
    map<pair<int,int>, set<int>> sepset;

    bool IsIndependent(int x_idx, int y_idx, set<int> s, string metric);
};

#endif //BAYESIANNETWORK_INDEPENDENCETEST_H
