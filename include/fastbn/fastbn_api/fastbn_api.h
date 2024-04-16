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
#include "fastbn/parameter_learning/ParameterLearning.h"
#include "fastbn/inference/Inference.h"
#include "fastbn/inference/JunctionTree.h"
#include "fastbn/SampleSetGenerator.h"

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
