//
// Created by jjt on 2021/6/16.
//

#ifndef BAYESIANNETWORK_EXACTINFERENCE_H
#define BAYESIANNETWORK_EXACTINFERENCE_H

#include "Inference.h"

/**
 * exact inference: brute force, variable elimination
 */
class ExactInference : public Inference {
public:
    ExactInference(Network *net) {network = net;};

    virtual double EvaluateAccuracy(Dataset *dts, int num_samp, string alg, bool is_dense);

    int PredictDirectly(DiscreteConfig E, int Y_index);
    vector<int> PredictDirectly(vector<DiscreteConfig> evidences, int target_node_idx);
    map<int, double> GetMarginalProbabilitiesDirectly(int target_var_index, DiscreteConfig evidence);

    int PredictUseVEInfer(DiscreteConfig evid, int target_node_idx, vector<int> elim_order=vector<int>{});
    vector<int> PredictUseVEInfer(vector<DiscreteConfig> evidences, int target_node_idx,
                                  vector<vector<int>> elim_orders=vector<vector<int>>{});
    Factor GetMarginalProbabilitiesUseVE(int target_var_index, DiscreteConfig evidence, vector<int> elim_order);
    vector<int> FilterOutIrrelevantNodes();
    vector<int> DefaultEliminationOrder(DiscreteConfig evidence, vector<int> left_nodes);
    Factor SumProductVarElim(vector<Factor> factors_list, vector<int> elim_order);
};


#endif //BAYESIANNETWORK_EXACTINFERENCE_H
