#include <iostream>

#include "Dataset.h"
#include "CustomNetwork.h"
#include "Inference.h"
#include "BruteForce.h"
#include "JunctionTree.h"
#include "VariableElimination.h"
#include "Network.h"
#include "StructureLearning.h"
#include "PCStable.h"
#include "BNSLComparison.h"
#include "Parameter.h"

using namespace std;

int main(int argc, char** argv) {

    Parameter param; // initialize parameters with default settings
    param.ParseParameters(argc, argv);

//    cout << "===============================" << endl;
//    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
//    cout << "task: " << param.algorithm << " #threads = " << param.num_threads << endl;
//    cout << "(0: PC-stable; 1: brute force; 2: junction tree; 3: variable elimination; 4: probabilistic logic sampling;" << endl
//         << "5: likelihood weighting; 6: EPIS-BN; 7: loopy belief propagation; 8: self-importance sampling; 9: AIS-BN)" << endl;
//    cout << "===============================" << endl;
//    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
//    if (param.algorithm == ALGJT) {
//        Dataset *tester = new Dataset();
//        CustomNetwork *network = new CustomNetwork(true);
//
//        network->GetNetFromXMLBIFFile(param.net_file_path);
//        tester->LoadLIBSVMDataKnownNetwork(param.test_set_file_path, network->num_nodes);
//
//        Inference *inference = new JunctionTree(network, tester, false);
//        double accuracy = inference->EvaluateAccuracy(param.prob_tab_file_path, param.num_threads);
//        cout << "accuracy = " << accuracy << endl;
//
//        delete inference;
//        delete network;
//        delete tester;
//    }
//
//    else if (param.algorithm == ALGPCSTABLE) {
//        Dataset *trainer = new Dataset();
//        Network *network = new Network(true);
//
//        trainer->LoadCSVData(param.train_set_file_path, true, true, 0);
//        StructureLearning *bnsl = new PCStable(network, 0.05);
//
//        bnsl->StructLearnCompData(trainer, param.group_size, param.num_threads, false, false);
//        delete trainer;
//
//        CustomNetwork *ref_net = new CustomNetwork();
//        ref_net->LoadBIFFile(param.ref_net_path);
//
//        BNSLComparison comp(ref_net, network);
//        int shd = comp.GetSHD();
//        cout << "SHD = " << shd << endl;
//        delete network;
//        delete ref_net;
//    }

    return 0;
}

