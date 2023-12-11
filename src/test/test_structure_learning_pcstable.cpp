////
//// Created by jjt on 12/10/23.
////
//#include <iostream>
//#include <string>
//#include <vector>
//#include "gtest/gtest.h"
//
//#include "fastbo/Dataset.h"
//#include "fastbo/Network.h"
//#include "fastbo/CustomNetwork.h"
//#include "fastbo/structure_learning/StructureLearning.h"
//#include "fastbo/structure_learning/PCStable.h"
//#include "fastbo/structure_learning/BNSLComparison.h"
//#include "fastbo/test_common.h"
//
//
//class TestStructureLearningPCStableAlarm : public ::testing::Test {
//protected:
//
//    void SetUp() override {
//        trainer = new Dataset();
//
//        string train_set_file = dataset_path + "alarm/alarm_s5000.txt";
//        trainer->LoadCSVTrainingData(train_set_file, true, true, 0);
//
//        network = new Network(true);
//        bnsl = new PCStable(network, ALPHA);
//
//        string ref_set_file = dataset_path + "alarm/alarm.bif";
//        ref_net = new CustomNetwork();
//        ref_net->LoadBIFFile(ref_set_file);
//    }
//
//    Dataset *trainer;
//    Network *network;
//    CustomNetwork *ref_net;
//    StructureLearning *bnsl;
//};
//
//TEST_F(TestStructureLearningPCStableAlarm, thread_1) {
//    int group_size = GROUP_SIZE;
//    int num_threads = 1;
//    int verbose  = VERBOSE;
//    bnsl->StructLearnCompData(trainer, group_size, num_threads, true, verbose);
//    SAFE_DELETE(trainer);
//
//    BNSLComparison comp(ref_net, network);
//    int shd = comp.GetSHD();
//    cout << "SHD = " << shd << endl;
//    SAFE_DELETE(network);
//    SAFE_DELETE(ref_net);
//}
////
////TEST_F(TestStructureLearningPCStableAlarm, thread_2) {
////    int group_size = GROUP_SIZE;
////    int num_threads = 2;
////    int verbose  = VERBOSE;
////    bnsl->StructLearnCompData(trainer, group_size, num_threads, false, verbose);
////    SAFE_DELETE(trainer);
////
////    BNSLComparison comp(ref_net, network);
////    int shd = comp.GetSHD();
////    cout << "SHD = " << shd << endl;
////    SAFE_DELETE(network);
////    SAFE_DELETE(ref_net);
////}
////
////TEST_F(TestStructureLearningPCStableAlarm, thread_4) {
////    int group_size = GROUP_SIZE;
////    int num_threads = 4;
////    int verbose  = VERBOSE;
////    bnsl->StructLearnCompData(trainer, group_size, num_threads, false, verbose);
////    SAFE_DELETE(trainer);
////
////    BNSLComparison comp(ref_net, network);
////    int shd = comp.GetSHD();
////    cout << "SHD = " << shd << endl;
////    SAFE_DELETE(network);
////    SAFE_DELETE(ref_net);
////}
////
////TEST_F(TestStructureLearningPCStableAlarm, thread_8) {
////    int group_size = GROUP_SIZE;
////    int num_threads = 8;
////    int verbose  = VERBOSE;
////    bnsl->StructLearnCompData(trainer, group_size, num_threads, false, verbose);
////    SAFE_DELETE(trainer);
////
////    BNSLComparison comp(ref_net, network);
////    int shd = comp.GetSHD();
////    cout << "SHD = " << shd << endl;
////    SAFE_DELETE(network);
////    SAFE_DELETE(ref_net);
////}
////
////TEST_F(TestStructureLearningPCStableAlarm, thread_16) {
////    int group_size = GROUP_SIZE;
////    int num_threads = 16;
////    int verbose  = VERBOSE;
////    bnsl->StructLearnCompData(trainer, group_size, num_threads, false, verbose);
////    SAFE_DELETE(trainer);
////
////    BNSLComparison comp(ref_net, network);
////    int shd = comp.GetSHD();
////    cout << "SHD = " << shd << endl;
////    SAFE_DELETE(network);
////    SAFE_DELETE(ref_net);
////}
////
////
////
//class TestStructureLearningPCStableInsurance: public ::testing::Test {
//protected:
//
//    void SetUp() override {
//        trainer = new Dataset();
//
//        string train_set_file = dataset_path + "insurance/insurance_s5000.txt";
//        trainer->LoadCSVTrainingData(train_set_file, true, true, 0);
//
//        network = new Network(true);
//        bnsl = new PCStable(network, ALPHA);
//
//        string ref_set_file = dataset_path + "insurance/insurance.bif";
//        ref_net = new CustomNetwork();
//        ref_net->LoadBIFFile(ref_set_file);
//    }
//
//    Dataset *trainer;
//    Network *network;
//    CustomNetwork *ref_net;
//    StructureLearning *bnsl;
//};
//
//TEST_F(TestStructureLearningPCStableInsurance, thread_1) {
//    int group_size = GROUP_SIZE;
//    int num_threads = 1;
//    int verbose  = VERBOSE;
//    bnsl->StructLearnCompData(trainer, group_size, num_threads, true, verbose);
//    SAFE_DELETE(trainer);
//
//    BNSLComparison comp(ref_net, network);
//    int shd = comp.GetSHD();
//    cout << "SHD = " << shd << endl;
//    SAFE_DELETE(network);
//    SAFE_DELETE(ref_net);
//}
////
////TEST_F(TestStructureLearningPCStableInsurance, thread_2) {
////    int group_size = GROUP_SIZE;
////    int num_threads = 2;
////    int verbose  = VERBOSE;
////    bnsl->StructLearnCompData(trainer, group_size, num_threads, false, verbose);
////    SAFE_DELETE(trainer);
////
////    BNSLComparison comp(ref_net, network);
////    int shd = comp.GetSHD();
////    cout << "SHD = " << shd << endl;
////    SAFE_DELETE(network);
////    SAFE_DELETE(ref_net);
////}
////
////TEST_F(TestStructureLearningPCStableInsurance, thread_4) {
////    int group_size = GROUP_SIZE;
////    int num_threads = 4;
////    int verbose  = VERBOSE;
////    bnsl->StructLearnCompData(trainer, group_size, num_threads, false, verbose);
////    SAFE_DELETE(trainer);
////
////    BNSLComparison comp(ref_net, network);
////    int shd = comp.GetSHD();
////    cout << "SHD = " << shd << endl;
////    SAFE_DELETE(network);
////    SAFE_DELETE(ref_net);
////}
////
////TEST_F(TestStructureLearningPCStableInsurance, thread_8) {
////    int group_size = GROUP_SIZE;
////    int num_threads = 8;
////    int verbose  = VERBOSE;
////    bnsl->StructLearnCompData(trainer, group_size, num_threads, false, verbose);
////    SAFE_DELETE(trainer);
////
////    BNSLComparison comp(ref_net, network);
////    int shd = comp.GetSHD();
////    cout << "SHD = " << shd << endl;
////    SAFE_DELETE(network);
////    SAFE_DELETE(ref_net);
////}
////
////TEST_F(TestStructureLearningPCStableInsurance, thread_16) {
////    int group_size = GROUP_SIZE;
////    int num_threads = 16;
////    int verbose  = VERBOSE;
////    bnsl->StructLearnCompData(trainer, group_size, num_threads, false, verbose);
////    SAFE_DELETE(trainer);
////
////    BNSLComparison comp(ref_net, network);
////    int shd = comp.GetSHD();
////    cout << "SHD = " << shd << endl;
////    SAFE_DELETE(network);
////    SAFE_DELETE(ref_net);
////}
////
////
////
//class TestStructureLearningPCStableHepar2: public ::testing::Test {
//protected:
//
//    void SetUp() override {
//        trainer = new Dataset();
//        string train_set_file = dataset_path + "hepar2/hepar2_s5000.txt";
//        trainer->LoadCSVTrainingData(train_set_file, true, true, 0);
//
//        network = new Network(true);
//        bnsl = new PCStable(network, ALPHA);
//
//        string ref_set_file = dataset_path + "hepar2/hepar2.bif";
//        ref_net = new CustomNetwork();
//        ref_net->LoadBIFFile(ref_set_file);
//    }
//
//    Dataset *trainer;
//    Network *network;
//    CustomNetwork *ref_net;
//    StructureLearning *bnsl;
//};
//
//TEST_F(TestStructureLearningPCStableHepar2, thread_1) {
//    int group_size = GROUP_SIZE;
//    int num_threads = 1;
//    int verbose  = VERBOSE;
//    bnsl->StructLearnCompData(trainer, group_size, num_threads, true, verbose);
//    SAFE_DELETE(trainer);
//
//    BNSLComparison comp(ref_net, network);
//    int shd = comp.GetSHD();
//    cout << "SHD = " << shd << endl;
//    SAFE_DELETE(network);
//    SAFE_DELETE(ref_net);
//}
////
////TEST_F(TestStructureLearningPCStableHepar2, thread_2) {
////    int group_size = GROUP_SIZE;
////    int num_threads = 2;
////    int verbose  = VERBOSE;
////    bnsl->StructLearnCompData(trainer, group_size, num_threads, false, verbose);
////    SAFE_DELETE(trainer);
////
////    BNSLComparison comp(ref_net, network);
////    int shd = comp.GetSHD();
////    cout << "SHD = " << shd << endl;
////    SAFE_DELETE(network);
////    SAFE_DELETE(ref_net);
////}
////
////TEST_F(TestStructureLearningPCStableHepar2, thread_4) {
////    int group_size = GROUP_SIZE;
////    int num_threads = 4;
////    int verbose  = VERBOSE;
////    bnsl->StructLearnCompData(trainer, group_size, num_threads, false, verbose);
////    SAFE_DELETE(trainer);
////
////    BNSLComparison comp(ref_net, network);
////    int shd = comp.GetSHD();
////    cout << "SHD = " << shd << endl;
////    SAFE_DELETE(network);
////    SAFE_DELETE(ref_net);
////}
////
////TEST_F(TestStructureLearningPCStableHepar2, thread_8) {
////    int group_size = GROUP_SIZE;
////    int num_threads = 8;
////    int verbose  = VERBOSE;
////    bnsl->StructLearnCompData(trainer, group_size, num_threads, false, verbose);
////    SAFE_DELETE(trainer);
////
////    BNSLComparison comp(ref_net, network);
////    int shd = comp.GetSHD();
////    cout << "SHD = " << shd << endl;
////    SAFE_DELETE(network);
////    SAFE_DELETE(ref_net);
////}
////
////TEST_F(TestStructureLearningPCStableHepar2, thread_16) {
////    int group_size = GROUP_SIZE;
////    int num_threads = 16;
////    int verbose  = VERBOSE;
////    bnsl->StructLearnCompData(trainer, group_size, num_threads, false, verbose);
////    SAFE_DELETE(trainer);
////
////    BNSLComparison comp(ref_net, network);
////    int shd = comp.GetSHD();
////    cout << "SHD = " << shd << endl;
////    SAFE_DELETE(network);
////    SAFE_DELETE(ref_net);
////}
//
//
//class TestStructureLearningPCStableMunin1: public ::testing::Test {
//protected:
//
//    void SetUp() override {
//        trainer = new Dataset();
//        string train_set_file = dataset_path + "hepar2/hepar2_s5000.txt";
//        trainer->LoadCSVTrainingData(train_set_file, true, true, 0);
//
//        network = new Network(true);
//        bnsl = new PCStable(network, ALPHA);
//
//        string ref_set_file = dataset_path + "hepar2/hepar2.bif";
//        ref_net = new CustomNetwork();
//        ref_net->LoadBIFFile(ref_set_file);
//    }
//
//    Dataset *trainer;
//    Network *network;
//    CustomNetwork *ref_net;
//    StructureLearning *bnsl;
//};
//
//TEST_F(TestStructureLearningPCStableMunin1, thread_1) {
//    int group_size = GROUP_SIZE;
//    int num_threads = 1;
//    int verbose  = VERBOSE;
//    bnsl->StructLearnCompData(trainer, group_size, num_threads, false, verbose);
//    SAFE_DELETE(trainer);
//
//    BNSLComparison comp(ref_net, network);
//    int shd = comp.GetSHD();
//    cout << "SHD = " << shd << endl;
//    SAFE_DELETE(network);
//    SAFE_DELETE(ref_net);
//}
//
//TEST_F(TestStructureLearningPCStableMunin1, thread_2) {
//    int group_size = GROUP_SIZE;
//    int num_threads = 2;
//    int verbose  = VERBOSE;
//    bnsl->StructLearnCompData(trainer, group_size, num_threads, false, verbose);
//    SAFE_DELETE(trainer);
//
//    BNSLComparison comp(ref_net, network);
//    int shd = comp.GetSHD();
//    cout << "SHD = " << shd << endl;
//    SAFE_DELETE(network);
//    SAFE_DELETE(ref_net);
//}
//
//TEST_F(TestStructureLearningPCStableMunin1, thread_4) {
//    int group_size = GROUP_SIZE;
//    int num_threads = 4;
//    int verbose  = VERBOSE;
//    bnsl->StructLearnCompData(trainer, group_size, num_threads, false, verbose);
//    SAFE_DELETE(trainer);
//
//    BNSLComparison comp(ref_net, network);
//    int shd = comp.GetSHD();
//    cout << "SHD = " << shd << endl;
//    SAFE_DELETE(network);
//    SAFE_DELETE(ref_net);
//}
//
//TEST_F(TestStructureLearningPCStableMunin1, thread_8) {
//    int group_size = GROUP_SIZE;
//    int num_threads = 8;
//    int verbose  = VERBOSE;
//    bnsl->StructLearnCompData(trainer, group_size, num_threads, false, verbose);
//    SAFE_DELETE(trainer);
//
//    BNSLComparison comp(ref_net, network);
//    int shd = comp.GetSHD();
//    cout << "SHD = " << shd << endl;
//    SAFE_DELETE(network);
//    SAFE_DELETE(ref_net);
//}
//
//TEST_F(TestStructureLearningPCStableMunin1, thread_16) {
//    int group_size = GROUP_SIZE;
//    int num_threads = 16;
//    int verbose  = VERBOSE;
//    bnsl->StructLearnCompData(trainer, group_size, num_threads, false, verbose);
//    SAFE_DELETE(trainer);
//
//    BNSLComparison comp(ref_net, network);
//    int shd = comp.GetSHD();
//    cout << "SHD = " << shd << endl;
//    SAFE_DELETE(network);
//    SAFE_DELETE(ref_net);
//}
//
