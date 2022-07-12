//
// Created by jjt on 2022/4/6.
//

#include <iostream>
#include <vector>
#include "gtest/gtest.h"

#include "Dataset.h"
#include "CustomNetwork.h"
#include "Inference.h"
#include "JunctionTree.h"

//string path = "/Users/jjt/work/research_project/BN/inference/dataset/BN/";
//string path = "/home/zeyiwen/jiantong/BN/dataset/";
//string path = "/home/ubuntu/BN/dataset/";
string path = "/hpc/users/HKUST-GZ/wenzeyi/jjt/BN/dataset/";

class InferenceAlarm: public ::testing::Test {
protected:

    void SetUp() override {
        tester = new Dataset();
        network = new CustomNetwork(true);

        string net_file_path = path + "alarm/alarm.xml";
        string test_set_file_path = path + "alarm/alarm_2w_p20";

        network->GetNetFromXMLBIFFile(net_file_path);
        tester->LoadLIBSVMDataKnownNetwork(test_set_file_path, network->num_nodes);
    }
    CustomNetwork *network;
    Dataset *tester;
};

//TEST_F(InferenceAlarm, junction_tree_partial1) {
//    Inference *inference = new JunctionTree(network);
//    double accuracy = inference->EvaluateAccuracy(tester, 1, -1, "jt", false);
//    delete inference;
//    delete network;
//     delete tester;
//    EXPECT_EQ(accuracy, 0.5024);
//}

TEST_F(InferenceAlarm, junction_tree_partial2) {
    Inference *inference = new JunctionTree(network);
    double accuracy = inference->EvaluateAccuracy(tester, 2, -1, "jt", false);
    delete inference;
    delete network;
    delete tester;
    EXPECT_EQ(accuracy, 0.5024);
}

//TEST_F(InferenceAlarm, junction_tree_partial4) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 4, -1, "jt", false);
//delete inference;
//delete network;
//delete tester;
//    EXPECT_EQ(accuracy, 0.5024);
//}
//
//TEST_F(InferenceAlarm, junction_tree_partial6) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 6, -1, "jt", false);
//delete inference;
//delete network;
//delete tester;
//EXPECT_EQ(accuracy, 0.5024);
//}
//
//TEST_F(InferenceAlarm, junction_tree_partial8) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 8, -1, "jt", false);
//delete inference;
//delete network;
//delete tester;
//EXPECT_EQ(accuracy, 0.5024);
//}
//
//TEST_F(InferenceAlarm, junction_tree_partial10) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 10, -1, "jt", false);
//delete inference;
//delete network;
//delete tester;
//EXPECT_EQ(accuracy, 0.5024);
//}




