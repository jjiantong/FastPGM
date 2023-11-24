//
// Created by jjt on 8/12/22.
//
#include "fastbo/SampleSetGenerator.h"

SampleSetGenerator::SampleSetGenerator(Network *net, int num_samples): network(net), num_samples(num_samples) {
    int num_nodes = network->num_nodes;
    cpts.resize(num_nodes);

    samples.resize(num_samples);
    for (int i = 0; i < num_samples; ++i) {
        samples[i].resize(num_nodes);
    }
}

/**
 * @brief: generate samples and output using LIBSVM format
 * variable 0 is query variable by default
 */
void SampleSetGenerator::GenerateSamplesBasedOnCPTs() {
    /**
     * initialization CPTs of each node in the network
     */
    int i = 0;
    for (auto &id_node_ptr : network->map_idx_node_ptr) { // for each node of the network
        auto node_ptr = id_node_ptr.second;
        // add the factor that consists of this node and its parents
        cpts[i] = PotentialTableBase(dynamic_cast<DiscreteNode*>(node_ptr), network);
        i++;
    }

    /**
     * generate samples: forward sampling: generate samples follow the topological ordering
     */
    for (int i = 0; i < num_samples; ++i) {
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        default_random_engine rand_gen (seed);
        for (const auto &index : network->GetTopoOrd()) { // for each node following the topological ordering
            vector<double> weights(cpts[index].var_dims[0]); // dimension of this node
            cpts[index].GetReducedPotentials(weights, samples[i], 1); // TODO number threads
            discrete_distribution<int> this_distribution(weights.begin(), weights.end());
            samples[i][index] = this_distribution(rand_gen); // get the final value
        }
    }

    /**
     * output the results
     */
    for (int i = 0; i < num_samples; ++i) {
        cout << samples[i][0] << " ";
        for (int j = 1; j < network->num_nodes - 1; ++j) {
            cout << j << ":" << samples[i][j] << " ";
        }
        cout << network->num_nodes - 1 << ":" << samples[i][network->num_nodes - 1] << endl;
    }
}