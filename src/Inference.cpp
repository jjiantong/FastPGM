//
// Created by jjt on 2021/6/15.
//

#include "Inference.h"

double Inference::Accuracy(vector<int> ground_truth, vector<int> predictions) {
    int size = ground_truth.size(),
            num_of_correct = 0,
            num_of_wrong = 0;
    for (int i = 0; i < size; ++i) {
        int g = ground_truth.at(i),
                p = predictions.at(i);
        if (g == p) {
            ++num_of_correct;
        } else {
            ++num_of_wrong;
        }
    }
    double accuracy = num_of_correct / (double)(num_of_correct+num_of_wrong);
    return accuracy;
}

/**
 * @brief: convert sparse to dense by filling zero
 */
DiscreteConfig Inference::Sparse2Dense(DiscreteConfig evidence) {
    set<int> existing_index;
    for (auto it = evidence.begin(); it != evidence.end(); it++) {
        existing_index.insert(it->first);
    }
    for (int i = 1; i < network->num_nodes; i++) { // for all nodes except for the target node
        // if node index i is not in existing evidence, filling 0
        if (existing_index.find(i) == existing_index.end()) {
            pair<int, int> p;
            p.first = i;
            p.second = 0;
            evidence.insert(p);
        }
    }

    DiscreteConfig dense_instance = evidence;
    return dense_instance;
}
