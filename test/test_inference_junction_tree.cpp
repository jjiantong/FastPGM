////
//// Created by jjt on 13/10/23.
////
//#include <iostream>
//#include <string>
//#include <vector>
//#include "gtest/gtest.h"
//
//#include "Dataset.h"
//#include "CustomNetwork.h"
//#include "Inference.h"
//#include "JunctionTree.h"
//
//
//class TestExactInferenceJTAlarm: public ::testing::Test {
//protected:
//
//    void SetUp() override {
//        Dataset *tester = new Dataset();
//        CustomNetwork *network = new CustomNetwork(true);
//
//        string net_file = "../../dataset/alarm/alarm.xml";
//        string test_set_file = "../../dataset/alarm/testing_alarm_1k_p20";
//
//        network->GetNetFromXMLBIFFile(net_file);
//        tester->LoadLIBSVMDataKnownNetwork(test_set_file, network->num_nodes);
//
//        inference = new JunctionTree(false, network, tester, false);
////        SAFE_DELETE(network);
//        SAFE_DELETE(tester);
//    }
//
//    Inference *inference;
//};
//
//TEST_F(TestExactInferenceJTAlarm, thread_1) {
//    string pt_file = "../../dataset/alarm/alarm_1k_pt";
//    int num_threads = 1;
//
//    double accuracy = inference->EvaluateAccuracy(pt_file, num_threads);
////    cout << "accuracy = " << accuracy << endl;
//    SAFE_DELETE(inference);
//}
////
////TEST_F(TestExactInferenceJTAlarm, thread_2) {
////    string pt_file = "../../dataset/alarm/alarm_1k_pt";
////    int num_threads = 2;
////
////    double accuracy = inference->EvaluateAccuracy(pt_file, num_threads);
////    cout << "accuracy = " << accuracy << endl;
////    SAFE_DELETE(inference);
////}
////
////TEST_F(TestExactInferenceJTAlarm, thread_4) {
////    string pt_file = "../../dataset/alarm/alarm_1k_pt";
////    int num_threads = 4;
////
////    double accuracy = inference->EvaluateAccuracy(pt_file, num_threads);
////    cout << "accuracy = " << accuracy << endl;
////    SAFE_DELETE(inference);
////}
////
////TEST_F(TestExactInferenceJTAlarm, thread_8) {
////    string pt_file = "../../dataset/alarm/alarm_1k_pt";
////    int num_threads = 8;
////
////    double accuracy = inference->EvaluateAccuracy(pt_file, num_threads);
////    cout << "accuracy = " << accuracy << endl;
////    SAFE_DELETE(inference);
////}
////
////TEST_F(TestExactInferenceJTAlarm, thread_16) {
////    string pt_file = "../../dataset/alarm/alarm_1k_pt";
////    int num_threads = 16;
////
////    double accuracy = inference->EvaluateAccuracy(pt_file, num_threads);
////    cout << "accuracy = " << accuracy << endl;
////    SAFE_DELETE(inference);
////}
////
////
////
////class TestExactInferenceJTHailfinder: public ::testing::Test { // TODO: acc = 0.347 for 1000 and 0.29 for 100
////protected:
////
////    void SetUp() override {
////        Dataset *tester = new Dataset();
////        CustomNetwork *network = new CustomNetwork(true);
////
////        string net_file = "../../dataset/hailfinder/hailfinder.xml";
////        string test_set_file = "../../dataset/hailfinder/testing_hailfinder_100_evi20";
////
////        network->GetNetFromXMLBIFFile(net_file);
////        tester->LoadLIBSVMDataKnownNetwork(test_set_file, network->num_nodes);
////
////        inference = new JunctionTree(false, network, tester, false);
//////        SAFE_DELETE(network);
////        SAFE_DELETE(tester);
////    }
////
////    Inference *inference;
////};
////
////TEST_F(TestExactInferenceJTHailfinder, thread_1) {
////    string pt_file = "../../dataset/hailfinder/hailfinder_100_pt";
////    int num_threads = 1;
////
////    double accuracy = inference->EvaluateAccuracy(pt_file, num_threads);
////    cout << "accuracy = " << accuracy << endl;
////    SAFE_DELETE(inference);
////}
////
////TEST_F(TestExactInferenceJTHailfinder, thread_2) {
////    string pt_file = "../../dataset/hailfinder/hailfinder_100_pt";
////    int num_threads = 2;
////
////    double accuracy = inference->EvaluateAccuracy(pt_file, num_threads);
////    cout << "accuracy = " << accuracy << endl;
////    SAFE_DELETE(inference);
////}
////
////TEST_F(TestExactInferenceJTHailfinder, thread_4) {
////    string pt_file = "../../dataset/hailfinder/hailfinder_100_pt";
////    int num_threads = 4;
////
////    double accuracy = inference->EvaluateAccuracy(pt_file, num_threads);
////    cout << "accuracy = " << accuracy << endl;
////    SAFE_DELETE(inference);
////}
////
////TEST_F(TestExactInferenceJTHailfinder, thread_8) {
////    string pt_file = "../../dataset/hailfinder/hailfinder_100_pt";
////    int num_threads = 8;
////
////    double accuracy = inference->EvaluateAccuracy(pt_file, num_threads);
////    cout << "accuracy = " << accuracy << endl;
////    SAFE_DELETE(inference);
////}
////
////TEST_F(TestExactInferenceJTHailfinder, thread_16) {
////    string pt_file = "../../dataset/hailfinder/hailfinder_100_pt";
////    int num_threads = 16;
////
////    double accuracy = inference->EvaluateAccuracy(pt_file, num_threads);
////    cout << "accuracy = " << accuracy << endl;
////    SAFE_DELETE(inference);
////}
////
////
////
////class TestExactInferenceJTPathfinder: public ::testing::Test { // TODO: acc=0.7
////protected:
////
////    void SetUp() override {
////        Dataset *tester = new Dataset();
////        CustomNetwork *network = new CustomNetwork(true);
////
////        string net_file = "../../dataset/pathfinder/pathfinder.xml";
////        string test_set_file = "../../dataset/pathfinder/testing_pathfinder_100_evi20";
////
////        network->GetNetFromXMLBIFFile(net_file);
////        tester->LoadLIBSVMDataKnownNetwork(test_set_file, network->num_nodes);
////
////        inference = new JunctionTree(false, network, tester, false);
//////        SAFE_DELETE(network);
////        SAFE_DELETE(tester);
////    }
////
////    Inference *inference;
////};
////
////TEST_F(TestExactInferenceJTPathfinder, thread_1) {
////    string pt_file = "../../dataset/pathfinder/pathfinder_100_pt";
////    int num_threads = 1;
////
////    double accuracy = inference->EvaluateAccuracy(pt_file, num_threads);
////    cout << "accuracy = " << accuracy << endl;
////    SAFE_DELETE(inference);
////}
////
////TEST_F(TestExactInferenceJTPathfinder, thread_2) {
////    string pt_file = "../../dataset/pathfinder/pathfinder_100_pt";
////    int num_threads = 2;
////
////    double accuracy = inference->EvaluateAccuracy(pt_file, num_threads);
////    cout << "accuracy = " << accuracy << endl;
////    SAFE_DELETE(inference);
////}
////
////TEST_F(TestExactInferenceJTPathfinder, thread_4) {
////    string pt_file = "../../dataset/pathfinder/pathfinder_100_pt";
////    int num_threads = 4;
////
////    double accuracy = inference->EvaluateAccuracy(pt_file, num_threads);
////    cout << "accuracy = " << accuracy << endl;
////    SAFE_DELETE(inference);
////}
////
////TEST_F(TestExactInferenceJTPathfinder, thread_8) {
////    string pt_file = "../../dataset/pathfinder/pathfinder_100_pt";
////    int num_threads = 8;
////
////    double accuracy = inference->EvaluateAccuracy(pt_file, num_threads);
////    cout << "accuracy = " << accuracy << endl;
////    SAFE_DELETE(inference);
////}
////
////TEST_F(TestExactInferenceJTPathfinder, thread_16) {
////    string pt_file = "../../dataset/pathfinder/pathfinder_100_pt";
////    int num_threads = 16;
////
////    double accuracy = inference->EvaluateAccuracy(pt_file, num_threads);
////    cout << "accuracy = " << accuracy << endl;
////    SAFE_DELETE(inference);
////}
//
//
//
////class TestExactInferenceJTDiabetes: public ::testing::Test { // TODO: acc=0.86
////protected:
////
////    void SetUp() override {
////        Dataset *tester = new Dataset();
////        CustomNetwork *network = new CustomNetwork(true);
////
////        string net_file = "../../dataset/diabetes/diabetes.xml";
////        string test_set_file = "../../dataset/diabetes/testing_diabetes_100_evi20";
////
////        network->GetNetFromXMLBIFFile(net_file);
////        tester->LoadLIBSVMDataKnownNetwork(test_set_file, network->num_nodes);
////
////        inference = new JunctionTree(false, network, tester, false);
//////        SAFE_DELETE(network);
////        SAFE_DELETE(tester);
////    }
////
////    Inference *inference;
////};
////
////TEST_F(TestExactInferenceJTDiabetes, thread_1) {
////    string pt_file = "../../dataset/diabetes/diabetes_100_pt";
////    int num_threads = 1;
////
////    double accuracy = inference->EvaluateAccuracy(pt_file, num_threads);
////    cout << "accuracy = " << accuracy << endl;
////    SAFE_DELETE(inference);
////}
////
////TEST_F(TestExactInferenceJTDiabetes, thread_2) {
////    string pt_file = "../../dataset/diabetes/diabetes_100_pt";
////    int num_threads = 2;
////
////    double accuracy = inference->EvaluateAccuracy(pt_file, num_threads);
////    cout << "accuracy = " << accuracy << endl;
////    SAFE_DELETE(inference);
////}
////
////TEST_F(TestExactInferenceJTDiabetes, thread_4) {
////    string pt_file = "../../dataset/diabetes/diabetes_100_pt";
////    int num_threads = 4;
////
////    double accuracy = inference->EvaluateAccuracy(pt_file, num_threads);
////    cout << "accuracy = " << accuracy << endl;
////    SAFE_DELETE(inference);
////}
////
////TEST_F(TestExactInferenceJTDiabetes, thread_8) {
////    string pt_file = "../../dataset/diabetes/diabetes_100_pt";
////    int num_threads = 8;
////
////    double accuracy = inference->EvaluateAccuracy(pt_file, num_threads);
////    cout << "accuracy = " << accuracy << endl;
////    SAFE_DELETE(inference);
////}
////
////TEST_F(TestExactInferenceJTDiabetes, thread_16) {
////    string pt_file = "../../dataset/diabetes/diabetes_100_pt";
////    int num_threads = 16;
////
////    double accuracy = inference->EvaluateAccuracy(pt_file, num_threads);
////    cout << "accuracy = " << accuracy << endl;
////    SAFE_DELETE(inference);
////}
//
//
//
////class TestExactInferenceJTPigs: public ::testing::Test { // TODO: acc=0.46
////protected:
////
////    void SetUp() override {
////        Dataset *tester = new Dataset();
////        CustomNetwork *network = new CustomNetwork(true);
////
////        string net_file = "../../dataset/pigs/pigs.xml";
////        string test_set_file = "../../dataset/pigs/testing_pigs_100_evi20";
////
////        network->GetNetFromXMLBIFFile(net_file);
////        tester->LoadLIBSVMDataKnownNetwork(test_set_file, network->num_nodes);
////
////        inference = new JunctionTree(false, network, tester, false);
//////        SAFE_DELETE(network);
////        SAFE_DELETE(tester);
////    }
////
////    Inference *inference;
////};
////
////TEST_F(TestExactInferenceJTPigs, thread_1) {
////    string pt_file = "../../dataset/pigs/pigs_100_pt";
////    int num_threads = 1;
////
////    double accuracy = inference->EvaluateAccuracy(pt_file, num_threads);
////    cout << "accuracy = " << accuracy << endl;
////    SAFE_DELETE(inference);
////}
////
////TEST_F(TestExactInferenceJTPigs, thread_2) {
////    string pt_file = "../../dataset/pigs/pigs_100_pt";
////    int num_threads = 2;
////
////    double accuracy = inference->EvaluateAccuracy(pt_file, num_threads);
////    cout << "accuracy = " << accuracy << endl;
////    SAFE_DELETE(inference);
////}
////
////TEST_F(TestExactInferenceJTPigs, thread_4) {
////    string pt_file = "../../dataset/pigs/pigs_100_pt";
////    int num_threads = 4;
////
////    double accuracy = inference->EvaluateAccuracy(pt_file, num_threads);
////    cout << "accuracy = " << accuracy << endl;
////    SAFE_DELETE(inference);
////}
////
////TEST_F(TestExactInferenceJTPigs, thread_8) {
////    string pt_file = "../../dataset/pigs/pigs_100_pt";
////    int num_threads = 8;
////
////    double accuracy = inference->EvaluateAccuracy(pt_file, num_threads);
////    cout << "accuracy = " << accuracy << endl;
////    SAFE_DELETE(inference);
////}
////
////TEST_F(TestExactInferenceJTPigs, thread_16) {
////    string pt_file = "../../dataset/pigs/pigs_100_pt";
////    int num_threads = 16;
////
////    double accuracy = inference->EvaluateAccuracy(pt_file, num_threads);
////    cout << "accuracy = " << accuracy << endl;
////    SAFE_DELETE(inference);
////}
////
////
////
////class TestExactInferenceJTMunin: public ::testing::Test { // TODO: acc=0.88
////protected:
////
////    void SetUp() override {
////        Dataset *tester = new Dataset();
////        CustomNetwork *network = new CustomNetwork(true);
////
////        string net_file = "../../dataset/munin/munin.xml";
////        string test_set_file = "../../dataset/munin/testing_munin_100_evi20";
////
////        network->GetNetFromXMLBIFFile(net_file);
////        tester->LoadLIBSVMDataKnownNetwork(test_set_file, network->num_nodes);
////
////        inference = new JunctionTree(false, network, tester, false);
//////        SAFE_DELETE(network);
////        SAFE_DELETE(tester);
////    }
////
////    Inference *inference;
////};
////
////TEST_F(TestExactInferenceJTMunin, thread_1) {
////    string pt_file = "../../dataset/munin/munin_100_pt";
////    int num_threads = 1;
////
////    double accuracy = inference->EvaluateAccuracy(pt_file, num_threads);
////    cout << "accuracy = " << accuracy << endl;
////    SAFE_DELETE(inference);
////}
////
////TEST_F(TestExactInferenceJTMunin, thread_2) {
////    string pt_file = "../../dataset/munin/munin_100_pt";
////    int num_threads = 2;
////
////    double accuracy = inference->EvaluateAccuracy(pt_file, num_threads);
////    cout << "accuracy = " << accuracy << endl;
////    SAFE_DELETE(inference);
////}
////
////TEST_F(TestExactInferenceJTMunin, thread_4) {
////    string pt_file = "../../dataset/munin/munin_100_pt";
////    int num_threads = 4;
////
////    double accuracy = inference->EvaluateAccuracy(pt_file, num_threads);
////    cout << "accuracy = " << accuracy << endl;
////    SAFE_DELETE(inference);
////}
////
////TEST_F(TestExactInferenceJTMunin, thread_8) {
////    string pt_file = "../../dataset/munin/munin_100_pt";
////    int num_threads = 8;
////
////    double accuracy = inference->EvaluateAccuracy(pt_file, num_threads);
////    cout << "accuracy = " << accuracy << endl;
////    SAFE_DELETE(inference);
////}
////
////TEST_F(TestExactInferenceJTMunin, thread_16) {
////    string pt_file = "../../dataset/munin/munin_100_pt";
////    int num_threads = 16;
////
////    double accuracy = inference->EvaluateAccuracy(pt_file, num_threads);
////    cout << "accuracy = " << accuracy << endl;
////    SAFE_DELETE(inference);
////}
////
////
////
////class TestExactInferenceJTMunin4: public ::testing::Test { // TODO: acc=0.39
////protected:
////
////    void SetUp() override {
////        Dataset *tester = new Dataset();
////        CustomNetwork *network = new CustomNetwork(true);
////
////        string net_file = "../../dataset/munin4/munin4.xml";
////        string test_set_file = "../../dataset/munin4/testing_munin4_100_evi20";
////
////        network->GetNetFromXMLBIFFile(net_file);
////        tester->LoadLIBSVMDataKnownNetwork(test_set_file, network->num_nodes);
////
////        inference = new JunctionTree(false, network, tester, false);
//////        SAFE_DELETE(network);
////        SAFE_DELETE(tester);
////    }
////
////    Inference *inference;
////};
////
////TEST_F(TestExactInferenceJTMunin4, thread_1) {
////    string pt_file = "../../dataset/munin4/munin4_100_pt";
////    int num_threads = 1;
////
////    double accuracy = inference->EvaluateAccuracy(pt_file, num_threads);
////    cout << "accuracy = " << accuracy << endl;
////    SAFE_DELETE(inference);
////}
////
////TEST_F(TestExactInferenceJTMunin4, thread_2) {
////    string pt_file = "../../dataset/munin4/munin4_100_pt";
////    int num_threads = 2;
////
////    double accuracy = inference->EvaluateAccuracy(pt_file, num_threads);
////    cout << "accuracy = " << accuracy << endl;
////    SAFE_DELETE(inference);
////}
////
////TEST_F(TestExactInferenceJTMunin4, thread_4) {
////    string pt_file = "../../dataset/munin4/munin4_100_pt";
////    int num_threads = 4;
////
////    double accuracy = inference->EvaluateAccuracy(pt_file, num_threads);
////    cout << "accuracy = " << accuracy << endl;
////    SAFE_DELETE(inference);
////}
////
////TEST_F(TestExactInferenceJTMunin4, thread_8) {
////    string pt_file = "../../dataset/munin4/munin4_100_pt";
////    int num_threads = 8;
////
////    double accuracy = inference->EvaluateAccuracy(pt_file, num_threads);
////    cout << "accuracy = " << accuracy << endl;
////    SAFE_DELETE(inference);
////}
////
////TEST_F(TestExactInferenceJTMunin4, thread_16) {
////    string pt_file = "../../dataset/munin4/munin4_100_pt";
////    int num_threads = 16;
////
////    double accuracy = inference->EvaluateAccuracy(pt_file, num_threads);
////    cout << "accuracy = " << accuracy << endl;
////    SAFE_DELETE(inference);
////}
