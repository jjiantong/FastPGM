//
// Created by jjt on 2021/6/16.
//

#ifndef BAYESIANNETWORK_BRUTEFORCE_H
#define BAYESIANNETWORK_BRUTEFORCE_H

#include "Inference.h"

class BruteForce : public Inference {
public:
    BruteForce(Network *net) { network = net;};

    virtual double EvaluateAccuracy(Dataset *dts, int num_samp, string alg, bool is_dense);

    int PredictDirectly(DiscreteConfig E, int Y_index);
    vector<int> PredictDirectly(vector<DiscreteConfig> evidences, int target_node_idx);
    map<int, double> GetMarginalProbabilitiesDirectly(int target_var_index, DiscreteConfig evidence);
};


#endif //BAYESIANNETWORK_BRUTEFORCE_H
