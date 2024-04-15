//
// Created by jjt on 8/12/22.
//
#include "fastbn/SampleSetGenerator.h"

SampleSetGenerator::SampleSetGenerator(Network *net, int num_samples, int class_var):
                                       network(net), num_samples(num_samples), class_var(class_var) {
    int num_nodes = network->num_nodes;
    cpts.resize(num_nodes);

    samples.resize(num_samples);
    for (int i = 0; i < num_samples; ++i) {
        samples[i].resize(num_nodes);
    }
}

/**
 * @brief: generate samples
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
}

/**
 * @brief: output in LIBSVM format
 * Sample set in LIBSVM format must be a complete dataset.
 * If requiring a output file in CSV format, also
 */
void SampleSetGenerator::OutputLIBSVM(string out_file_path) {
    ofstream out_file(out_file_path);
    if (!out_file.is_open()) {
        fprintf(stderr, "Error in function %s!", __FUNCTION__);
        fprintf(stderr, "Unable to open file %s!", out_file_path.c_str());
        exit(1);
    }

    for (int i = 0; i < num_samples; ++i) {
        string sample;

        // label
        sample = to_string(samples[i][class_var]) + " ";

        // features
        for (int j = 0; j < network->num_nodes; ++j) {
            if (j != class_var) {
                int var = samples[i][j];
                if (var != 0) {
                    sample += to_string(j) + ":" + to_string(var) + " ";
                }
            }
        }
        out_file << sample << endl;
    }

    cout << "Finish generating sample set with csv format. "
         << "Number of samples: " << num_samples << ". "
         << "Class variable id: " << class_var << "." << endl;

    out_file.close();
}