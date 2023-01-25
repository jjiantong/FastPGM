//
// Created by jjt on 20/01/23.
//

#ifndef BAYESIANNETWORK_PARAMETER_H
#define BAYESIANNETWORK_PARAMETER_H

#endif //BAYESIANNETWORK_PARAMETER_H

#include "common.h"

#define ALGPCSTABLE 0
#define ALGBF 1
#define ALGJT 2
#define ALGVE 3
#define ALGPLS 4
#define ALGLW 5
#define ALGEPISBN 6
#define ALGLBP 7
#define ALGSIS 8
#define ALGAISBN 9

class Parameter{
public:
    int algorithm;
    int num_threads; //for OpenMP

    int num_samples; // for approximate inference
    int max_updating; // for approximate inference
    int updating_interval; // for approximate inference
    int propagation_length; // for epis-bn and lbp
    bool enable_heuristic_uniform_distribution;
    bool enable_heuristic_theta_cutoff;

    int group_size; // for PC-Stable

    string net_file;
    string test_set_file;
    string pt_file;
    string train_set_file;
    string ref_net_file;

    Parameter();
    void ParseParameters(int argc, char *argv[]);
    void PrintHelpInfo();
    void PrintFuncInfo();
};