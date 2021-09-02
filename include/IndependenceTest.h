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

    Counts2D *table_2d;
    Counts3D *table_3d;
    Counts3DGroup *table_3d_group;

    /**
     * an inner class Result is used to store the parameters of the result returned by the G Square test
     */
    class Result {
    public:
        double p_value; // the p value of the result
        bool is_independent; // whether the conditional independence holds or not
        int first; // the first independent one, used only for computing a group of ci tests (in parallel)
        // constructs a new g square result using the given parameters
        Result(double p_value, bool is_dependent, int first = -1) {
            this->p_value = p_value;
            this->is_independent = is_dependent;
            this->first = first;
        }
    };

    IndependenceTest(Dataset *dataset, double alpha);
    ~IndependenceTest();

    /**----------------------------- implementations like bnlearn -----------------------------**/
    Result IndependenceResult(int x_idx, int y_idx, const vector<int> &z, string metric, Timer *timer,
                              int group_size = 1);

    Result ComputeGSquareXYZ(int x_idx, int y_idx, const vector<int> &z, Timer *timer);
    Result ComputeGSquareXYZGroup(int x_idx, int y_idx, const vector<int> &z, int group_size, Timer *timer);
    Result ComputeGSquareXY(int x_idx, int y_idx, Timer *timer);
    /**----------------------------- implementations like bnlearn -----------------------------**/

    /**----------------------------- implementations like Tetrad -----------------------------**/
//    Result IndependenceResult(int x_idx, int y_idx, const set<int> &z, string metric);
//    Result ComputeGSquare(const vector<int> &test_idx);

//    Result ComputeGSquare(int* test_idx, int size);
//    vector<int> Common(const vector<int> &subset, int index, int value);
    /**----------------------------- implementations like Tetrad -----------------------------**/
};

#endif //BAYESIANNETWORK_INDEPENDENCETEST_H
