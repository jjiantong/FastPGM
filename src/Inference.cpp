//
// Created by jjt on 2021/6/15.
//
#include "Inference.h"

/**
 * constructor of Inference
 * get evidences, query index and ground truths from the testing set
 * @param net the network used for inference
 * @param dts the testing set
 * @param is_dense whether we need to fill zero for the evidences
 */
Inference::Inference(Network *net, Dataset *dts, bool is_dense): network(net), num_instances(dts->num_instance), query_index(dts->class_var_index) {

    evidences.reserve(num_instances);
    ground_truths.reserve(num_instances);

    for (int i = 0; i < num_instances; ++i) {  // For each sample in test set
        vector<VarVal> vec_instance = dts->vector_dataset_all_vars.at(i);

        // construct an evidence by removing the class variable
        DiscreteConfig e;
        pair<int, int> p;
        for (int j = 0; j < vec_instance.size(); ++j) {
            if (j == query_index) { // skip the class variable
                continue;
            }
            p.first = vec_instance.at(j).first;
            p.second = vec_instance.at(j).second.GetInt();
            e.insert(p);
        }

        if (is_dense) {
            e = Sparse2Dense(e, network->num_nodes);
        }
        evidences.push_back(e);

        // construct the ground truth
        int g = vec_instance.at(query_index).second.GetInt();
        ground_truths.push_back(g);
    }
}

double Inference::Accuracy(vector<int> predictions) {
    int size = ground_truths.size(),
            num_of_correct = 0,
            num_of_wrong = 0;
    for (int i = 0; i < size; ++i) {
        int g = ground_truths.at(i),
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
DiscreteConfig Inference::Sparse2Dense(DiscreteConfig evidence, int num_nodes) {
    set<int> existing_index;
    for (auto &e: evidence) {
        existing_index.insert(e.first);
    }

    DiscreteConfig dense_instance = evidence;
    for (int i = 0; i < num_nodes; i++) {
        if (i == query_index) { // skip the class variable
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
