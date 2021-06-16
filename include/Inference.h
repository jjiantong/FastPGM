//
// Created by jjt on 2021/6/15.
//

#ifndef BAYESIANNETWORK_INFERENCE_H
#define BAYESIANNETWORK_INFERENCE_H

#include "Network.h"
#include "Dataset.h"
#include "Node.h"
#include "DiscreteNode.h"
#include "ContinuousNode.h"
#include "Factor.h"
#include "gadget.h"
#include <iostream>
#include <sys/time.h>
#include <locale.h>
#include "omp.h"

class Inference {
public:
    Network *network;//the learned network which can be used for inference

    Inference(){};
    Inference(Network *net) {network = net;};

    DiscreteConfig Sparse2Dense(DiscreteConfig evidence);

    /**
     * exact inference: brute force, variable elimination
     */
    int PredictDirectly(DiscreteConfig E, int Y_index);
    vector<int> PredictDirectly(vector<DiscreteConfig> evidences, int target_node_idx);
    map<int, double> GetMarginalProbabilitiesDirectly(int target_var_index, DiscreteConfig evidence);

    int PredictUseVEInfer(DiscreteConfig evid, int target_node_idx, vector<int> elim_order=vector<int>{});
    vector<int> PredictUseVEInfer(vector<DiscreteConfig> evidences, int target_node_idx,
                                  vector<vector<int>> elim_orders=vector<vector<int>>{});
    Factor GetMarginalProbabilitiesUseVE(int target_var_index, DiscreteConfig evidence, vector<int> elim_order);
    vector<int> FilterOutIrrelevantNodes();
    Factor SumProductVarElim(vector<Factor> factors_list, vector<int> elim_order);

    /**
     * approximate inference: likelihood weighting
     */
    int PredictUseLikelihoodWeighting(DiscreteConfig e, const int &node_index, const int &num_samp);
    vector<int> PredictUseLikelihoodWeighting(vector<DiscreteConfig> evidences,
                                              const int &target_node_idx, const int &num_samp);
    vector<pair<DiscreteConfig, double>> SampleUseLikelihoodWeighting(const DiscreteConfig &evidence, int num_samp);
    pair<DiscreteConfig, double> OneSampleUseLikelihoodWeighting(const DiscreteConfig &evidence);
    Factor GetMarginalProbabilitiesUseLikelihoodWeightingSamples(const vector<pair<DiscreteConfig, double>> &samples,
                                                                 const int &node_index);

    // TODO: see comments on EvaluateApproxInferAccuracy
    int ApproxInferByProbLogiRejectSamp(DiscreteConfig e, Node *node, vector<DiscreteConfig> &samples);
    int ApproxInferByProbLogiRejectSamp(DiscreteConfig e, int node_index, vector<DiscreteConfig> &samples);
    vector<int> ApproxInferByProbLogiRejectSamp(vector<DiscreteConfig> evidences, int node_idx,
                                                vector<DiscreteConfig> &samples);
    vector<DiscreteConfig> DrawSamplesByProbLogiSamp(int num_samp);
    DiscreteConfig GenerateInstanceByProbLogicSampleNetwork();

    /**
     * evaluation
     */
    double EvaluateExactInferenceAccuracy(Dataset *dts, string alg, bool is_dense);
    double EvaluateApproximateInferenceAccuracy(Dataset *dts, int num_samp, string alg, bool is_dense);
    // TODO: poor performance - check the algorithm.
    //  then merge into EvaluateApproximateInferenceAccuracy - consistency especially for sampling part
    double EvaluateApproxInferAccuracy(Dataset *dts, int num_samp, bool is_dense);

    double Accuracy(vector<int> ground_truth, vector<int> predictions);
};

#endif //BAYESIANNETWORK_INFERENCE_H
