////
//// Created by jjt on 25/11/23.
////
//#include <iostream>
//#include <string>
//#include <vector>
//#include "gtest/gtest.h"
//
//#include "fastbn/Dataset.h"
//#include "fastbn/CustomNetwork.h"
//#include "fastbn/test_common.h"
//
//
//class TestLibsvmToCSV : public ::testing::Test {
//protected:
//
//    void SetUp() override {
//        CustomNetwork *network = new CustomNetwork(true);
//        string net_file = dataset_path + "alarm/alarm.xml";
//        network->LoadXMLBIFFile(net_file, ALPHA) ;
//
//        // if the wrong old libsvm format is used, we need to do the changes in `LoadLIBSVMTestingData`.
//        dts = new Dataset(network);
//        string test_set_file = dataset_path + "alarm/testing_alarm_1k_p20";
//        dts->LoadLIBSVMTestingData(test_set_file, 0);
//
//        // for `vec_var_names`.
//        dts->vec_var_names.resize(dts->num_vars);
//        for (int i = 0; i < dts->num_vars; ++i) {
//            Node *node = network->FindNodePtrByIndex(i);
//            dts->vec_var_names[i] = node->node_name;
//        }
//    }
//
//    Dataset *dts;
//};
//
//TEST_F(TestLibsvmToCSV, alarm) {
//    string csv_file = dataset_path + "alarm/testing_alarm_1k_p20_csv.txt";
//    dts->StoreCSVData(csv_file, true);
//    SAFE_DELETE(dts);
//}
//
//
////class TestCSVToLibsvm : public ::testing::Test {
////protected:
////
////    void SetUp() override {
////        trainer = new Dataset();
////        string train_set_file = dataset_path + "alarm/alarm_s5000.txt";
////        trainer->LoadCSVTrainingData(train_set_file, true, 0);
////
////        tester = new Dataset(trainer);
////        string test_set_file = dataset_path + "alarm/alarm_s5000.txt";
////        tester->LoadCSVTestingData(test_set_file, true, 0);
////    }
////
////    Dataset *trainer;
////    Dataset *tester;
////};
////
////TEST_F(TestCSVToLibsvm, alarm) {
////    tester->vec_var_names = trainer->vec_var_names;
////
////    string libsvm_file = dataset_path + "alarm/test4.txt";
////    string relation_file = dataset_path + "alarm/relation.txt";
////    tester->StoreLIBSVMData(libsvm_file, relation_file);
////
////    SAFE_DELETE(trainer);
////    SAFE_DELETE(tester);
////}
//
