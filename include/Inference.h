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
#include "PotentialTable.h"
#include "gadget.h"
#include "Timer.h"
#include <iostream>
#include <sys/time.h>
#include <locale.h>
#include "omp.h"

class Inference {
public:
    Network *network;//the learned network which can be used for inference

    virtual double EvaluateAccuracy(Dataset *dts, int num_threads, int num_samp, string alg, bool is_dense)= 0;

    virtual ~Inference() {};

    double Accuracy(vector<int> ground_truth, vector<int> predictions);
    DiscreteConfig Sparse2Dense(DiscreteConfig evidence, int num_nodes, int class_var_index);

};

#endif //BAYESIANNETWORK_INFERENCE_H
