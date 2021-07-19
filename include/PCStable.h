//
// Created by jjt on 2021/6/23.
//

#ifndef BAYESIANNETWORK_PCSTABLE_H
#define BAYESIANNETWORK_PCSTABLE_H

#include "StructureLearning.h"
#include "IndependenceTest.h"
#include "ChoiceGenerator.h"
#include "Timer.h"
#include <algorithm>

/**
 * @brief: Implementation the PC-stable (Peter & Clark) algorithm
 * PC: Chapter 6 of Spirtes, Glymour, and Scheines, "Causation, Prediction, and Search," 2nd edition,
 *     with a modified rule set in step D due to Chris Meek,
 *     see Chris Meek (1995), "Causal inference and causal explanation with background knowledge."
 * PC-stable: Colombo and Maathuis, "Order-independent constraint-based causal structure learning."
 */
class PCStable : public StructureLearning {
public:
    int depth = 1000; // The maximum number of nodes conditioned on in the search. The default it 1000.
    IndependenceTest* ci_test;
    int num_ci_test;
    int num_dependence_judgement;
    Timer timer;

    PCStable(Network *net, Dataset *dataset, double alpha);
    PCStable(Network *net, int d, Dataset *dataset, double alpha);

    virtual void StructLearnCompData(Dataset *dts, bool print_struct);
    void StructLearnByPCStable(bool print_struct);
    bool SearchAtDepth(int c_depth);
    bool CheckSide(const map<int, set<int>> &adjacencies, int c_depth, Node* x, Node* y);
    int FreeDegree(const map<int, set<int>> &adjacencies);

    void OrientVStructure();
    void OrientImplied();
    bool Direct(int a, int c);
    set<int> GetCommonAdjacents(int x_idx, int y_idx);
    bool Rule1(int b_idx, int c_idx);
    bool Rule2(int a_idx, int c_idx);
    bool Rule3(int d_idx, int a_idx);
    bool R3Helper(int a_idx, int d_idx, int b_idx, int c_idx);
};

#endif //BAYESIANNETWORK_PCSTABLE_H
