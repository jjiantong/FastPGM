//
// Created by jjt on 21/03/24.
//

#ifndef BAYESIANNETWORK_FASTBN_API_H
#define BAYESIANNETWORK_FASTBN_API_H

#include <iostream>
#include "fastbn/Dataset.h"
#include "fastbn/Network.h"
#include "fastbn/structure_learning/StructureLearning.h"
#include "fastbn/structure_learning/PCStable.h"
#include "fastbn/CustomNetwork.h"
#include "fastbn/structure_learning/BNSLComparison.h"

using namespace std;

void BNSL_PCStable(int verbose, int n_threads, int group_size, double alpha, string ref_net, string train_set, bool save_struct);


#endif //BAYESIANNETWORK_FASTBN_API_H
