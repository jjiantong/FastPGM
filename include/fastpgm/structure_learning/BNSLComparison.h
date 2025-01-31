//
// Created by jjt on 2021/7/10.
//

#ifndef BAYESIANNETWORK_BNSLCOMPARISON_H
#define BAYESIANNETWORK_BNSLCOMPARISON_H

#include "fastpgm/Network.h"

/**
 * this class is used to measure the quality of the learned Bayesian Networks
 * available metrics include
 *      Structural Hamming Distance (SHD)
 */
class BNSLComparison {
public:
    Network* true_graph;
    Network* learned_graph;

    BNSLComparison(Network* true_graph, Network* learned_graph);

    int GetSHD();
    int GetSHDOneEdge(int index1, int index2);
};


#endif //BAYESIANNETWORK_BNSLCOMPARISON_H
