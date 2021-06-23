//
// Created by jjt on 2021/6/23.
//

#ifndef BAYESIANNETWORK_PCSTABLE_H
#define BAYESIANNETWORK_PCSTABLE_H

#include "StructureLearning.h"


class PCStable : public StructureLearning {
public:
    PCStable(Network *net) {network = net;};

    virtual void StructLearnCompData(Dataset *dts, bool print_struct, string topo_ord_constraint, int max_num_parents);
};

#endif //BAYESIANNETWORK_PCSTABLE_H
