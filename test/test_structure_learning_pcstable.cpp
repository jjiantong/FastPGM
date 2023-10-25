////
//// Created by jjt on 12/10/23.
////
//#include <iostream>
//#include <string>
//#include <vector>
//#include "gtest/gtest.h"
//
//#include "Dataset.h"
//#include "Network.h"
//#include "CustomNetwork.h"
//#include "StructureLearning.h"
//#include "PCStable.h"
//#include "BNSLComparison.h"
//
//#define GROUP_SIZE 1
//#define VERBOSE 1
//
//
//class TestStructureLearningPCStableAlarm : public ::testing::Test {
//protected:
//
//    void SetUp() override {
//        trainer = new Dataset();
//        network = new Network(true);
//
//        string train_set_file = "../../dataset/alarm/alarm_s5000.txt";
//        string ref_set_file = "../../dataset/alarm/alarm.bif";
//
//        trainer->LoadCSVData(train_set_file, true, true, 0);
//        bnsl = new PCStable(network, 0.05);
//
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
//        network = new Network(true);
//
//        string train_set_file = "../../dataset/insurance/insurance_s5000.txt";
//        string ref_set_file = "../../dataset/insurance/insurance.bif";
//
//        trainer->LoadCSVData(train_set_file, true, true, 0);
//        bnsl = new PCStable(network, 0.05);
//
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
//        network = new Network(true);
//
//        string train_set_file = "../../dataset/hepar2/hepar2_s5000.txt";
//        string ref_set_file = "../../dataset/hepar2/hepar2.bif";
//
//        trainer->LoadCSVData(train_set_file, true, true, 0);
//        bnsl = new PCStable(network, 0.05);
//
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
//        network = new Network(true);
//
//        string train_set_file = "../../dataset/munin1/munin1_s5000_o.txt"; // TODO: forget the reason of using '_o'
//        string ref_set_file = "../../dataset/munin1/munin1.bif";
//
//        trainer->LoadCSVData(train_set_file, true, true, 0);
//        bnsl = new PCStable(network, 0.05);
//
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
//
//
////class TestStructureLearningPCStableDiabetes: public ::testing::Test {
////protected:
////
////    void SetUp() override {
////        trainer = new Dataset();
////        network = new Network(true);
////
////        string train_set_file = "../../dataset/diabetes/diabetes_s5000.txt";
////        string ref_set_file = "../../dataset/diabetes/diabetes.bif";
////
////        trainer->LoadCSVData(train_set_file, true, true, 0);
////        bnsl = new PCStable(network, 0.05);
////
////        ref_net = new CustomNetwork();
////        ref_net->LoadBIFFile(ref_set_file);
////    }
////
////    Dataset *trainer;
////    Network *network;
////    CustomNetwork *ref_net;
////    StructureLearning *bnsl;
////};
////
////TEST_F(TestStructureLearningPCStableDiabetes, thread_1) {
////    int group_size = GROUP_SIZE;
////    int num_threads = 1;
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
////TEST_F(TestStructureLearningPCStableDiabetes, thread_2) {
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
////TEST_F(TestStructureLearningPCStableDiabetes, thread_4) {
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
////TEST_F(TestStructureLearningPCStableDiabetes, thread_8) {
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
////TEST_F(TestStructureLearningPCStableDiabetes, thread_16) {
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
////class TestStructureLearningPCStableLink: public ::testing::Test {
////protected:
////
////    void SetUp() override {
////        trainer = new Dataset();
////        network = new Network(true);
////
////        string train_set_file = "../../dataset/link/link_s5000.txt";
////        string ref_set_file = "../../dataset/link/link.bif";
////
////        trainer->LoadCSVData(train_set_file, true, true, 0);
////        bnsl = new PCStable(network, 0.05);
////
////        ref_net = new CustomNetwork();
////        ref_net->LoadBIFFile(ref_set_file);
////    }
////
////    Dataset *trainer;
////    Network *network;
////    CustomNetwork *ref_net;
////    StructureLearning *bnsl;
////};
////
////TEST_F(TestStructureLearningPCStableLink, thread_1) {
////    int group_size = GROUP_SIZE;
////    int num_threads = 1;
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
////TEST_F(TestStructureLearningPCStableLink, thread_2) {
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
////TEST_F(TestStructureLearningPCStableLink, thread_4) {
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
////TEST_F(TestStructureLearningPCStableLink, thread_8) {
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
////TEST_F(TestStructureLearningPCStableLink, thread_16) {
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
//class TestStructureLearningPCStableMunin2: public ::testing::Test {
//protected:
//
//    void SetUp() override {
//        trainer = new Dataset();
//        network = new Network(true);
//
//        string train_set_file = "../../dataset/munin2/munin2_s5000.txt";
//        string ref_set_file = "../../dataset/munin2/munin2.bif";
//
//        trainer->LoadCSVData(train_set_file, true, true, 0);
//        bnsl = new PCStable(network, 0.05);
//
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
//TEST_F(TestStructureLearningPCStableMunin2, thread_1) {
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
//TEST_F(TestStructureLearningPCStableMunin2, thread_2) {
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
//TEST_F(TestStructureLearningPCStableMunin2, thread_4) {
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
//TEST_F(TestStructureLearningPCStableMunin2, thread_8) {
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
//TEST_F(TestStructureLearningPCStableMunin2, thread_16) {
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
////
////
////
////class TestStructureLearningPCStableMunin3: public ::testing::Test {
////protected:
////
////    void SetUp() override {
////        trainer = new Dataset();
////        network = new Network(true);
////
////        string train_set_file = "../../dataset/munin3/munin3_s5000.txt";
////        string ref_set_file = "../../dataset/munin3/munin3.bif";
////
////        trainer->LoadCSVData(train_set_file, true, true, 0);
////        bnsl = new PCStable(network, 0.05);
////
////        ref_net = new CustomNetwork();
////        ref_net->LoadBIFFile(ref_set_file);
////    }
////
////    Dataset *trainer;
////    Network *network;
////    CustomNetwork *ref_net;
////    StructureLearning *bnsl;
////};
////
////TEST_F(TestStructureLearningPCStableMunin3, thread_1) {
////    int group_size = GROUP_SIZE;
////    int num_threads = 1;
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
////TEST_F(TestStructureLearningPCStableMunin3, thread_2) {
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
////TEST_F(TestStructureLearningPCStableMunin3, thread_4) {
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
////TEST_F(TestStructureLearningPCStableMunin3, thread_8) {
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
////TEST_F(TestStructureLearningPCStableMunin3, thread_16) {
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