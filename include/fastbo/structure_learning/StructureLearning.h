//
// Created by jjt on 2021/6/17.
//

#ifndef BAYESIANNETWORK_STRUCTURELEARNING_H
#define BAYESIANNETWORK_STRUCTURELEARNING_H

#include "fastbo/Network.h"
#include "fastbo/Dataset.h"
#include "fastbo/DiscreteNode.h"


class StructureLearning {
public:
    Network *network; // the network to be learned

    virtual ~StructureLearning() {};

    /**
     * @param group_size and @param num_threads are used only for PCStable
     */
    virtual void StructLearnCompData(Dataset *dts, int group_size, int num_threads, bool print_struct, int verbose) = 0;
    void AssignNodeInformation(Dataset *dts);
    vector<int> AssignNodeOrder(string topo_ord_constraint);
};

#endif //BAYESIANNETWORK_STRUCTURELEARNING_H
