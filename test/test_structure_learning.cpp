//
// Created by jjt on 12/10/23.
//
#include <iostream>
#include <string>
#include <vector>
#include "gtest/gtest.h"
#include "tinyxml2.h"

#include "Dataset.h"
#include "Network.h"
#include "CustomNetwork.h"
#include "StructureLearning.h"
#include "PCStable.h"
#include "BNSLComparison.h"
#include "Parameter.h"

class TestStructureLearningPCStableAlarm : public ::testing::Test {
protected:

    void SetUp() override {
        trainer = new Dataset();
        network = new Network(true);

        string train_set_file = "../../dataset/alarm/alarm_s5000.txt";
        string ref_set_file = "../../dataset/alarm/alarm.bif";

        trainer->LoadCSVData(train_set_file, true, true, 0);
        bnsl = new PCStable(network, 0.05);

        ref_net = new CustomNetwork();
        ref_net->LoadBIFFile(ref_set_file);
    }

    Dataset *trainer;
    Network *network;
    CustomNetwork *ref_net;
    StructureLearning *bnsl;
};

TEST_F(TestStructureLearningPCStableAlarm, thread_1) {
    int group_size = 1;
    int num_threads = 1;
    bnsl->StructLearnCompData(trainer, group_size, num_threads, false, false);
    SAFE_DELETE(trainer);

    BNSLComparison comp(ref_net, network);
    int shd = comp.GetSHD();
    cout << "SHD = " << shd << endl;
    SAFE_DELETE(network);
    SAFE_DELETE(ref_net);
}