//class InferenceHailfinder: public ::testing::Test {
//protected:
//
//    void SetUp() override {
//        tester = new Dataset();
//        network = new CustomNetwork(true);
//
//        string net_file_path = path + "hailfinder/hailfinder.xml";
//        string test_set_file_path = path + "hailfinder/hailfinder_2w_p20";
//
//        network->GetNetFromXMLBIFFile(net_file_path);
//        tester->LoadLIBSVMDataKnownNetwork(test_set_file_path, network->num_nodes);
//    }
//    CustomNetwork *network;
//    Dataset *tester;
//};
//
//TEST_F(InferenceHailfinder, junction_tree_partial1) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 1, -1, "jt", false);
//delete inference;
//delete network;
//delete tester;
//EXPECT_GT(accuracy, 0.8230);
//}
//
//TEST_F(InferenceHailfinder, junction_tree_partial2) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 2, -1, "jt", false);
//delete inference;
//delete network;
//delete tester;
//EXPECT_GT(accuracy, 0.8230);
//}
//
//TEST_F(InferenceHailfinder, junction_tree_partial4) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 4, -1, "jt", false);
//delete inference;
//delete network;
//delete tester;
//EXPECT_GT(accuracy, 0.8230);
//}
//
//TEST_F(InferenceHailfinder, junction_tree_partial6) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 6, -1, "jt", false);
//delete inference;
//delete network;
//delete tester;
//EXPECT_GT(accuracy, 0.8230);
//}
//
//TEST_F(InferenceHailfinder, junction_tree_partial8) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 8, -1, "jt", false);
//delete inference;
//delete network;
//delete tester;
//EXPECT_GT(accuracy, 0.8230);
//}
//
//TEST_F(InferenceHailfinder, junction_tree_partial10) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 10, -1, "jt", false);
//delete inference;
//delete network;
//delete tester;
//EXPECT_GT(accuracy, 0.8230);
//}
//
//
//
//
//class InferenceHepar2: public ::testing::Test {
//protected:
//
//    void SetUp() override {
//        tester = new Dataset();
//        network = new CustomNetwork(true);
//
//        string net_file_path = path + "hepar2/hepar2.xml";
//        string test_set_file_path = path + "hepar2/hepar2_2w_p20";
//
//        network->GetNetFromXMLBIFFile(net_file_path);
//        tester->LoadLIBSVMDataKnownNetwork(test_set_file_path, network->num_nodes);
//    }
//    CustomNetwork *network;
//    Dataset *tester;
//};
//
//TEST_F(InferenceHepar2, junction_tree_partial1) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 1, -1, "jt", false);
//delete inference;
//delete network;
//delete tester;
//EXPECT_GT(accuracy, 0.8230);
//}
//
//TEST_F(InferenceHepar2, junction_tree_partial2) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 2, -1, "jt", false);
//delete inference;
//delete network;
//delete tester;
//EXPECT_GT(accuracy, 0.8230);
//}
//
//TEST_F(InferenceHepar2, junction_tree_partial4) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 4, -1, "jt", false);
//delete inference;
//delete network;
//delete tester;
//EXPECT_GT(accuracy, 0.8230);
//}
//
//TEST_F(InferenceHepar2, junction_tree_partial6) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 6, -1, "jt", false);
//delete inference;
//delete network;
//delete tester;
//EXPECT_GT(accuracy, 0.8230);
//}
//
//TEST_F(InferenceHepar2, junction_tree_partial8) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 8, -1, "jt", false);
//delete inference;
//delete network;
//delete tester;
//EXPECT_GT(accuracy, 0.8230);
//}
//
//TEST_F(InferenceHepar2, junction_tree_partial10) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 10, -1, "jt", false);
//delete inference;
//delete network;
//delete tester;
//EXPECT_GT(accuracy, 0.8230);
//}
//
//
//class InferenceWin95pts: public ::testing::Test {
//protected:
//
//    void SetUp() override {
//        tester = new Dataset();
//        network = new CustomNetwork(true);
//
//        string net_file_path = path + "win95pts/win95pts.xml";
//        string test_set_file_path = path + "win95pts/win95pts_2w_p20";
//
//        network->GetNetFromXMLBIFFile(net_file_path);
//        tester->LoadLIBSVMDataKnownNetwork(test_set_file_path, network->num_nodes);
//    }
//    CustomNetwork *network;
//    Dataset *tester;
//};
//
//TEST_F(InferenceWin95pts, junction_tree_partial1) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 1, -1, "jt", false);
//delete inference;
//delete network;
//delete tester;
//EXPECT_GT(accuracy, 0.8230);
//}
//
//TEST_F(InferenceWin95pts, junction_tree_partial2) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 2, -1, "jt", false);
//delete inference;
//delete network;
//delete tester;
//EXPECT_GT(accuracy, 0.8230);
//}
//
//TEST_F(InferenceWin95pts, junction_tree_partial4) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 4, -1, "jt", false);
//delete inference;
//delete network;
//delete tester;
//EXPECT_GT(accuracy, 0.8230);
//}
//
//TEST_F(InferenceWin95pts, junction_tree_partial6) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 6, -1, "jt", false);
//delete inference;
//delete network;
//delete tester;
//EXPECT_GT(accuracy, 0.8230);
//}
//
//TEST_F(InferenceWin95pts, junction_tree_partial8) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 8, -1, "jt", false);
//delete inference;
//delete network;
//delete tester;
//EXPECT_GT(accuracy, 0.8230);
//}
//
//TEST_F(InferenceWin95pts, junction_tree_partial10) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 10, -1, "jt", false);
//delete inference;
//delete network;
//delete tester;
//EXPECT_GT(accuracy, 0.8230);
//}



class InferencePathfinder: public ::testing::Test {
protected:

