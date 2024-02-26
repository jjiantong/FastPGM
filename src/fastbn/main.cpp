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
            cout << "There is no reference BN (ground-truth) provided, so BN comparison (showing accuracy) is skipped."
                    " You can provide the reference BN via -f1." << endl;
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
            exit(0);
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
//
//    else if (param.algorithm == ALGBF) {
//
//        cout << "===============================" << endl;
//        cout << "Algorithm: brute force (BF) exact inference (full evidence), #threads = " << param.num_threads << endl;
//        cout << "\tBN: " << param.net_file << endl;
//        cout << "\ttesting set: " << param.test_set_file << endl;
//        cout << "\treference potential table: " << param.pt_file << endl;
//        cout << "===============================" << endl;
//
//        cout << "BF is under development" << endl;
//    }
//
//    else if (param.algorithm == ALGJT) {
//
//        cout << "===============================" << endl;
//        cout << "Algorithm: junction tree (JT) for exact inference, #threads = " << param.num_threads << endl;
//        cout << "\tBN: " << param.net_file << endl;
//        cout << "\ttesting set: " << param.test_set_file << endl;
//        cout << "\treference potential table: " << param.pt_file << endl;
//        cout << "===============================" << endl;
//
//        Dataset *tester = new Dataset();
//        CustomNetwork *network = new CustomNetwork(true);
//        network->GetNetFromXMLBIFFile(param.net_file);
//        tester->LoadLIBSVMDataKnownNetwork(param.test_set_file, network->num_nodes);
//
//        Inference *inference = new JunctionTree(network, tester, false);
//        double accuracy = inference->EvaluateAccuracy(param.pt_file, param.num_threads);
//        cout << "accuracy = " << accuracy << endl;
//
//        SAFE_DELETE(inference);
//        SAFE_DELETE(network);
//        SAFE_DELETE(tester);
//    }
//
//    else if (param.algorithm == ALGVE) {
//
//        cout << "===============================" << endl;
//        cout << "Algorithm: variable elimination (VE) for exact inference, #threads = " << param.num_threads << endl;
//        cout << "\tBN: " << param.net_file << endl;
//        cout << "\ttesting set: " << param.test_set_file << endl;
//        cout << "\treference potential table: " << param.pt_file << endl;
//        cout << "===============================" << endl;
//
//        cout << "VE exact inference is under development" << endl;
//    }
//
//    else if (param.algorithm == ALGPLS) {
//
//        cout << "===============================" << endl;
//        cout << "Algorithm: probabilistic logic sampling (PLS) for approximate inference, #threads = " << param.num_threads << endl;
//        cout << "#samples = " << param.num_samples << endl;
//        cout << "\tBN: " << param.net_file << endl;
//        cout << "\ttesting set: " << param.test_set_file << endl;
//        cout << "\treference potential table: " << param.pt_file << endl;
//        cout << "===============================" << endl;
//
//        cout << "PLS is under development" << endl;
//    }
//
//    else if (param.algorithm == ALGLW) {
//
//        cout << "===============================" << endl;
//        cout << "Algorithm: likelihood weighting (LW) for approximate inference, #threads = " << param.num_threads << endl;
//        cout << "#samples = " << param.num_samples << endl;
//        cout << "\tBN: " << param.net_file << endl;
//        cout << "\ttesting set: " << param.test_set_file << endl;
//        cout << "\treference potential table: " << param.pt_file << endl;
//        cout << "===============================" << endl;
//
//        cout << "LW is under development" << endl;
//    }
//
//    else if (param.algorithm == ALGEPISBN) {
//
//        cout << "===============================" << endl;
//        cout << "Algorithm: EPIS-BN for approximate inference, #threads = " << param.num_threads << endl;
//        cout << "#samples = " << param.num_samples << ", propagation length = " << param.propagation_length << endl;
//        cout << "\tBN: " << param.net_file << endl;
//        cout << "\ttesting set: " << param.test_set_file << endl;
//        cout << "\treference potential table: " << param.pt_file << endl;
//        cout << "===============================" << endl;
//
//        cout << "EPIS-BN is under development" << endl;
//    }
//
//    else if (param.algorithm == ALGLBP) {
//
//        cout << "===============================" << endl;
//        cout << "Algorithm: loopy belief propagation (LBP) for approximate inference, #threads = " << param.num_threads << endl;
//        cout << ", propagation length = " << param.propagation_length << endl;
//        cout << "\tBN: " << param.net_file << endl;
//        cout << "\ttesting set: " << param.test_set_file << endl;
//        cout << "\treference potential table: " << param.pt_file << endl;
//        cout << "===============================" << endl;
//
//        cout << "LBP is under development" << endl;
//    }
//
//    else if (param.algorithm == ALGSIS) {
//
//        cout << "===============================" << endl;
//        cout << "Algorithm: self importance sampling (SIS) for approximate inference, #threads = " << param.num_threads << endl;
//        cout << "#samples = " << param.num_samples << "updating interval = " << param.updating_interval << "maximum updating times = " << param.max_updating << endl;
//        cout << "\tBN: " << param.net_file << endl;
//        cout << "\ttesting set: " << param.test_set_file << endl;
//        cout << "\treference potential table: " << param.pt_file << endl;
//        cout << "===============================" << endl;
//
//        cout << "SIS is under development" << endl;
//    }
//
//    else if (param.algorithm == ALGSISV1) {
//
//        cout << "===============================" << endl;
//        cout << "Algorithm: self importance sampling variant (SISv1) for approximate inference, #threads = " << param.num_threads << endl;
//        cout << "#samples = " << param.num_samples << "updating interval = " << param.updating_interval << "maximum updating times = " << param.max_updating << endl;
//        cout << "\tBN: " << param.net_file << endl;
//        cout << "\ttesting set: " << param.test_set_file << endl;
//        cout << "\treference potential table: " << param.pt_file << endl;
//        cout << "===============================" << endl;
//
//        cout << "SISv1 is under development" << endl;
//    }
//
//    else if (param.algorithm == ALGAISBN) {
//
//        cout << "===============================" << endl;
//        cout << "Algorithm: AIS-BN for approximate inference, #threads = " << param.num_threads << endl;
//        cout << "#samples = " << param.num_samples << "updating interval = " << param.updating_interval << "maximum updating times = " << param.max_updating << endl;
//        cout << "\tBN: " << param.net_file << endl;
//        cout << "\ttesting set: " << param.test_set_file << endl;
//        cout << "\treference potential table: " << param.pt_file << endl;
//        cout << "===============================" << endl;
//
//        cout << "AIS-BN is under development" << endl;
//    }
//
//    else if (param.algorithm == FUNCSAMPSET) {
//
//        cout << "===============================" << endl;
//        cout << "Function: generate a sample set from a BN, #threads = " << param.num_threads << endl;
//        cout << "#samples = " << param.num_samples << endl;
//        cout << "\tBN: " << param.net_file << endl;
//        cout << "===============================" << endl;
//
//        cout << "This function is under development" << endl;
//    }


    return 0;
}

