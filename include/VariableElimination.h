//
// Created by jjt on 6/10/22.
//

#ifndef BAYESIANNETWORK_VARIABLEELIMINATION_H
#define BAYESIANNETWORK_VARIABLEELIMINATION_H

#include "Inference.h"
#include "PotentialTable.h"

class VariableElimination: public Inference {
public:
    vector<PotentialTable> cpts;

    VariableElimination(Network *net, Dataset *dts, bool is_dense): Inference(net, dts, is_dense) {};

    virtual double EvaluateAccuracy(int num_threads);

protected:
    void InitializeCPTAndLoadEvidence(const vector<int> &left_nodes, const DiscreteConfig &evidence, int num_threads);

    int PredictUseVEInfer(const DiscreteConfig &evid, int num_threads, Timer *timer, vector<int> elim_order=vector<int>{});
    vector<int> PredictUseVEInfer(int num_threads, Timer *timer, vector<vector<int>> elim_orders=vector<vector<int>>{});
    PotentialTable GetMarginalProbabilitiesUseVE(const DiscreteConfig &evidence, int num_threads, Timer *timer, vector<int> elim_order);
    vector<int> FilterOutIrrelevantNodes();
    vector<int> DefaultEliminationOrder(const DiscreteConfig &evidence, const vector<int> &left_nodes);
    PotentialTable SumProductVE(vector<int> elim_order);
};

#endif //BAYESIANNETWORK_VARIABLEELIMINATION_H