    void SetUp() override {
        tester = new Dataset();
        network = new CustomNetwork(true);

        string net_file_path = path + "pathfinder/pathfinder.xml";
        string test_set_file_path = path + "pathfinder/pathfinder_2w_p20";

        network->GetNetFromXMLBIFFile(net_file_path);
        tester->LoadLIBSVMDataKnownNetwork(test_set_file_path, network->num_nodes);
    }
    CustomNetwork *network;
    Dataset *tester;
};

TEST_F(InferencePathfinder, junction_tree_partial1) {
Inference *inference = new JunctionTree(network);
double accuracy = inference->EvaluateAccuracy(tester, 1, -1, "jt", false);
delete inference;
delete network;
delete tester;
    EXPECT_EQ(accuracy, 0.01605);
}

TEST_F(InferencePathfinder, junction_tree_partial2) {
Inference *inference = new JunctionTree(network);
double accuracy = inference->EvaluateAccuracy(tester, 2, -1, "jt", false);
delete inference;
delete network;
delete tester;
    EXPECT_EQ(accuracy, 0.01605);
}

TEST_F(InferencePathfinder, junction_tree_partial4) {
Inference *inference = new JunctionTree(network);
double accuracy = inference->EvaluateAccuracy(tester, 4, -1, "jt", false);
delete inference;
delete network;
delete tester;
EXPECT_EQ(accuracy, 0.01605);
}

TEST_F(InferencePathfinder, junction_tree_partial6) {
Inference *inference = new JunctionTree(network);
double accuracy = inference->EvaluateAccuracy(tester, 6, -1, "jt", false);
delete inference;
delete network;
delete tester;
EXPECT_EQ(accuracy, 0.01605);
}

TEST_F(InferencePathfinder, junction_tree_partial8) {
Inference *inference = new JunctionTree(network);
double accuracy = inference->EvaluateAccuracy(tester, 8, -1, "jt", false);
delete inference;
delete network;
delete tester;
EXPECT_EQ(accuracy, 0.01605);
}

TEST_F(InferencePathfinder, junction_tree_partial10) {
Inference *inference = new JunctionTree(network);
double accuracy = inference->EvaluateAccuracy(tester, 10, -1, "jt", false);
delete inference;
delete network;
delete tester;
EXPECT_EQ(accuracy, 0.01605);
}




