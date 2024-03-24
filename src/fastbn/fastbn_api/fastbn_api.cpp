//
// Created by jjt on 21/03/24.
//
#include "fastbn/fastbn_api/fastbn_api.h"

void BNSL_PCStable(int verbose, int n_threads, int group_size,
                   double alpha, string ref_net, string train_set,
                   bool save_struct) {

    Dataset *trainer = new Dataset(); // todo: decide whether the dataset is in libsvm format or in csv format
    trainer->LoadCSVTrainingData(train_set, true, true, 0);

    Network *network = new Network(true);
    StructureLearning *bnsl = new PCStable(network, alpha);
    // for structure learning, we don't change to dag or add ROOT
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

void BNL_PCStable(int verbose, int n_threads, int group_size,
                  double alpha, string ref_net, string train_set,
                  bool save_struct, bool save_param) {

    Dataset *trainer = new Dataset(); // todo: decide whether the dataset is in libsvm format or in csv format
    trainer->LoadCSVTrainingData(train_set, true, true, 0);

    Network *network = new Network(true);
    StructureLearning *bnsl = new PCStable(network, alpha);
    // for learning, we don't change to dag or add ROOT
    bnsl->StructLearnCompData(trainer, group_size, n_threads,false, false,
                              save_struct, train_set + "_struct", verbose);

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

    ParameterLearning *bnpl = new ParameterLearning(network);
    bnpl->LearnParamsKnowStructCompData(trainer, 1, save_param, train_set + "_param", verbose);

    SAFE_DELETE(trainer);
    SAFE_DELETE(network);
}

void BNEI_JT(int verbose, int n_threads,
             string net, string test_set, string pt) {

    CustomNetwork *network = new CustomNetwork(true);
    // todo: network file format
    network->LoadXMLBIFFile(net, 1);

    Dataset *tester = new Dataset(network);
    // todo: dataset file format
    tester->LoadCSVTestingData(test_set, true, true, 0);

    Inference *inference = new JunctionTree(0, network, tester);
    SAFE_DELETE(tester);

    double accuracy = inference->EvaluateAccuracy(pt, n_threads);
    SAFE_DELETE(inference);
    SAFE_DELETE(network);
}

void C_PCStable_JT(int verbose, int n_threads, int group_size, double alpha,
                   string ref_net, string train_set, string test_set,
                   bool save_struct, bool save_param) {

    Dataset *trainer = new Dataset(); // todo: decide whether the dataset is in libsvm format or in csv format
    trainer->LoadCSVTrainingData(train_set, true, true, 0);

    Dataset *tester = new Dataset(trainer);
    tester->LoadCSVTestingData(test_set, true, true, 0);

    Network *network = new Network(true);
    StructureLearning *bnsl = new PCStable(network, alpha);
    // for classification, we change to dag and add ROOT
    bnsl->StructLearnCompData(trainer, group_size, n_threads,true, true,
                              save_struct, train_set + "_struct", verbose);

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

    ParameterLearning *bnpl = new ParameterLearning(network);
    bnpl->LearnParamsKnowStructCompData(trainer, 1, save_param,train_set + "_param", verbose);
    SAFE_DELETE(trainer);

    Inference *inference = new JunctionTree(1, network, tester);
    SAFE_DELETE(tester);

    double accuracy = inference->EvaluateAccuracy("", n_threads);
    cout << "accuracy = " << accuracy << endl;
    SAFE_DELETE(inference);
    SAFE_DELETE(bnsl);
    SAFE_DELETE(bnpl);
}