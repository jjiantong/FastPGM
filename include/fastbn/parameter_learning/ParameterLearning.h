//
// Created by jjt on 2021/6/23.
//

#ifndef BAYESIANNETWORK_PARAMETERLEARNING_H
#define BAYESIANNETWORK_PARAMETERLEARNING_H

#include "fastbn/Network.h"
#include "fastbn/Dataset.h"
#include "fastbn/DiscreteNode.h"
#include "fastbn/PotentialTableBase.h"
#include "fastbn/Timer.h"


class ParameterLearning {
public:
    Network *network; // the network to be learned

    ParameterLearning(Network *net) {network = net;};

    void LearnParamsKnowStructCompData(const Dataset *dts, int alpha, bool save_param, string param_file, int verbose);

    void SaveBNParameter(string param_file);
};





#endif //BAYESIANNETWORK_PARAMETERLEARNING_H
