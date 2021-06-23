//
// Created by jjt on 2021/6/23.
//

#ifndef BAYESIANNETWORK_PARAMETERLEARNING_H
#define BAYESIANNETWORK_PARAMETERLEARNING_H

#include "Network.h"
#include "Dataset.h"


class ParameterLearning {
public:
    Network *network; // the network to be learned

    ParameterLearning(Network *net) {network = net;};

    void LearnParamsKnowStructCompData(const Dataset *dts, int alpha=1, bool print_params=true);
};





#endif //BAYESIANNETWORK_PARAMETERLEARNING_H
