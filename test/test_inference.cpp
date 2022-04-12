//
// Created by jjt on 2022/4/6.
//

#include <iostream>
#include <vector>
#include "gtest/gtest.h"
#include "gadget.h"

#include "Dataset.h"
#include "Node.h"
#include "DiscreteNode.h"
#include "Network.h"
#include "CustomNetwork.h"
#include "Inference.h"
#include "JunctionTree.h"

//class InferenceAlarm: public ::testing::Test {
//protected:
//
//    void SetUp() override {
//        tester = new Dataset();
//        network = new CustomNetwork(true);
//
//        string net_file_path = "/home/zeyiwen/jiantong/BN/dataset/alarm/alarm.xml";
//        string test_set_file_path = "/home/zeyiwen/jiantong/BN/dataset/alarm/alarm_2w_p20";
//
//        network->GetNetFromXMLBIFFile(net_file_path);
//        tester->LoadLIBSVMDataKnownNetwork(test_set_file_path, network->num_nodes);
//    }
//    CustomNetwork *network;
//    Dataset *tester;
//};
//
//TEST_F(InferenceAlarm, junction_tree_partial1) {
//    Inference *inference = new JunctionTree(network);
//    double accuracy = inference->EvaluateAccuracy(tester, 1, -1, "jt", false);
//    delete inference;
//    EXPECT_GT(accuracy, 0.8230);
//}