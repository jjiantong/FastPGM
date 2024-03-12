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

using namespace std;

int main(int argc, char** argv) {
    cout << "Hello world." << endl;

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

        Dataset *trainer = new Dataset(); // todo: decide whether the dataset is in libsvm format or in csv format
        trainer->LoadCSVTrainingData(dpath + param.train_set_file, true, true, 0);

        Network *network = new Network(true);
        StructureLearning *bnsl = new PCStable(network, param.alpha);
        bnsl->StructLearnCompData(trainer, param.group_size, param.num_threads,false, false,
                                  param.save_struct, dpath + param.train_set_file + "_struct", param.verbose);
        SAFE_DELETE(trainer);

        if (!param.ref_net_file.empty()) {
            CustomNetwork *ref_net = new CustomNetwork();
            ref_net->LoadBIFFile(dpath + param.ref_net_file);
            BNSLComparison comp(ref_net, network);
            int shd = comp.GetSHD();
            cout << "SHD = " << shd << endl;
            SAFE_DELETE(ref_net);
        } else {
            cout << "There is no reference BN (ground-truth) provided, so BN comparison (showing accuracy) is skipped." << endl;
        }

        SAFE_DELETE(network);
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

        Dataset *trainer = new Dataset(); // todo: decide whether the dataset is in libsvm format or in csv format
        trainer->LoadCSVTrainingData(dpath + param.train_set_file, true, true, 0);

        Network *network = new Network(true);
        StructureLearning *bnsl = new PCStable(network, param.alpha);
        bnsl->StructLearnCompData(trainer, param.group_size, param.num_threads,false, false,
                                  param.save_struct, dpath + param.train_set_file + "_struct", param.verbose);

        if (!param.ref_net_file.empty()) {
            CustomNetwork *ref_net = new CustomNetwork();
            ref_net->LoadBIFFile(dpath + param.ref_net_file);
            BNSLComparison comp(ref_net, network);
            int shd = comp.GetSHD();
            cout << "SHD = " << shd << endl;
            SAFE_DELETE(ref_net);
        } else {
            cout << "There is no reference BN (ground-truth) provided, so BN comparison (showing accuracy) is skipped."
                    " You can provide the reference BN via -f1." << endl;
        }

        ParameterLearning *bnpl = new ParameterLearning(network);
        bnpl->LearnParamsKnowStructCompData(trainer, 1, param.save_param,
                                            dpath + param.train_set_file + "_param", param.verbose);

        SAFE_DELETE(trainer);
        SAFE_DELETE(network);
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

            CustomNetwork *network = new CustomNetwork(true);
            // todo: network file format
            network->LoadXMLBIFFile(dpath + param.net_file, 1);

            Dataset *tester = new Dataset(network);
            // todo: dataset file format
            tester->LoadCSVTestingData(dpath + param.test_set_file, true, true, 0);

            Inference *inference = new JunctionTree(0, network, tester);
            SAFE_DELETE(tester);

            string file = "";
            if (!param.pt_file.empty()) {
                file = dpath + param.pt_file;
            }
            double accuracy = inference->EvaluateAccuracy(file, param.num_threads);
            SAFE_DELETE(inference);
            SAFE_DELETE(network);
        }

        /**
         * Method = variable elimination
         */
        else if (param.method == 0) {
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

            Dataset *trainer = new Dataset(); // todo: decide whether the dataset is in libsvm format or in csv format
            trainer->LoadCSVTrainingData(dpath + param.train_set_file, true, true, 0);

            Dataset *tester = new Dataset(trainer);
            tester->LoadCSVTestingData(dpath + param.test_set_file, true, true, 0);

            Network *network = new Network(true);
            StructureLearning *bnsl = new PCStable(network, param.alpha);
            bnsl->StructLearnCompData(trainer, param.group_size, param.num_threads,true, true,
                                      param.save_struct, dpath + param.train_set_file + "_struct", param.verbose);

            if (!param.ref_net_file.empty()) {
                CustomNetwork *ref_net = new CustomNetwork();
                ref_net->LoadBIFFile(dpath + param.ref_net_file);
                BNSLComparison comp(ref_net, network);
                int shd = comp.GetSHD();
                cout << "SHD = " << shd << endl;
                SAFE_DELETE(ref_net);
            } else {
                cout << "There is no reference BN (ground-truth) provided, so BN comparison (showing accuracy) is skipped."
                        " You can provide the reference BN via -f1." << endl;
            }

            ParameterLearning *bnpl = new ParameterLearning(network);
            bnpl->LearnParamsKnowStructCompData(trainer, 1, param.save_param,
                                                dpath + param.train_set_file + "_param", param.verbose);
            SAFE_DELETE(trainer);

            Inference *inference = new JunctionTree(1, network, tester);
            SAFE_DELETE(tester);

            double accuracy = inference->EvaluateAccuracy("", param.num_threads);
            cout << "accuracy = " << accuracy << endl;
            SAFE_DELETE(inference);
            SAFE_DELETE(bnsl);
            SAFE_DELETE(bnpl);

//            SAFE_DELETE(network);
        }
    }

    return 0;
}

