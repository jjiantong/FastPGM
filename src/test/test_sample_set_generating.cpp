////
//// Created by jjt on 25/10/23.
////
//#include <iostream>
//#include <string>
//#include <vector>
//#include "gtest/gtest.h"
//
//#include "fastbn/CustomNetwork.h"
//#include "fastbn/test_common.h"
//#include "fastbn/SampleSetGenerator.h"
//#include "fastbn/Dataset.h"
//
//
//class TestSampleSet: public ::testing::Test {
//protected:
//
//    void SetUp() override {
//        network = new CustomNetwork(true);
//    }
//    CustomNetwork *network;
//};
//
////TEST_F(TestSampleSet, alarm_libsvm) {
////    string net_file = dataset_path + "alarm/alarm.xml";
////    network->LoadXMLBIFFile(net_file, ALPHA);
////
////    SampleSetGenerator *sample_set = new SampleSetGenerator(network, 1000, 0);
////    sample_set->GenerateSamplesBasedOnCPTs();
////    sample_set->OutputLIBSVM(dataset_path + "alarm/alarm_training_libsvm");
////
////    SAFE_DELETE(sample_set);
////    SAFE_DELETE(network);
////}
//
//TEST_F(TestSampleSet, alarm_csv) {
//    string net_file = dataset_path + "alarm/alarm.xml";
//    network->LoadXMLBIFFile(net_file, ALPHA);
//
//    SampleSetGenerator *sample_set = new SampleSetGenerator(network, 1000, 0);
//    sample_set->GenerateSamplesBasedOnCPTs();
//    sample_set->OutputLIBSVM(dataset_path + "alarm/alarm_training_libsvm");
//    SAFE_DELETE(sample_set);
//
//    Dataset *dts = new Dataset(network);
//    dts->LoadLIBSVMTestingData(dataset_path + "alarm/alarm_training_libsvm", 0);
//
//    dts->vec_var_names.resize(dts->num_vars);
//    for (int i = 0; i < dts->num_vars; ++i) {
//        Node *node = network->FindNodePtrByIndex(i);
//        dts->vec_var_names[i] = node->node_name;
//    }
//    SAFE_DELETE(network);
//
//    string csv_file = dataset_path + "alarm/alarm_training_csv";
//    dts->StoreCSVData(csv_file, true);
//    SAFE_DELETE(dts);
//}
//
