#include <iostream>

#include "fastbn/Dataset.h"
#include "fastbn/CustomNetwork.h"
#include "fastbn/inference/Inference.h"
#include "fastbn/inference/BruteForce.h"
#include "fastbn/inference/JunctionTree.h"
#include "fastbn/inference/VariableElimination.h"
#include "fastbn/Network.h"
#include "fastbn/structure_learning/StructureLearning.h"
#include "fastbn/structure_learning/PCStable.h"
#include "fastbn/structure_learning/BNSLComparison.h"
#include "fastbn/Parameter.h"
#include "fastbn/parameter_learning/ParameterLearning.h"
#include "fastbn/fastbn_api/fastbn_api.h"

using namespace std;

int main(int argc, char** argv) {

    Parameter param; // initialize parameters with default settings
    param.ParseParameters(argc, argv);

    string path = __FILE__;
    string project = "FastBN";
    string project_root = path.substr(0, path.find(project) + project.length() + 1);
    string dpath = project_root + "dataset/";

    /**
     * Job = structure learning
     * Method = PC-Stable
     * by default, we get a CPDAG and the graph may contain multiple independent sub-graphs. in order to get a DAG, or
     * to get one connected graph, change the corresponding arguments in `StructLearnCompData`
     */
    if (param.job == 0) {
        if (param.method != 0) {
                cout << "\tError! We currently only support -a 0 for PC-Stable structure learning" << endl;
                exit(0);
        }

        cout << "==================================================" << endl;
        cout << "Job: PC-stable for structure learning, #threads = " << param.num_threads << endl;
        cout << "\tgroup size = " << param.group_size << endl;
        cout << "\treference BN: " << param.ref_net_file << endl;
        cout << "\tsample set: " << param.train_set_file << endl;
        cout << "==================================================" << endl;

        string ref_file = "";
        if (!param.ref_net_file.empty()) {
            ref_file = dpath + param.ref_net_file;
        }

        BNSL_PCStable(param.verbose, param.num_threads, param.group_size,
                      param.alpha, ref_file, dpath + param.train_set_file,
                      param.save_struct);
    }

    /**
     * Job = learning (structure learning + parameter learning)
     * Method = PC-Stable + maximum likelihood estimation
     * by default, we get a CPDAG and the graph may contain multiple independent sub-graphs. in order to get a DAG, or
     * to get one connected graph, change the corresponding arguments in `StructLearnCompData`
     */
    else if (param.job == 1) {
        if (param.method != 0) {
            cout << "\tError! We currently only support -a 0 for PC-Stable structure learning + maximum likelihood "
                    "estimation parameter learning" << endl;
            exit(1);
        }

        cout << "==================================================" << endl;
        cout << "Job: PC-stable + maximum likelihood estimation for learning, #threads = " << param.num_threads << endl;
        cout << "\tgroup size = " << param.group_size << endl;
        cout << "\treference BN: " << param.ref_net_file << endl;
        cout << "\tsample set: " << param.train_set_file << endl;
        cout << "==================================================" << endl;

        string ref_file = "";
        if (!param.ref_net_file.empty()) {
            ref_file = dpath + param.ref_net_file;
        }

        BNL_PCStable(param.verbose, param.num_threads, param.group_size,
                      param.alpha, ref_file, dpath + param.train_set_file,
                      param.save_struct, param.save_param);
    }

    /**
     * Job = exact inference
     */
    else if (param.job == 2) {
        /**
         * Method = brute force
         * brute force should only work on full evidence
         */
        if (param.method == 0) {
            cout << "==================================================" << endl;
            cout << "Job: brute force for exact inference, #threads = " << param.num_threads << endl;
            cout << "\tBN: " << param.net_file << endl;
            cout << "\ttesting set: " << param.test_set_file << endl;
            cout << "\treference potential table: " << param.pt_file << endl;
            cout << "==================================================" << endl;

            cout << "Brute force for exact inference is under development" << endl;
            exit(1);
        }

        /**
         * Method = junction tree
         */
        else if (param.method == 1) {
            cout << "==================================================" << endl;
            cout << "Job: junction tree for exact inference, #threads = " << param.num_threads << endl;
            cout << "\tBN: " << param.net_file << endl;
            cout << "\ttesting set: " << param.test_set_file << endl;
            cout << "\treference potential table: " << param.pt_file << endl;
            cout << "==================================================" << endl;

            string pt = "";
            if (!param.pt_file.empty()) {
                pt = dpath + param.pt_file;
            }

            BNEI_JT(param.verbose, param.num_threads,
                    dpath + param.net_file, dpath + param.test_set_file, pt);
        }

        /**
         * Method = variable elimination
         */
        else if (param.method == 2) {
            cout << "==================================================" << endl;
            cout << "Job: variable elimination for exact inference, #threads = " << param.num_threads << endl;
            cout << "\tBN: " << param.net_file << endl;
            cout << "\ttesting set: " << param.test_set_file << endl;
            cout << "\treference potential table: " << param.pt_file << endl;
            cout << "==================================================" << endl;

            cout << "Variable elimination for exact inference is under development" << endl;
            exit(1);
        }

        else {
            cout << "\tError! For exact inference, we currently support -m 0 brute force, -m 1 junction tree and -m 2 "
                    "variable elimination." << endl;
            exit(1);
        }
    }

    /**
     * Job = approximate inference
     */
    else if (param.job == 3) {
        /**
         * Method = probabilistic logic sampling
         */
        if (param.method == 0) {
            cout << "==================================================" << endl;
            cout << "Job: probabilistic logic sampling for approximate inference, #threads = " << param.num_threads << endl;
            cout << "\t#samples: " << param.num_samples << endl;
            cout << "\tBN: " << param.net_file << endl;
            cout << "\ttesting set: " << param.test_set_file << endl;
            cout << "\treference potential table: " << param.pt_file << endl;
            cout << "==================================================" << endl;

            cout << "Probabilistic logic sampling for approximate inference is under development" << endl;
            exit(1);
        }
    }

    /**
     * Job = classification
     */
    else if (param.job == 4) {
        /**
         * Method = PC-Stable + maximum likelihood estimation + junction tree
         */
        if (param.method == 1) {
            cout << "==================================================" << endl;
            cout << "Job: PC-stable + maximum likelihood estimation + junction tree for classification, #threads = "
                 << param.num_threads << endl;
            cout << "\tgroup size = " << param.group_size << endl;
            cout << "\treference BN: " << param.ref_net_file << endl;
            cout << "\ttraining set: " << param.train_set_file << endl;
            cout << "\ttesting set: " << param.test_set_file << endl;
            cout << "==================================================" << endl;
            // todo: classification can also compare probability table, we can add param.pt_file

            string ref_file = "";
            if (!param.ref_net_file.empty()) {
                ref_file = dpath + param.ref_net_file;
            }

            C_PCStable_JT(param.verbose, param.num_threads, param.group_size, param.alpha,
                          ref_file, dpath + param.train_set_file, dpath + param.test_set_file,
                          param.save_struct, param.save_param);
        }
    }

    return 0;
}

