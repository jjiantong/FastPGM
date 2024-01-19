//
// Created by jjt on 2021/6/23.
//

#ifndef BAYESIANNETWORK_PCSTABLE_H
#define BAYESIANNETWORK_PCSTABLE_H

#include "fastbo/common.h"
#include "StructureLearning.h"
#include "fastbo/structure_learning/IndependenceTest.h"
#include "fastbo/ChoiceGenerator.h"
#include "fastbo/Timer.h"
#include <algorithm>
#include <stack>

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
    int num_dependence;
    double alpha;
    /**
     * Stores a map from pairs of nodes (key) to separating sets (value) --
     * for each unordered pair of nodes {node1, node2} in a graph,
     * stores a set of nodes which node1 and node2 are independent conditional on
     * or stores null if the pair are independent conditional on the empty set
     */
    map<pair<int, int>, set<int>> sepset;

    PCStable(Network *net, double a, int d = 1000);
    ~PCStable();

    virtual void StructLearnCompData(Dataset *dts, int group_size, int num_threads,
                                     bool dag, bool save_struct, bool add_root, int verbose);
    void StructLearnByPCStable(Dataset *dts, int num_threads, int group_size, bool dag,
                               Timer *timer, int verbose);
    bool SearchAtDepth(Dataset *dts, int c_depth, int num_threads, map<int, map<int, double>> &adjacencies,
                       Timer *timer, int group_size, int verbose);

    bool CheckEdge(Dataset *dts, const map<int, map<int, double>> &adjacencies, int c_depth,
                   int edge_id, Timer *timer, int group_size, int verbose);
    int FindAdjacencies(const map<int, map<int, double>> &adjacencies, int edge_id, int x_idx, int y_idx);
    bool Testing(Dataset *dts, int c_depth, int edge_id, int x_idx, int y_idx, Timer *timer, int group_size, int verbose);
    int FreeDegree(const map<int, map<int, double>> &adjacencies);

    void OrientVStructure(const map<int, map<int, double>> &adjacencies);
    void OrientImplied(const map<int, map<int, double>> &adjacencies);
    bool Rule1(const map<int, map<int, double>> &adjacencies, int b_idx, int c_idx);
    bool Rule2(const map<int, map<int, double>> &adjacencies, int a_idx, int c_idx);
    bool Rule3(const map<int, map<int, double>> &adjacencies, int d_idx, int a_idx);
    bool R3Helper(const map<int, map<int, double>> &adjacencies, int a_idx, int d_idx, int b_idx, int c_idx);
};

#endif //BAYESIANNETWORK_PCSTABLE_H
