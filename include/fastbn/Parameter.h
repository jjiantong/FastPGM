//
// Created by jjt on 20/01/23.
//

#ifndef BAYESIANNETWORK_PARAMETER_H
#define BAYESIANNETWORK_PARAMETER_H

#endif //BAYESIANNETWORK_PARAMETER_H

#include "common.h"

class Parameter{
public:
    int job;
    int method;
    int num_threads; //for OpenMP
    int verbose;

    int num_samples; // for approximate inference
    int max_updating; // for approximate inference
    int updating_interval; // for approximate inference
    int propagation_length; // for epis-bn and lbp
    bool enable_heuristic_uniform_distribution;
    bool enable_heuristic_theta_cutoff;

    int group_size; // for PC-Stable
    double alpha; // for PC-Stable

    bool save_struct; // for structure learning
    bool save_param; // for parameter learning

    string net_file;
    string test_set_file;
    string pt_file;
    string train_set_file;
    string ref_net_file;

    Parameter();
    void ParseParameters(int argc, char *argv[]);
    void PrintHelpInfo();
    void PrintJobInfo();
    void PrintJobAndAlgInfo();
};

int add(int i, int j);