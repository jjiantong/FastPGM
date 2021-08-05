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
#include "Dataset.h"
#include "CellTable.h"
#include "CombinationGenerator.h"
#include "Timer.h"

#include "gcem.hpp"
#include "stats.hpp"

using namespace std;

/**
 * Conditional Independence Test (CI test): checks the conditional independence X _||_ Y | S
 * S is a set of discrete variable; X and Y are discrete variable not in S
 * available tests include:
 *      G Square
 */
class IndependenceTest {
public:
    Dataset *dataset;
    double alpha;
    Timer *timer;
    /**
     * note that "dims" is not equal to "cell_table->dims":
     * "dims" contains the dimensions of all variables in the data set
     * while "cell_table->dims" contains the dimensions of a subset of variables (related to the cell_table)
     */
    vector<int> dims;
    CellTable *cell_table;
    /**
     * Stores a map from pairs of nodes (key) to separating sets (value) --
     * for each unordered pair of nodes {node1, node2} in a graph,
     * stores a set of nodes which node1 and node2 are independent conditional on
     * or stores null if the pair are independent conditional on the empty set
     */
    map<pair<int, int>, set<int>> sepset;

    /**
     * an inner class Result is used to store the parameters of the result returned by the G Square test
     */
    class Result {
    public:
        double g_square; // the g square value itself
        double p_value; // the p value of the result
        int df; // the adjusted degrees of freedom
        bool is_independent; // whether the conditional independence holds or not
        // constructs a new g square result using the given parameters
        Result(double g_square, double p_value, int df, bool is_dependent) {
            this->g_square = g_square;
            this->p_value = p_value;
            this->df = df;
            this->is_independent = is_dependent;
        }
    };

//    IndependenceTest(){};
    IndependenceTest(Dataset *dataset, double alpha);
    ~IndependenceTest();

    Result IndependenceResult(int x_idx, int y_idx, const set<int> &z, string metric);
    Result ComputeGSquareXYZ(const vector<int> &test_idx);
    Result ComputeGSquareXY(const vector<int> &test_idx);

//    Result IndependenceResult(int x_idx, int y_idx, const set<int> &z, string metric);
//    Result ComputeGSquare(const vector<int> &test_idx);

//    Result ComputeGSquare(int* test_idx, int size);
//    vector<int> Common(const vector<int> &subset, int index, int value);
};

#endif //BAYESIANNETWORK_INDEPENDENCETEST_H