//class InferenceDiabetes: public ::testing::Test {
//protected:
//
//    void SetUp() override {
//        tester = new Dataset();
//        network = new CustomNetwork(true);
//
//        string net_file_path = path + "diabetes/diabetes.xml";
//        string test_set_file_path = path + "diabetes/diabetes_2w_p20";
//
//        network->GetNetFromXMLBIFFile(net_file_path);
//        tester->LoadLIBSVMDataKnownNetwork(test_set_file_path, network->num_nodes);
//    }
//    CustomNetwork *network;
//    Dataset *tester;
//};
//
//TEST_F(InferenceDiabetes, junction_tree_partial1) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 1, -1, "jt", false);
//delete inference;
//delete network;
//delete tester;
//EXPECT_GT(accuracy, 0.4);
//}
//
//TEST_F(InferenceDiabetes, junction_tree_partial2) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 2, -1, "jt", false);
//delete inference;
//delete network;
//delete tester;
//EXPECT_GT(accuracy, 0.4);
//}
//
//TEST_F(InferenceDiabetes, junction_tree_partial4) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 4, -1, "jt", false);
//delete inference;
//delete network;
//delete tester;
//EXPECT_GT(accuracy, 0.4);
//}
//
//TEST_F(InferenceDiabetes, junction_tree_partial6) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 6, -1, "jt", false);
//delete inference;
//delete network;
//delete tester;
//EXPECT_GT(accuracy, 0.4);
//}
//
//TEST_F(InferenceDiabetes, junction_tree_partial8) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 8, -1, "jt", false);
//delete inference;
//delete network;
//delete tester;
//EXPECT_GT(accuracy, 0.4);
//}
//
//TEST_F(InferenceDiabetes, junction_tree_partial10) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 10, -1, "jt", false);
//delete inference;
//delete network;
//delete tester;
//EXPECT_GT(accuracy, 0.4);
//}
//
//
//
//class InferencePigs: public ::testing::Test {
//protected:
//
//    void SetUp() override {
//        tester = new Dataset();
//        network = new CustomNetwork(true);
//
//        string net_file_path = path + "pigs/pigs.xml";
//        string test_set_file_path = path + "pigs/pigs_2w_p20";
//
//        network->GetNetFromXMLBIFFile(net_file_path);
//        tester->LoadLIBSVMDataKnownNetwork(test_set_file_path, network->num_nodes);
//    }
//    CustomNetwork *network;
//    Dataset *tester;
//};
//
//TEST_F(InferencePigs, junction_tree_partial1) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 1, -1, "jt", false);
//delete inference;
//delete network;
//delete tester;
//EXPECT_GT(accuracy, 0.8230);
//}
//
//TEST_F(InferencePigs, junction_tree_partial2) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 2, -1, "jt", false);
//delete inference;
//delete network;
//delete tester;
//EXPECT_GT(accuracy, 0.8230);
//}
//
//TEST_F(InferencePigs, junction_tree_partial4) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 4, -1, "jt", false);
//delete inference;
//delete network;
//delete tester;
//EXPECT_GT(accuracy, 0.8230);
//}
//
//TEST_F(InferencePigs, junction_tree_partial6) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 6, -1, "jt", false);
//delete inference;
//delete network;
//delete tester;
//EXPECT_GT(accuracy, 0.8230);
//}
//
//TEST_F(InferencePigs, junction_tree_partial8) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 8, -1, "jt", false);
//delete inference;
//delete network;
//delete tester;
//EXPECT_GT(accuracy, 0.8230);
//}
//
//TEST_F(InferencePigs, junction_tree_partial10) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 10, -1, "jt", false);
//delete inference;
//delete network;
//delete tester;
//EXPECT_GT(accuracy, 0.8230);
//}
//
//
//
//
//
//
//class InferenceLink: public ::testing::Test {
//protected:
//
//    void SetUp() override {
//        tester = new Dataset();
//        network = new CustomNetwork(true);
//
//        string net_file_path = "/home/zeyiwen/jiantong/BN/dataset/link/link.xml";
//        string test_set_file_path = "/home/zeyiwen/jiantong/BN/dataset/link/link_2w_p20";
//
//        network->GetNetFromXMLBIFFile(net_file_path);
//        tester->LoadLIBSVMDataKnownNetwork(test_set_file_path, network->num_nodes);
//    }
//    CustomNetwork *network;
//    Dataset *tester;
//};
//
//TEST_F(InferenceLink, junction_tree_partial1) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 1, -1, "jt", false);
//delete inference;
//delete network;
//delete tester;
//EXPECT_GT(accuracy, 0.8230);
//}
//
//TEST_F(InferenceLink, junction_tree_partial2) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 2, -1, "jt", false);
//delete inference;
//delete network;
//delete tester;
//EXPECT_GT(accuracy, 0.8230);
//}
//
//TEST_F(InferenceLink, junction_tree_partial4) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 4, -1, "jt", false);
//delete inference;
//delete network;
//delete tester;
//EXPECT_GT(accuracy, 0.8230);
//}
//
//TEST_F(InferenceLink, junction_tree_partial6) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 6, -1, "jt", false);
//delete inference;
//delete network;
//delete tester;
//EXPECT_GT(accuracy, 0.8230);
//}
//
//TEST_F(InferenceLink, junction_tree_partial8) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 8, -1, "jt", false);
//delete inference;
//delete network;
//delete tester;
//EXPECT_GT(accuracy, 0.8230);
//}
//
//TEST_F(InferenceLink, junction_tree_partial10) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 10, -1, "jt", false);
//delete inference;
//delete network;
//delete tester;
//EXPECT_GT(accuracy, 0.8230);
//}
//
//
//
//
//class InferenceMunin1: public ::testing::Test {
//protected:
//
//    void SetUp() override {
//        tester = new Dataset();
//        network = new CustomNetwork(true);
//
//        string net_file_path = "/home/zeyiwen/jiantong/BN/dataset/munin1/munin1.xml";
//        string test_set_file_path = "/home/zeyiwen/jiantong/BN/dataset/munin1/munin1_2w_p20";
//
//        network->GetNetFromXMLBIFFile(net_file_path);
//        tester->LoadLIBSVMDataKnownNetwork(test_set_file_path, network->num_nodes);
//    }
//    CustomNetwork *network;
//    Dataset *tester;
//};
//
//TEST_F(InferenceMunin1, junction_tree_partial1) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 1, -1, "jt", false);
//delete inference;
//delete network;
//delete tester;
//EXPECT_GT(accuracy, 0.49);
//}
//
//TEST_F(InferenceMunin1, junction_tree_partial2) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 2, -1, "jt", false);
//delete inference;
//delete network;
//delete tester;
//EXPECT_GT(accuracy, 0.49);
//}
//
//TEST_F(InferenceMunin1, junction_tree_partial4) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 4, -1, "jt", false);
//delete inference;
//delete network;
//delete tester;
//EXPECT_GT(accuracy, 0.49);
//}
//
//TEST_F(InferenceMunin1, junction_tree_partial6) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 6, -1, "jt", false);
//delete inference;
//delete network;
//delete tester;
//EXPECT_GT(accuracy, 0.49);
//}
//
//TEST_F(InferenceMunin1, junction_tree_partial8) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 8, -1, "jt", false);
//delete inference;
//delete network;
//delete tester;
//EXPECT_GT(accuracy, 0.49);
//}
//
//TEST_F(InferenceMunin1, junction_tree_partial10) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 10, -1, "jt", false);
//delete inference;
//delete network;
//delete tester;
//EXPECT_GT(accuracy, 0.49);
//}






