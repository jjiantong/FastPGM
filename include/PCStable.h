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
#include <stack>

bool CmpByValue(const pair<int, int> &left, const pair<int, int> &right) {
    return left.second < right.second;
}

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
    int num_ci_test;
    int num_dependence_judgement;
    double alpha;
    Timer *timer;
    /**
     * Stores a map from pairs of nodes (key) to separating sets (value) --
     * for each unordered pair of nodes {node1, node2} in a graph,
     * stores a set of nodes which node1 and node2 are independent conditional on
     * or stores null if the pair are independent conditional on the empty set
     */
    map<pair<int, int>, set<int>> sepset;

    PCStable(Network *net, double a, int d = 1000);
    ~PCStable();

    virtual void StructLearnCompData(Dataset *dts, int group_size, int num_threads, bool print_struct, bool verbose);
    void StructLearnByPCStable(Dataset *dts, int num_threads, int group_size, bool print_struct, bool verbose);
    bool SearchAtDepth(Dataset *dts, int c_depth, int num_threads, int group_size, bool verbose);

    bool CheckEdge(Dataset *dts, const map<int, map<int, double>> &adjacencies, int c_depth,
                   int edge_id, int group_size, bool verbose);
    int FindAdjacencies(Dataset *dts, const map<int, map<int, double>> &adjacencies, int edge_id, int x_idx, int y_idx);
    bool Testing(Dataset *dts, int c_depth, int edge_id, int x_idx, int y_idx, int group_size, bool verbose);
    int FreeDegree(const map<int, map<int, double>> &adjacencies);

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
