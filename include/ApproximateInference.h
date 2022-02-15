//
// Created by jjt on 2021/6/16.
//

#ifndef BAYESIANNETWORK_APPROXIMATEINFERENCE_H
#define BAYESIANNETWORK_APPROXIMATEINFERENCE_H

#include "Inference.h"

/**
 * approximate inference: likelihood weighting
 */
//class ApproximateInference : public Inference {
//public:
//    ApproximateInference(Network *net) {network = net;};
//
//    virtual double EvaluateAccuracy(Dataset *dts, int num_samp, string alg, bool is_dense);
//
//    int PredictUseLikelihoodWeighting(DiscreteConfig e, const int &node_index, const int &num_samp);
//    vector<int> PredictUseLikelihoodWeighting(vector<DiscreteConfig> evidences,
//                                              const int &target_node_idx, const int &num_samp);
//    vector<pair<DiscreteConfig, double>> SampleUseLikelihoodWeighting(const DiscreteConfig &evidence, int num_samp);
//    pair<DiscreteConfig, double> OneSampleUseLikelihoodWeighting(const DiscreteConfig &evidence);
//    Factor GetMarginalProbabilitiesUseLikelihoodWeightingSamples(const vector<pair<DiscreteConfig, double>> &samples,
//                                                                 const int &node_index);
//
//    // TODO: poor performance - check the algorithm.
//    //  then merge into EvaluateAccuracy - consistency especially for sampling part
//    int ApproxInferByProbLogiRejectSamp(DiscreteConfig e, Node *node, vector<DiscreteConfig> &samples);
//    int ApproxInferByProbLogiRejectSamp(DiscreteConfig e, int node_index, vector<DiscreteConfig> &samples);
//    vector<int> ApproxInferByProbLogiRejectSamp(vector<DiscreteConfig> evidences, int node_idx,
//                                                vector<DiscreteConfig> &samples);
//    vector<DiscreteConfig> DrawSamplesByProbLogiSamp(int num_samp);
//    DiscreteConfig GenerateInstanceByProbLogicSampleNetwork();
//
//
//    int SampleNodeGivenMarkovBlanketReturnValIndex(Node *node_ptr, DiscreteConfig markov_blanket);
//    vector<DiscreteConfig> DrawSamplesByGibbsSamp(int num_samp, int num_burn_in);
//};


#endif //BAYESIANNETWORK_APPROXIMATEINFERENCE_H