//
//class InferenceMunin2: public ::testing::Test {
//protected:
//
//    void SetUp() override {
//        tester = new Dataset();
//        network = new CustomNetwork(true);
//
//        string net_file_path = "/home/zeyiwen/jiantong/BN/dataset/munin2/munin2.xml";
//        string test_set_file_path = "/home/zeyiwen/jiantong/BN/dataset/munin2/munin2_cut";
//
//        network->GetNetFromXMLBIFFile(net_file_path);
//        tester->LoadLIBSVMDataKnownNetwork(test_set_file_path, network->num_nodes);
//    }
//    CustomNetwork *network;
//    Dataset *tester;
//};
//
//TEST_F(InferenceMunin2, junction_tree_partial1) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 1, -1, "jt", false);
//delete inference;
//delete network;
//delete tester;
//EXPECT_GT(accuracy, 0.8230);
//}

//class InferenceMunin3: public ::testing::Test {
//protected:
//
//    void SetUp() override {
//        tester = new Dataset();
//        network = new CustomNetwork(true);
//
//        string net_file_path = "/home/zeyiwen/jiantong/BN/dataset/munin3/munin3.xml";
//        string test_set_file_path = "/home/zeyiwen/jiantong/BN/dataset/munin3/munin3_2w_p20";
//
//        network->GetNetFromXMLBIFFile(net_file_path);
//        tester->LoadLIBSVMDataKnownNetwork(test_set_file_path, network->num_nodes);
//    }
//    CustomNetwork *network;
//    Dataset *tester;
//};
//
//TEST_F(InferenceMunin3, junction_tree_partial1) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 1, -1, "jt", false);
//delete inference;
//delete network;
//delete tester;
//EXPECT_GT(accuracy, 0.8230);
//}
//
//class InferenceMunin4: public ::testing::Test {
//protected:
//
//    void SetUp() override {
//        tester = new Dataset();
//        network = new CustomNetwork(true);
//
//        string net_file_path = "/home/zeyiwen/jiantong/BN/dataset/munin4/munin4.xml";
//        string test_set_file_path = "/home/zeyiwen/jiantong/BN/dataset/munin4/munin4_2w_p20";
//
//        network->GetNetFromXMLBIFFile(net_file_path);
//        tester->LoadLIBSVMDataKnownNetwork(test_set_file_path, network->num_nodes);
//    }
//    CustomNetwork *network;
//    Dataset *tester;
//};
//
//TEST_F(InferenceMunin4, junction_tree_partial1) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 1, -1, "jt", false);
//delete inference;
//delete network;
//delete tester;
//EXPECT_GT(accuracy, 0.8230);
//}
//


