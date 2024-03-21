//
// Created by jjt on 21/03/24.
//
#include "fastbn/fastbn_api/fastbn_api.h"

void BNSL_PCStable(int verbose, int n_threads, int group_size, double alpha, string ref_net, string train_set,
                   bool save_struct) {

    Dataset *trainer = new Dataset(); // todo: decide whether the dataset is in libsvm format or in csv format
    trainer->LoadCSVTrainingData(train_set, true, true, 0);

    Network *network = new Network(true);
    StructureLearning *bnsl = new PCStable(network, alpha);
    bnsl->StructLearnCompData(trainer, group_size, n_threads,false, false,
                              save_struct, train_set + "_struct", verbose);
    SAFE_DELETE(trainer);

    if (!ref_net.empty()) {
        CustomNetwork *r_net = new CustomNetwork();
        r_net->LoadBIFFile(ref_net);
        BNSLComparison comp(r_net, network);
        int shd = comp.GetSHD();
        cout << "SHD = " << shd << endl;
        SAFE_DELETE(r_net);
    } else {
        cout << "There is no reference BN (ground-truth) provided, comparison (showing accuracy) is skipped." << endl;
    }

    SAFE_DELETE(network);
}