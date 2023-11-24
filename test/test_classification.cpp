//
// Created by jjt on 25/10/23.
//
#include <iostream>
#include <string>
#include <vector>
#include "gtest/gtest.h"

#include "Dataset.h"
#include "Network.h"
#include "StructureLearning.h"
#include "ParameterLearning.h"
#include "PCStable.h"
#include "Inference.h"
#include "JunctionTree.h"

#define GROUP_SIZE 1
#define VERBOSE 1


class TestClassificationAlarm : public ::testing::Test {
protected:

    void SetUp() override {
        trainer = new Dataset();
        tester = new Dataset();
        network = new Network(true);

        string train_set_file = "../../dataset/alarm/alarm_s5000.txt";
        trainer->LoadCSVData(train_set_file, true, true, 0);

        string test_set_file = "../../dataset/alarm/testing_alarm_1k_p20";
        tester->LoadLIBSVMDataKnownNetwork(test_set_file, trainer->num_vars,
                                           network->num_nodes);

        bnsl = new PCStable(network, 0.05);
        bnpl = new ParameterLearning(network);
    }

    Dataset *trainer;
    Dataset *tester;
    Network *network;
    StructureLearning *bnsl;
    ParameterLearning *bnpl;
};

TEST_F(TestClassificationAlarm, thread_1) {
    int group_size = GROUP_SIZE;
    int num_threads = 1;
    int verbose  = VERBOSE;

    bnsl->StructLearnCompData(trainer, group_size, num_threads, false, verbose);
    auto pc_bnsl = (PCStable*)bnsl;
    vector<int> roots = pc_bnsl->FindRootsInDAGForest();
    pc_bnsl->AddRootNode(roots);

    bnpl->LearnParamsKnowStructCompData(trainer, 1, verbose); // todo: alpha = 1
    SAFE_DELETE(trainer);

    Inference *inference = new JunctionTree(true, network, tester, false);
    SAFE_DELETE(tester);

    string pt_file = "../../dataset/alarm/alarm_1k_pt";
    double accuracy = inference->EvaluateAccuracy(pt_file, num_threads);
    cout << "accuracy = " << accuracy << endl;
    SAFE_DELETE(inference);
    SAFE_DELETE(bnsl);
    SAFE_DELETE(bnpl);
}

