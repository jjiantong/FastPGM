////
//// Created by jjt on 25/10/23.
////
//#include <iostream>
//#include <string>
//#include <vector>
//#include "gtest/gtest.h"
//
//#include "Dataset.h"
//#include "Network.h"
//#include "StructureLearning.h"
//#include "ParameterLearning.h"
//#include "PCStable.h"
//#include "Inference.h"
//#include "JunctionTree.h"
//
//#define GROUP_SIZE 1
//#define VERBOSE 1
//
//
//class TestClassificationAlarm : public ::testing::Test {
//protected:
//
//    void SetUp() override {
//        trainer = new Dataset();
//        tester = new Dataset();
//        network = new Network(true);
//
//        string train_set_file = "../../dataset/alarm/alarm_s5000.txt";
//        trainer->LoadCSVData(train_set_file, true, true, 0);
//
//        bnsl = new PCStable(network, 0.05);
//        bnpl = new ParameterLearning(network);
//    }
//
//    Dataset *trainer;
//    Dataset *tester;
//    Network *network;
//    StructureLearning *bnsl;
//    ParameterLearning *bnpl;
//};
//
//TEST_F(TestClassificationAlarm, thread_1) {
//    int group_size = GROUP_SIZE;
//    int num_threads = 1;
//    int verbose  = VERBOSE;
//
//    bnsl->StructLearnCompData(trainer, group_size, num_threads, true, verbose);
//    bnpl->LearnParamsKnowStructCompData(trainer, 1, verbose); // todo: alpha = 1
////    SAFE_DELETE(trainer);
//
//    string test_set_file = "../../dataset/alarm/testing_alarm_1k_p20";
//    tester->LoadLIBSVMDataKnownNetwork(test_set_file, network->num_nodes);
//    Inference *inference = new JunctionTree(network, tester, false);
////    SAFE_DELETE(tester);
//
//    string pt_file = "../../dataset/alarm/alarm_1k_pt";
//    double accuracy = inference->EvaluateAccuracy(pt_file, num_threads);
//    cout << "accuracy = " << accuracy << endl;
////    SAFE_DELETE(inference);
//}
//
//
////class TestClassificationMunin2 : public ::testing::Test {
////protected:
////
////    void SetUp() override {
////        trainer = new Dataset();
////        tester = new Dataset();
////        network = new Network(true);
////
////        string train_set_file = "../../dataset/munin2/munin2_s5000.txt";
////        trainer->LoadCSVData(train_set_file, true, true, 0);
////
////        bnsl = new PCStable(network, 0.05);
////        bnpl = new ParameterLearning(network);
////    }
////
////    Dataset *trainer;
////    Dataset *tester;
////    Network *network;
////    StructureLearning *bnsl;
////    ParameterLearning *bnpl;
////};
////
////TEST_F(TestClassificationMunin2, thread_1) {
////    int group_size = GROUP_SIZE;
////    int num_threads = 4;
////    int verbose  = VERBOSE;
////
////    bnsl->StructLearnCompData(trainer, group_size, num_threads, true, verbose);
////    bnpl->LearnParamsKnowStructCompData(trainer, 1, verbose); // todo: alpha = 1
//////    SAFE_DELETE(trainer);
////
////    string test_set_file = "../../dataset/munin2/testing_munin2_100_evi20";
////    tester->LoadLIBSVMDataKnownNetwork(test_set_file, network->num_nodes);
////    Inference *inference = new JunctionTree(network, tester, false);
//////    SAFE_DELETE(tester);
////
////    string pt_file = "../../dataset/munin2/munin2_100_pt";
////    double accuracy = inference->EvaluateAccuracy(pt_file, num_threads);
////    cout << "accuracy = " << accuracy << endl;
//////    SAFE_DELETE(inference);
////}