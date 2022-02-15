//
// Created by jjt on 2022/2/15.
//

#ifndef BAYESIANNETWORK_VARIABLEELIMINATION_H
#define BAYESIANNETWORK_VARIABLEELIMINATION_H

#include "Inference.h"

class VariableElimination: public Inference {
public:
    VariableElimination(Network *net) { network = net;};

    virtual double EvaluateAccuracy(Dataset *dts, int num_samp, string alg, bool is_dense);

    int PredictUseVEInfer(DiscreteConfig evid, int target_node_idx, vector<int> elim_order=vector<int>{});
    vector<int> PredictUseVEInfer(vector<DiscreteConfig> evidences, int target_node_idx,
                                  vector<vector<int>> elim_orders=vector<vector<int>>{});
    Factor GetMarginalProbabilitiesUseVE(int target_var_index, DiscreteConfig evidence, vector<int> elim_order);
    vector<int> FilterOutIrrelevantNodes();
    vector<int> DefaultEliminationOrder(DiscreteConfig evidence, vector<int> left_nodes);
    Factor SumProductVarElim(vector<Factor> factors_list, vector<int> elim_order);
};

#endif //BAYESIANNETWORK_VARIABLEELIMINATION_H
