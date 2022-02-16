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
DiscreteConfig Inference::Sparse2Dense(DiscreteConfig evidence, int num_nodes, int class_var_index) {
    set<int> existing_index;
    for (auto &e: evidence) {
        existing_index.insert(e.first);
    }

    DiscreteConfig dense_instance = evidence;
    for (int i = 0; i < num_nodes; i++) {
        if (i == class_var_index) { // skip the class variable
            continue;
        }
        // if node index i is not in existing evidence, filling 0
        if (existing_index.find(i) == existing_index.end()) {
            pair<int, int> p;
            p.first = i;
            p.second = 0;
            dense_instance.insert(p);
        }
    }

    return dense_instance;
}
