//
// Created by jjt on 2021/6/17.
//

#ifndef BAYESIANNETWORK_K2_H
#define BAYESIANNETWORK_K2_H

#include "StructureLearning.h"

class K2 : public StructureLearning {
public:
    virtual void StructLearnCompData(Dataset *dts, bool print_struct, string topo_ord_constraint, int max_num_parents);

    void StructLearnByK2Weka(Dataset *dts, vector<int> topo_ord_constraint, int max_num_parents);
};

#endif //BAYESIANNETWORK_K2_H
