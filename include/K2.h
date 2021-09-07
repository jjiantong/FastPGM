//
// Created by jjt on 2021/6/17.
//

#ifndef BAYESIANNETWORK_K2_H
#define BAYESIANNETWORK_K2_H

#include "StructureLearning.h"

class K2 : public StructureLearning {
public:
    string order_constraint;
    vector<int> order;
    int max_num_parents;

    K2(Network *net) {network = net;};
    K2(Network *net, string oc, int mnp) {network = net; order_constraint = oc; max_num_parents = mnp;};

    virtual void StructLearnCompData(Dataset *dts, int group_size, int num_threads, bool print_struct, bool verbose);

    void StructLearnByK2Weka(Dataset *dts);
};

#endif //BAYESIANNETWORK_K2_H
