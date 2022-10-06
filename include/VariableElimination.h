//
// Created by jjt on 6/10/22.
//

#ifndef BAYESIANNETWORK_VARIABLEELIMINATION_H
#define BAYESIANNETWORK_VARIABLEELIMINATION_H

#include "Inference.h"

class VariableElimination: public Inference {
public:
    VariableElimination(Network *net, Dataset *dts, bool is_dense): Inference(net, dts, is_dense) {};

    virtual double EvaluateAccuracy(int num_threads);

    int PredictUseVEInfer(DiscreteConfig evid, int target_node_idx, Timer *timer, vector<int> elim_order=vector<int>{});
    vector<int> PredictUseVEInfer(vector<DiscreteConfig> evidences, int target_node_idx, Timer *timer,
                                  vector<vector<int>> elim_orders=vector<vector<int>>{});
    Factor GetMarginalProbabilitiesUseVE(int target_var_index, DiscreteConfig evidence, Timer *timer, vector<int> elim_order);
    vector<int> FilterOutIrrelevantNodes();
    vector<int> DefaultEliminationOrder(DiscreteConfig evidence, vector<int> left_nodes);
    Factor SumProductVarElim(vector<Factor> factors_list, vector<int> elim_order);
};

#endif //BAYESIANNETWORK_VARIABLEELIMINATION_H
