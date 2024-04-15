//
// Created by jjt on 8/12/22.
//

#ifndef BAYESIANNETWORK_SAMPLESETGENERATOR_H
#define BAYESIANNETWORK_SAMPLESETGENERATOR_H

#include <iostream>
#include <fstream>
#include "Network.h"
#include "PotentialTableBase.h"

/**
 * @brief: this class is used to generate sample set from a given BN
 * pick a value for each node in BN according to the CPTs of the BN
 *  -- cannot randomly pick one value for each node
 */
class SampleSetGenerator {
public:
    Network *network;
    vector<vector<int>> samples;
    int num_samples;
    int class_var;

    /**
     * conditional probability tables of all the nodes in BN
     */
    vector<PotentialTableBase> cpts;

    SampleSetGenerator(Network *net, int num_samples, int class_var);
    void GenerateSamplesBasedOnCPTs();
    void OutputLIBSVM(string out_file_path);
    void OutputCSV(string out_file_path, int remove);
};

#endif //BAYESIANNETWORK_SAMPLESETGENERATOR_H
