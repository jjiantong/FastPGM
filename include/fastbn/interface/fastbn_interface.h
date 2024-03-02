//
// Created by jjt on 2/03/24.
//
#ifndef BAYESIANNETWORK_FASTBN_INTERFACE_H
#define BAYESIANNETWORK_FASTBN_INTERFACE_H

#include <iostream>

#include "fastbn/Dataset.h"
#include "fastbn/structure_learning/StructureLearning.h"
#include "fastbn/structure_learning/PCStable.h"
#include "fastbn/CustomNetwork.h"
#include "fastbn/structure_learning/BNSLComparison.h"


string GetDataPath();

void BNSLPCStable(int t, int g, int v, const char* refbn, const char* trainset, double a, bool stru);

#endif //BAYESIANNETWORK_FASTBN_INTERFACE_H
