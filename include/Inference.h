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
#include "PotentialTable.h"
#include "Timer.h"
#include <iostream>
#include <sys/time.h>
#include <locale.h>
#include "omp.h"

/**
 * abstract class
 */
class Inference {
public:
    Network *network; // the learned network which can be used for inference
    int num_instances; // number of instances in the testing set
    /**
     * the index of the variable we are interested in (for each instance in the testing set)
     * for the classification problem, typically we have only one class variable to infer, so query_index is only one index
     * but actually for inference problem on BNs, there are commonly a set of variables interested
     * meanwhile, we commonly compute/estimate the probability of every non-evidence variable in the network
     */
    int query_index;

    vector<DiscreteConfig> evidences; // the evidences of each instance in the testing set
    vector<int> ground_truths; // the ground truths of each class variable

    Inference(Network *net, Dataset *dts, bool is_dense);
    virtual ~Inference() {};

    virtual double EvaluateAccuracy(int num_threads)= 0;

    double Accuracy(vector<int> predictions);
    DiscreteConfig Sparse2Dense(DiscreteConfig evidence, int num_nodes);
};

#endif //BAYESIANNETWORK_INFERENCE_H
