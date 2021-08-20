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
    CellTable *cell_table;

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

    IndependenceTest(Dataset *dataset, double alpha);
    ~IndependenceTest();

    /**----------------------------- implementations like bnlearn -----------------------------**/
    Result IndependenceResult(int x_idx, int y_idx, const set<int> &z, string metric, Timer *timer);
    Result ComputeGSquareXYZ(const vector<int> &test_idx, Timer *timer);
    Result ComputeGSquareXY(const vector<int> &test_idx, Timer *timer);
    /**----------------------------- implementations like bnlearn -----------------------------**/

    /**----------------------------- implementations like Tetrad -----------------------------**/
//    Result IndependenceResult(int x_idx, int y_idx, const set<int> &z, string metric);
//    Result ComputeGSquare(const vector<int> &test_idx);

//    Result ComputeGSquare(int* test_idx, int size);
//    vector<int> Common(const vector<int> &subset, int index, int value);
    /**----------------------------- implementations like Tetrad -----------------------------**/
};

#endif //BAYESIANNETWORK_INDEPENDENCETEST_H
