//
// Created by jjt on 2021/6/17.
//

#include "K2.h"

/**
 * @brief: learning network structure using Weka's algorithm
 * https://github.com/Waikato/weka-3.8
 *      under: weka/src/main/java/weka/classifiers/bayes/net/search/global/K2.java
 *      or     weka/src/main/java/weka/classifiers/bayes/net/search/local/K2.java
 *
 * @key idea: add a parent to a node based on topo_ord_constraint
 *            if adding the node as a parent results in increment of the score
 *            -- there are different scoring functions.
 */
void K2::StructLearnByK2Weka(Dataset *dts, vector<int> topo_ord_constraint, int max_num_parents) { //checked
    // todo: test the correctness
    // if "topo_ord_constraint" = "best"; no order is provided (i.e. no constraint)
    if (topo_ord_constraint.empty() || topo_ord_constraint.size() != network->num_nodes) {
        topo_ord_constraint.reserve(network->num_nodes);
        for (int i = 0; i < network->num_nodes; ++i) {
            topo_ord_constraint.push_back(i); // provide a order constraint: 1, 2, ..., num_nodes-1
        }
    }
    network->GenDiscParCombsForAllNodes(); // generate all possible parent configurations ("set_discrete_parents_combinations") for all nodes

//#pragma omp parallel for
    for (int i = 0; i < network->num_nodes; ++i) {

        int var_index = topo_ord_constraint.at(i);
        DiscreteNode *node = (DiscreteNode*)network->map_idx_node_ptr.at(var_index); // TODO: function "FindNodePtrByIndex"？

        bool ok_to_proceed = (node->GetNumParents() < max_num_parents);
        while (ok_to_proceed) {
            int best_par_index = -1;
            double best_extra_score = 0;

            for (int j = 0; j < i; ++j) { // j < i: traverse all pre nodes, satisfy the ordering constraint
                int par_index = topo_ord_constraint.at(j);

                // TODO: this function calculates old score and new score and returns the delta score as "extra_score"
                // TODO: why not
                // TODO:      1. directly compute the new score and compare the i scores to find the maximum, rather than compare the delta scores
                // TODO:      2. compute the old score before calling this function, because the i old scores seem to be the same
                double extra_score = network->CalcuExtraScoreWithModifiedEdge(par_index, var_index, dts, "add", "log K2");//use K2 as scoring function
                if (extra_score > best_extra_score) { // find the max g() as the "best_extra_score" iteratively
                    // TODO: may not need to addarc and deletearc
                    // TODO: because if "AddArc" function returns false, then current "extra_score" equals to 0 and cannot be the "best_extra_score"
                    if (network->AddEdge(par_index, var_index)) {
                        best_par_index = j;
                        best_extra_score = extra_score;
                        network->DeleteEdge(par_index, var_index);
                    }
                }
            }
            // if the maximum new score is less than or equal to the old score, stop
            if (best_par_index == -1) {
                ok_to_proceed = false;
            }
            // if the maximum new score is better than the old score, add this arc and continue
            else {
                network->AddEdge(best_par_index, var_index);
                ok_to_proceed = (node->GetNumParents() < max_num_parents);
            }
        }
    }
}

void K2::StructLearnCompData(Dataset *dts, bool print_struct, string topo_ord_constraint, int max_num_parents) {

    cout << "==================================================" << '\n'
         << "Begin structural learning with complete data using K2 algorithm...." << endl;

    struct timeval start, end;
    double diff;
    gettimeofday(&start,NULL);

    AssignNodeInformation(dts);
    vector<int> ord = AssignNodeOrder(topo_ord_constraint);

    StructLearnByK2Weka(dts, ord, max_num_parents);

    cout << "==================================================" << '\n'
         << "Finish structural learning." << endl;

    gettimeofday(&end,NULL);
    diff = (end.tv_sec-start.tv_sec) + ((double)(end.tv_usec-start.tv_usec))/1.0E6;
    setlocale(LC_NUMERIC, "");//formatting the output
    cout << "==================================================" << '\n'
         << "The time spent to learn the structure is " << diff << " seconds" << endl;

    if (print_struct) {

        cout << "==================================================" << '\n'
             << "Topological sorted permutation generated using width-first-traversal: " << endl;
        auto topo = network->GetTopoOrd();
        for (int m = 0; m < network->num_nodes; ++m) {
            cout << topo.at(m) << ", ";
        }
        cout << endl;

        cout << "==================================================" << '\n'
             << "Each node's parents: " << endl;
        network->PrintEachNodeParents();
    }
}