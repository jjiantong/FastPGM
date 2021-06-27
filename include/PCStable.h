//
// Created by jjt on 2021/6/23.
//

#ifndef BAYESIANNETWORK_PCSTABLE_H
#define BAYESIANNETWORK_PCSTABLE_H

#include "StructureLearning.h"
#include "IndependenceTest.h"
#include "ChoiceGenerator.h"

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
    bool stable = true; // PC-Stable or PC
    IndependenceTest* ci_test;
    int num_ci_test;
    int num_dependence_judgement;

    PCStable(Network *net);
    PCStable(Network *net, int d, bool s);

    virtual void StructLearnCompData(Dataset *dts, bool print_struct);
    void StructLearnByPCStable(Dataset *dts, bool print_struct);
    bool SearchAtDepth(int c_depth);
    bool CheckSide(map<int, set<int>> adjacencies, int c_depth, Node* x, Node* y);
    int FreeDegree(map<int, set<int>> adjacencies);

    void OrientVStructure();
};

#endif //BAYESIANNETWORK_PCSTABLE_H
