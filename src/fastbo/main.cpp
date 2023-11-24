#include <iostream>

#include "fastbo/Dataset.h"
#include "fastbo/CustomNetwork.h"
#include "fastbo/Inference.h"
#include "fastbo/BruteForce.h"
#include "fastbo/JunctionTree.h"
#include "fastbo/VariableElimination.h"
#include "fastbo/Network.h"
#include "fastbo/StructureLearning.h"
#include "fastbo/PCStable.h"
#include "fastbo/BNSLComparison.h"
#include "fastbo/Parameter.h"

using namespace std;

int main(int argc, char** argv) {
    cout << "Hello world." << endl;

//    Parameter param; // initialize parameters with default settings
//    param.ParseParameters(argc, argv);
//
//    if (param.algorithm == ALGPCSTABLE) {
//
//        cout << "===============================" << endl;
//        cout << "Algorithm: PC-stable for structure learning, #threads = " << param.num_threads << endl;
//        cout << "group size = " << param.group_size << endl;
//        cout << "\treference BN: " << param.ref_net_file << endl;
//        cout << "\tsample set: " << param.train_set_file << endl;
//        cout << "===============================" << endl;
//
//        Dataset *trainer = new Dataset();
//        Network *network = new Network(true);
//
//        trainer->LoadCSVData(param.train_set_file, true, true, 0);
//        StructureLearning *bnsl = new PCStable(network, 0.05);
//
//        bnsl->StructLearnCompData(trainer, param.group_size, param.num_threads, false, false);
//        SAFE_DELETE(trainer);
//
//        CustomNetwork *ref_net = new CustomNetwork();
//        ref_net->LoadBIFFile(param.ref_net_file);
//
//        BNSLComparison comp(ref_net, network);
//        int shd = comp.GetSHD();
//        cout << "SHD = " << shd << endl;
//        SAFE_DELETE(network);
//        SAFE_DELETE(ref_net);
//    }
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

