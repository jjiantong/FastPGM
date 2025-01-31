//
// Created by jjt on 21/03/24.
//

#ifndef BAYESIANNETWORK_FASTBN_API_H
#define BAYESIANNETWORK_FASTBN_API_H

#include <iostream>
#include "fastpgm/Dataset.h"
#include "fastpgm/Network.h"
#include "fastpgm/structure_learning/StructureLearning.h"
#include "fastpgm/structure_learning/PCStable.h"
#include "fastpgm/CustomNetwork.h"
#include "fastpgm/structure_learning/BNSLComparison.h"
#include "fastpgm/parameter_learning/ParameterLearning.h"
#include "fastpgm/inference/Inference.h"
#include "fastpgm/inference/JunctionTree.h"
#include "fastpgm/SampleSetGenerator.h"

using namespace std;

string GetLastPath(const string& path);
void BNSL_PCStable(int verbose, int n_threads, int group_size,
                   double alpha, string ref_net, string train_set,
                   bool save_struct);
void BNL_PCStable(int verbose, int n_threads, int group_size,
                  double alpha, string ref_net, string train_set,
                  bool save_struct, bool save_param);
void BNEI_JT(int verbose, int n_threads,
             string net, string test_set, string pt);
void C_PCStable_JT(int verbose, int n_threads, int group_size, double alpha,
                   string ref_net, string train_set, string test_set,
                   bool save_struct, bool save_param);
void Sample_Generator(int verbose, int n_threads, string net, bool libsvm, int num_samples,
                      int class_variable);



#endif //BAYESIANNETWORK_FASTBN_API_H
