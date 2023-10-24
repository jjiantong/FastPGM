//
// Created by jjt on 2021/6/23.
//

#ifndef BAYESIANNETWORK_PARAMETERLEARNING_H
#define BAYESIANNETWORK_PARAMETERLEARNING_H

#include "Network.h"
#include "Dataset.h"
#include "DiscreteNode.h"
#include "Timer.h"


class ParameterLearning {
public:
    Network *network; // the network to be learned

    ParameterLearning(Network *net) {network = net;};

    void LearnParamsKnowStructCompData(const Dataset *dts, int alpha, int verbose);
};





#endif //BAYESIANNETWORK_PARAMETERLEARNING_H
