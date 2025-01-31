//
// Created by jjt on 2021/6/15.
//

#ifndef BAYESIANNETWORK_INFERENCE_H
#define BAYESIANNETWORK_INFERENCE_H

#include "fastpgm/Network.h"
#include "fastpgm/Dataset.h"
#include "fastpgm/Node.h"
#include "fastpgm/DiscreteNode.h"
#include "fastpgm/ContinuousNode.h"
#include "fastpgm/PotentialTableBase.h"
#include "fastpgm/Timer.h"
#include <iostream>
#include <sys/time.h>
#include <locale.h>
#include "omp.h"

/**
 * abstract class
 */
class Inference {
public:
    /**
     * three modes: classification mode vs. inference mode. they are different in (i) whether to load the truth cpt and
     * compare the mse/hd and (ii) whether to load the ground truth and compute the accuracy. specifically:
     *                          msd/hd      accuracy
     * classification mode 1    no          yes
     * inference mode 0         yes         no
     * inference with no cpt 2  no          no
     */
    int mode; // 1 for classification mode, 0 for inference mode, 2 for inference with no reference cpt
    Network *network; // the learned network which can be used for inference
    int num_instances; // number of instances in the testing set
    /**
     * the index of the variable we are interested in (for each instance in the testing set). for the classification
     * problem, typically we have only one class variable to infer, so query_index is only one index. but for inference
     * problem on BNs, there are commonly a set of variables interested. we compute/estimate the probability of every
     * non-evidence variable in the network.
     */
    int query_index;

    vector<DiscreteConfig> evidences; // the evidences of each instance in the testing set
    vector<int> ground_truths; // the ground truths of each class variable
    vector<vector<double>> ground_truth_probability_tables; // the probabilities for each state of each node, computed by exact inference algorithms

    Inference(int classification, Network *net, Dataset *dts);
    Inference(Network *net); // it is used only in LBP for EPIS-BN

    virtual ~Inference() {};

    virtual double EvaluateAccuracy(string path, int num_threads);
    virtual vector<int> Predict(int num_threads) = 0;

    double Accuracy(vector<int> predictions);
    int ArgMax(const vector<double> &array);

    void LoadGroundTruthProbabilityTable(string file_path);
    double CalculateMSE(const vector<double> &approximate_distribution, int instance_index);
    double CalculateHellingerDistance(const vector<double> &approximate_distribution, int instance_index);
};

double Round(double number, unsigned int bits);

#endif //BAYESIANNETWORK_INFERENCE_H
