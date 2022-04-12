#include <iostream>
#include <string>
#include <vector>
#include "gtest/gtest.h"
#include "tinyxml2.h"

#include "Dataset.h"
#include "Network.h"
#include "CustomNetwork.h"
#include "BNSLComparison.h"
#include "Inference.h"
#include "BruteForce.h"
#include "VariableElimination.h"
#include "ApproximateInference.h"
#include "StructureLearning.h"
#include "ParameterLearning.h"
#include "ChowLiuTree.h"
#include "PCStable.h"
#include "K2.h"
#include "Ott.h"
#include "JunctionTree.h"
#include "CustomNetwork.h"
#include "ScoreFunction.h"
#include "gadget.h"

//string DATA_PATH = "/Users/jjt/work/research_project/BN"; // local
string DATA_PATH = "/home/zeyiwen/jiantong/BN"; // NUS
//string DATA_PATH = "/home/ubuntu"; // Oracle

//class ExperimentBNSLOnAlarm : public ::testing::Test {
//protected:
//
//    void SetUp() override {
//        trainer = new Dataset();
//        network = new Network(true);
//        string train_set_file_path = DATA_PATH + "/dataset/alarm/alarm_s5000.txt";
//        trainer->LoadCSVData(train_set_file_path, true, true, 0);
//    }
//    Dataset *trainer;
//    Network *network;
//};
//
//TEST_F(ExperimentBNSLOnAlarm, pc_stable) {
//    StructureLearning *bnsl = new PCStable(network, 0.05);
//    bnsl->StructLearnCompData(trainer, 1, 1, true, false);
//    delete trainer;




class ExperimentOnA1a : public ::testing::Test {
protected:

    void SetUp() override {
        trainer = new Dataset();
        tester = new Dataset();
//        network = new ChowLiuTree(true);
        network = new Network(true);

        string train_set_file_path = DATA_PATH + "/Bayesian-network/data/dataset/a1a.txt",//"../../data/dataset/a1a.txt",
                test_set_file_path = DATA_PATH + "/Bayesian-network/data/dataset/a1a.test.txt";//"../../data/dataset/a1a.test.txt";

        trainer->LoadLIBSVMData(train_set_file_path);
        tester->LoadLIBSVMData(test_set_file_path);

//        network->StructLearnCompData(trainer, true);
        StructureLearning *bnsl = new ChowLiuTree(network);
        bnsl->StructLearnCompData(trainer, 1, 1, false, false);
        ParameterLearning *bnpl = new ParameterLearning(network);
        bnpl->LearnParamsKnowStructCompData(trainer, 2, false);
    }

    Dataset *trainer;
    Dataset *tester;
    Network *network;
};

//TEST_F(ExperimentOnA1a, brute_force) {
//    Inference *inference = new BruteForce(network);
//    double accuracy = inference->EvaluateAccuracy(tester, -1, "direct", true);
//    delete inference;
//    EXPECT_GT(accuracy, 0.8230);
//}

//TEST_F(ExperimentOnA1a, ve) {
//    Inference *inference = new VariableElimination(network);
//    double accuracy = inference->EvaluateAccuracy(tester, -1, "ve", true);
//    delete inference;
//    EXPECT_GT(accuracy, 0.8230);
//}
//
//TEST_F(ExperimentOnA1a, ve_partial) {
//    Inference *inference = new VariableElimination(network);
//    double accuracy = inference->EvaluateAccuracy(tester, -1, "ve", false);
//    delete inference;
//    EXPECT_GT(accuracy, 0.8230);
//}

//TEST_F(ExperimentOnA1a, junction_tree) {
//    Inference *inference = new JunctionTree(network);
//    double accuracy = inference->EvaluateAccuracy(tester, -1, "jt", true);
//    delete inference;
//    EXPECT_GT(accuracy, 0.8230);
//}

TEST_F(ExperimentOnA1a, junction_tree_partial1) {
    Inference *inference = new JunctionTree(network);
    double accuracy = inference->EvaluateAccuracy(tester, 1, -1, "jt", false);
    delete inference;
    EXPECT_GT(accuracy, 0.8230);
}
//TEST_F(ExperimentOnA1a, junction_tree_partial2) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 2, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnA1a, junction_tree_partial3) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 3, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnA1a, junction_tree_partial4) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 4, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnA1a, junction_tree_partial5) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 5, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnA1a, junction_tree_partial6) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 6, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnA1a, junction_tree_partial7) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 7, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnA1a, junction_tree_partial8) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 8, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnA1a, junction_tree_partial9) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 9, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnA1a, junction_tree_partial10) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 10, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnA1a, junction_tree_partial11) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 11, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnA1a, junction_tree_partial12) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 12, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnA1a, junction_tree_partial13) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 13, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnA1a, junction_tree_partial14) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 14, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnA1a, junction_tree_partial15) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 15, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnA1a, junction_tree_partial16) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 16, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}


//
//TEST_F(ExperimentOnA1a, likelihood_weighing) {
//    Inference *inference = new ApproximateInference(network);
//    double accuracy = inference->EvaluateAccuracy(tester, 50, "likelihood", true);
//    EXPECT_GT(accuracy, 0.8150);
//}
//
//TEST_F(ExperimentOnA1a, approx) {
//    Inference *inference = new ApproximateInference(network);
//    double accuracy = inference->EvaluateAccuracy(tester, 50, "emm", true);
//    EXPECT_GT(accuracy, 0.650);
//}
//

//class ExperimentOnDNA : public ::testing::Test {
//protected:
//
//    void SetUp() override {
//        trainer = new Dataset();
//        tester = new Dataset();
//        network = new Network(true);
//
//        string train_set_file_path = DATA_PATH + "/dataset/dna.scale",
//                test_set_file_path = DATA_PATH + "/dataset/dna.scale.t";
//
//        trainer->LoadLIBSVMData(train_set_file_path);
//        tester->LoadLIBSVMData(test_set_file_path);
//
//        StructureLearning *bnsl = new ChowLiuTree(network);
//        bnsl->StructLearnCompData(trainer, 1, 1, true, false);
//        ParameterLearning *bnpl = new ParameterLearning(network);
//        bnpl->LearnParamsKnowStructCompData(trainer, 2, true);
//    }
//
//    Dataset *trainer;
//    Dataset *tester;
//    Network *network;
//};
//
//TEST_F(ExperimentOnDNA, junction_tree_partial) {
//    Inference *inference = new JunctionTree(network);
//    double accuracy = inference->EvaluateAccuracy(tester, -1, "jt", false);
//    delete inference;
//    EXPECT_GT(accuracy, 0.8230);
//}



//class ExperimentOnMNIST : public ::testing::Test {
//protected:
//
//    void SetUp() override {
//        trainer = new Dataset();
//        tester = new Dataset();
//        network = new Network(true);
//
//        string train_set_file_path = DATA_PATH + "/dataset/mnist.scale.dis8.t",
//        test_set_file_path = DATA_PATH + "/dataset/mnist.scale.dis8.t";
//
//        trainer->LoadLIBSVMData(train_set_file_path);
//        tester->LoadLIBSVMData(test_set_file_path);
//
//        StructureLearning *bnsl = new ChowLiuTree(network);
//        bnsl->StructLearnCompData(trainer, 1, 1, false, false);
//        ParameterLearning *bnpl = new ParameterLearning(network);
//        bnpl->LearnParamsKnowStructCompData(trainer, 2, false);
//    }
//
//    Dataset *trainer;
//    Dataset *tester;
//    Network *network;
//};
//
//TEST_F(ExperimentOnMNIST, junction_tree_partial1) {
//    Inference *inference = new JunctionTree(network);
//    double accuracy = inference->EvaluateAccuracy(tester, 1, -1, "jt", false);
//    delete inference;
//    EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnMNIST, junction_tree_partial2) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 2, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnMNIST, junction_tree_partial3) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 3, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnMNIST, junction_tree_partial4) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 4, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnMNIST, junction_tree_partial5) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 5, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnMNIST, junction_tree_partial6) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 6, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnMNIST, junction_tree_partial7) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 7, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnMNIST, junction_tree_partial8) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 8, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnMNIST, junction_tree_partial9) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 9, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnMNIST, junction_tree_partial10) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 10, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnMNIST, junction_tree_partial11) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 11, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnMNIST, junction_tree_partial12) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 12, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnMNIST, junction_tree_partial13) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 13, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnMNIST, junction_tree_partial14) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 14, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnMNIST, junction_tree_partial15) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 15, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnMNIST, junction_tree_partial16) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 16, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}



//class ExperimentOnMadelon : public ::testing::Test {
//protected:
//
//    void SetUp() override {
//        trainer = new Dataset();
//        tester = new Dataset();
////        network = new ChowLiuTree(true);
//        network = new Network(true);
//
//        string train_set_file_path = DATA_PATH + "/dataset/madelon.scale.dis8.p10",
//                test_set_file_path = DATA_PATH + "/dataset/madelon.scale.dis8.p10.t";
//
//        trainer->LoadLIBSVMData(train_set_file_path);
//        tester->LoadLIBSVMData(test_set_file_path);
//
//        StructureLearning *bnsl = new ChowLiuTree(network);
//        bnsl->StructLearnCompData(trainer, 1, 1, false, false);
//        ParameterLearning *bnpl = new ParameterLearning(network);
//        bnpl->LearnParamsKnowStructCompData(trainer, 2, false);
//    }
//
//    Dataset *trainer;
//    Dataset *tester;
//    Network *network;
//};
//
//TEST_F(ExperimentOnMadelon, junction_tree_partial1) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 1, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnMadelon, junction_tree_partial2) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 2, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnMadelon, junction_tree_partial3) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 3, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnMadelon, junction_tree_partial4) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 4, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnMadelon, junction_tree_partial5) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 5, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnMadelon, junction_tree_partial6) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 6, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnMadelon, junction_tree_partial7) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 7, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnMadelon, junction_tree_partial8) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 8, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnMadelon, junction_tree_partial9) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 9, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnMadelon, junction_tree_partial10) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 10, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnMadelon, junction_tree_partial11) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 11, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnMadelon, junction_tree_partial12) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 12, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnMadelon, junction_tree_partial13) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 13, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnMadelon, junction_tree_partial14) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 14, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnMadelon, junction_tree_partial15) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 15, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnMadelon, junction_tree_partial16) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 16, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//
//
//class ExperimentOnUsps : public ::testing::Test {
//protected:
//
//    void SetUp() override {
//        trainer = new Dataset();
//        tester = new Dataset();
////        network = new ChowLiuTree(true);
//        network = new Network(true);
//
//        string train_set_file_path = DATA_PATH + "/dataset/usps.scale.dis8.p10",
//                test_set_file_path = DATA_PATH + "/dataset/usps.scale.dis8.p10.t";
//
//        trainer->LoadLIBSVMData(train_set_file_path);
//        tester->LoadLIBSVMData(test_set_file_path);
//
//        StructureLearning *bnsl = new ChowLiuTree(network);
//        bnsl->StructLearnCompData(trainer, 1, 1, false, false);
//        ParameterLearning *bnpl = new ParameterLearning(network);
//        bnpl->LearnParamsKnowStructCompData(trainer, 2, false);
//    }
//
//    Dataset *trainer;
//    Dataset *tester;
//    Network *network;
//};
//
//TEST_F(ExperimentOnUsps, junction_tree_partial1) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 1, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnUsps, junction_tree_partial2) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 2, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnUsps, junction_tree_partial3) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 3, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnUsps, junction_tree_partial4) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 4, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnUsps, junction_tree_partial5) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 5, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnUsps, junction_tree_partial6) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 6, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnUsps, junction_tree_partial7) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 7, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnUsps, junction_tree_partial8) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 8, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnUsps, junction_tree_partial9) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 9, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnUsps, junction_tree_partial10) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 10, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnUsps, junction_tree_partial11) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 11, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnUsps, junction_tree_partial12) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 12, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnUsps, junction_tree_partial13) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 13, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnUsps, junction_tree_partial14) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 14, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnUsps, junction_tree_partial15) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 15, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnUsps, junction_tree_partial16) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 16, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//
//
//
//
//class ExperimentOnAloi : public ::testing::Test {
//protected:
//
//    void SetUp() override {
//        trainer = new Dataset();
//        tester = new Dataset();
////        network = new ChowLiuTree(true);
//        network = new Network(true);
//
//        string train_set_file_path = DATA_PATH + "/dataset/aloi.scale.dis8",
//                test_set_file_path = DATA_PATH + "/dataset/aloi.scale.dis8";
//
//        trainer->LoadLIBSVMData(train_set_file_path);
//        tester->LoadLIBSVMData(test_set_file_path);
//
//        StructureLearning *bnsl = new ChowLiuTree(network);
//        bnsl->StructLearnCompData(trainer, 1, 1, false, false);
//        ParameterLearning *bnpl = new ParameterLearning(network);
//        bnpl->LearnParamsKnowStructCompData(trainer, 2, false);
//    }
//
//    Dataset *trainer;
//    Dataset *tester;
//    Network *network;
//};
//
//TEST_F(ExperimentOnAloi, junction_tree_partial1) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 1, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnAloi, junction_tree_partial2) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 2, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnAloi, junction_tree_partial3) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 3, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnAloi, junction_tree_partial4) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 4, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnAloi, junction_tree_partial5) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 5, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnAloi, junction_tree_partial6) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 6, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnAloi, junction_tree_partial7) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 7, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnAloi, junction_tree_partial8) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 8, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnAloi, junction_tree_partial9) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 9, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnAloi, junction_tree_partial10) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 10, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnAloi, junction_tree_partial11) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 11, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnAloi, junction_tree_partial12) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 12, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnAloi, junction_tree_partial13) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 13, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnAloi, junction_tree_partial14) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 14, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnAloi, junction_tree_partial15) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 15, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnAloi, junction_tree_partial16) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 16, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}



//class ExperimentOnSVHN : public ::testing::Test {
//protected:
//
//    void SetUp() override {
//        trainer = new Dataset();
//        tester = new Dataset();
////        network = new ChowLiuTree(true);
//        network = new Network(true);
//
//        string train_set_file_path = DATA_PATH + "/dataset/SVHN.scale.dis8.p10.t",
//                test_set_file_path = DATA_PATH + "/dataset/SVHN.scale.dis8.p10.t";
//
//        trainer->LoadLIBSVMData(train_set_file_path);
//        tester->LoadLIBSVMData(test_set_file_path);
//
//        StructureLearning *bnsl = new ChowLiuTree(network);
//        bnsl->StructLearnCompData(trainer, 1, 1, false, false);
//        ParameterLearning *bnpl = new ParameterLearning(network);
//        bnpl->LearnParamsKnowStructCompData(trainer, 2, false);
//    }
//
//    Dataset *trainer;
//    Dataset *tester;
//    Network *network;
//};
//
//TEST_F(ExperimentOnSVHN, junction_tree_partial1) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 1, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnSVHN, junction_tree_partial2) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 2, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnSVHN, junction_tree_partial3) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 3, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnSVHN, junction_tree_partial4) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 4, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnSVHN, junction_tree_partial5) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 5, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnSVHN, junction_tree_partial6) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 6, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnSVHN, junction_tree_partial7) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 7, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnSVHN, junction_tree_partial8) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 8, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnSVHN, junction_tree_partial9) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 9, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnSVHN, junction_tree_partial10) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 10, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnSVHN, junction_tree_partial11) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 11, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnSVHN, junction_tree_partial12) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 12, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnSVHN, junction_tree_partial13) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 13, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnSVHN, junction_tree_partial14) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 14, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnSVHN, junction_tree_partial15) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 15, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnSVHN, junction_tree_partial16) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 16, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}



//class ExperimentOnCifar : public ::testing::Test {
//protected:
//
//    void SetUp() override {
//        trainer = new Dataset();
//        tester = new Dataset();
////        network = new ChowLiuTree(true);
//        network = new Network(true);
//
//        string train_set_file_path = DATA_PATH + "/dataset/cifar10.scale.dis8.p10",
//                test_set_file_path = DATA_PATH + "/dataset/cifar10.scale.dis8.p10.t";
//
//        trainer->LoadLIBSVMData(train_set_file_path);
//        tester->LoadLIBSVMData(test_set_file_path);
//
//        StructureLearning *bnsl = new ChowLiuTree(network);
//        bnsl->StructLearnCompData(trainer, 1, 1, false, false);
//        ParameterLearning *bnpl = new ParameterLearning(network);
//        bnpl->LearnParamsKnowStructCompData(trainer, 2, false);
//    }
//
//    Dataset *trainer;
//    Dataset *tester;
//    Network *network;
//};
//
//TEST_F(ExperimentOnCifar, junction_tree_partial1) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 1, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnCifar, junction_tree_partial2) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 2, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnCifar, junction_tree_partial3) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 3, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnCifar, junction_tree_partial4) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 4, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnCifar, junction_tree_partial5) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 5, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnCifar, junction_tree_partial6) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 6, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnCifar, junction_tree_partial7) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 7, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnCifar, junction_tree_partial8) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 8, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnCifar, junction_tree_partial9) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 9, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnCifar, junction_tree_partial10) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 10, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnCifar, junction_tree_partial11) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 11, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnCifar, junction_tree_partial12) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 12, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnCifar, junction_tree_partial13) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 13, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnCifar, junction_tree_partial14) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 14, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnCifar, junction_tree_partial15) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 15, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}
//TEST_F(ExperimentOnCifar, junction_tree_partial16) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, 16, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}




//class ExperimentOnConnect : public ::testing::Test {
//protected:
//
//    void SetUp() override {
//        trainer = new Dataset();
//        tester = new Dataset();
////        network = new ChowLiuTree(true);
//        network = new Network(true);
//
//        string train_set_file_path = DATA_PATH + "/dataset/connect-4",
//                test_set_file_path = DATA_PATH + "/dataset/connect-4";
//
//        trainer->LoadLIBSVMData(train_set_file_path);
//        tester->LoadLIBSVMData(test_set_file_path);
//
//        StructureLearning *bnsl = new ChowLiuTree(network);
//        bnsl->StructLearnCompData(trainer, 1, 1, true, false);
//        ParameterLearning *bnpl = new ParameterLearning(network);
//        bnpl->LearnParamsKnowStructCompData(trainer, 2, true);
//    }
//
//    Dataset *trainer;
//    Dataset *tester;
//    Network *network;
//};
//
//TEST_F(ExperimentOnConnect, junction_tree_partial) {
//Inference *inference = new JunctionTree(network);
//double accuracy = inference->EvaluateAccuracy(tester, -1, "jt", false);
//delete inference;
//EXPECT_GT(accuracy, 0.8230);
//}




//class ExperimentBNSLOnAlarm : public ::testing::Test {
//protected:
//
//    void SetUp() override {
//        trainer = new Dataset();
//        network = new Network(true);
//        string train_set_file_path = DATA_PATH + "/dataset/alarm/alarm_s5000.txt";
//        trainer->LoadCSVData(train_set_file_path, true, true, 0);
//    }
//    Dataset *trainer;
//    Network *network;
//};
//
//TEST_F(ExperimentBNSLOnAlarm, pc_stable) {
//    StructureLearning *bnsl = new PCStable(network, 0.05);
//    bnsl->StructLearnCompData(trainer, 1, 1, true, false);
//    delete trainer;
//
//    CustomNetwork *ref_net = new CustomNetwork();
//    ref_net->LoadBIFFile(DATA_PATH + "/dataset/alarm/alarm.bif");
//
//    BNSLComparison comp(ref_net, network);
//    int shd = comp.GetSHD();
//    cout << "SHD = " << shd << endl;
//    delete network;
//    delete ref_net;
//}

//TEST_F(ExperimentBNSLOnAlarm, pc_stable2) {
//StructureLearning *bnsl = new PCStable(network, 0.05);
//bnsl->StructLearnCompData(trainer, 1, 2, false, false);
//delete trainer;
//
//CustomNetwork *ref_net = new CustomNetwork();
//ref_net->LoadBIFFile(DATA_PATH + "/dataset/alarm/alarm.bif");
//
//BNSLComparison comp(ref_net, network);
//int shd = comp.GetSHD();
//cout << "SHD = " << shd << endl;
//delete network;
//delete ref_net;
//}
//
//TEST_F(ExperimentBNSLOnAlarm, pc_stable4) {
//StructureLearning *bnsl = new PCStable(network, 0.05);
//bnsl->StructLearnCompData(trainer, 1, 4, false, false);
//delete trainer;
//
//CustomNetwork *ref_net = new CustomNetwork();
//ref_net->LoadBIFFile(DATA_PATH + "/dataset/alarm/alarm.bif");
//
//BNSLComparison comp(ref_net, network);
//int shd = comp.GetSHD();
//cout << "SHD = " << shd << endl;
//delete network;
//delete ref_net;
//}
//
//TEST_F(ExperimentBNSLOnAlarm, pc_stable8) {
//StructureLearning *bnsl = new PCStable(network, 0.05);
//bnsl->StructLearnCompData(trainer, 1, 8, false, false);
//delete trainer;
//
//CustomNetwork *ref_net = new CustomNetwork();
//ref_net->LoadBIFFile(DATA_PATH + "/dataset/alarm/alarm.bif");
//
//BNSLComparison comp(ref_net, network);
//int shd = comp.GetSHD();
//cout << "SHD = " << shd << endl;
//delete network;
//delete ref_net;
//}
//
//TEST_F(ExperimentBNSLOnAlarm, pc_stable16) {
//StructureLearning *bnsl = new PCStable(network, 0.05);
//bnsl->StructLearnCompData(trainer, 1, 16, false, false);
//delete trainer;
//
//CustomNetwork *ref_net = new CustomNetwork();
//ref_net->LoadBIFFile(DATA_PATH + "/dataset/alarm/alarm.bif");
//
//BNSLComparison comp(ref_net, network);
//int shd = comp.GetSHD();
//cout << "SHD = " << shd << endl;
//delete network;
//delete ref_net;
//}

//TEST_F(ExperimentBNSLOnAlarm, pc_stable32) {
//StructureLearning *bnsl = new PCStable(network, 0.05);
//bnsl->StructLearnCompData(trainer, 1, 32, false, false);
//delete trainer;
//
//CustomNetwork *ref_net = new CustomNetwork();
//ref_net->LoadBIFFile(DATA_PATH + "/dataset/alarm/alarm.bif");
//
//BNSLComparison comp(ref_net, network);
//int shd = comp.GetSHD();
//cout << "SHD = " << shd << endl;
//delete network;
//delete ref_net;
//}




//class ExperimentBNSLOnInsurance : public ::testing::Test {
//protected:
//
//    void SetUp() override {
//        trainer = new Dataset();
//        network = new Network(true);
//        string train_set_file_path = DATA_PATH + "/dataset/insurance/insurance_s5000.txt";
//        trainer->LoadCSVData(train_set_file_path, true, true, 0);
//    }
//    Dataset *trainer;
//    Network *network;
//};
//
//TEST_F(ExperimentBNSLOnInsurance, pc_stable) {
//    StructureLearning *bnsl = new PCStable(network, 0.05);
//    bnsl->StructLearnCompData(trainer, 1, 1, false, false);
//    delete trainer;
//
//    CustomNetwork *ref_net = new CustomNetwork();
//    ref_net->LoadBIFFile(DATA_PATH + "/dataset/insurance/insurance.bif");
//
//    BNSLComparison comp(ref_net, network);
//    int shd = comp.GetSHD();
//    cout << "SHD = " << shd << endl;
//    delete network;
//    delete ref_net;
//}
//
//TEST_F(ExperimentBNSLOnInsurance, pc_stable2) {
//StructureLearning *bnsl = new PCStable(network, 0.05);
//bnsl->StructLearnCompData(trainer, 1, 2, false, false);
//delete trainer;
//
//CustomNetwork *ref_net = new CustomNetwork();
//ref_net->LoadBIFFile(DATA_PATH + "/dataset/insurance/insurance.bif");
//
//BNSLComparison comp(ref_net, network);
//int shd = comp.GetSHD();
//cout << "SHD = " << shd << endl;
//delete network;
//delete ref_net;
//}
//
//TEST_F(ExperimentBNSLOnInsurance, pc_stable4) {
//StructureLearning *bnsl = new PCStable(network, 0.05);
//bnsl->StructLearnCompData(trainer, 1, 4, false, false);
//delete trainer;
//
//CustomNetwork *ref_net = new CustomNetwork();
//ref_net->LoadBIFFile(DATA_PATH + "/dataset/insurance/insurance.bif");
//
//BNSLComparison comp(ref_net, network);
//int shd = comp.GetSHD();
//cout << "SHD = " << shd << endl;
//delete network;
//delete ref_net;
//}
//
//TEST_F(ExperimentBNSLOnInsurance, pc_stable8) {
//StructureLearning *bnsl = new PCStable(network, 0.05);
//bnsl->StructLearnCompData(trainer, 1, 8, false, false);
//delete trainer;
//
//CustomNetwork *ref_net = new CustomNetwork();
//ref_net->LoadBIFFile(DATA_PATH + "/dataset/insurance/insurance.bif");
//
//BNSLComparison comp(ref_net, network);
//int shd = comp.GetSHD();
//cout << "SHD = " << shd << endl;
//delete network;
//delete ref_net;
//}
//
//TEST_F(ExperimentBNSLOnInsurance, pc_stable16) {
//StructureLearning *bnsl = new PCStable(network, 0.05);
//bnsl->StructLearnCompData(trainer, 1, 16, false, false);
//delete trainer;
//
//CustomNetwork *ref_net = new CustomNetwork();
//ref_net->LoadBIFFile(DATA_PATH + "/dataset/insurance/insurance.bif");
//
//BNSLComparison comp(ref_net, network);
//int shd = comp.GetSHD();
//cout << "SHD = " << shd << endl;
//delete network;
//delete ref_net;
//}
//
//TEST_F(ExperimentBNSLOnInsurance, pc_stable32) {
//StructureLearning *bnsl = new PCStable(network, 0.05);
//bnsl->StructLearnCompData(trainer, 1, 32, false, false);
//delete trainer;
//
//CustomNetwork *ref_net = new CustomNetwork();
//ref_net->LoadBIFFile(DATA_PATH + "/dataset/insurance/insurance.bif");
//
//BNSLComparison comp(ref_net, network);
//int shd = comp.GetSHD();
//cout << "SHD = " << shd << endl;
//delete network;
//delete ref_net;
//}




//class ExperimentBNSLOnHepar2 : public ::testing::Test {
//protected:
//
//    void SetUp() override {
//        trainer = new Dataset();
//        network = new Network(true);
//        string train_set_file_path = DATA_PATH + "/dataset/hepar2/hepar2_s5000.txt";
//        trainer->LoadCSVData(train_set_file_path, true, true, 0);
//    }
//    Dataset *trainer;
//    Network *network;
//};
//
//TEST_F(ExperimentBNSLOnHepar2, pc_stable) {
//    StructureLearning *bnsl = new PCStable(network, 0.05);
//    bnsl->StructLearnCompData(trainer, 1, 1, false, false);
//    delete trainer;
//
//    CustomNetwork *ref_net = new CustomNetwork();
//    ref_net->LoadBIFFile(DATA_PATH + "/dataset/hepar2/hepar2.bif");
//
//    BNSLComparison comp(ref_net, network);
//    int shd = comp.GetSHD();
//    cout << "SHD = " << shd << endl;
//    delete network;
//    delete ref_net;
//}
//
//TEST_F(ExperimentBNSLOnHepar2, pc_stable2) {
//StructureLearning *bnsl = new PCStable(network, 0.05);
//bnsl->StructLearnCompData(trainer, 1, 2, false, false);
//delete trainer;
//
//CustomNetwork *ref_net = new CustomNetwork();
//ref_net->LoadBIFFile(DATA_PATH + "/dataset/hepar2/hepar2.bif");
//
//BNSLComparison comp(ref_net, network);
//int shd = comp.GetSHD();
//cout << "SHD = " << shd << endl;
//delete network;
//delete ref_net;
//}
//
//TEST_F(ExperimentBNSLOnHepar2, pc_stable4) {
//StructureLearning *bnsl = new PCStable(network, 0.05);
//bnsl->StructLearnCompData(trainer, 1, 4, false, false);
//delete trainer;
//
//CustomNetwork *ref_net = new CustomNetwork();
//ref_net->LoadBIFFile(DATA_PATH + "/dataset/hepar2/hepar2.bif");
//
//BNSLComparison comp(ref_net, network);
//int shd = comp.GetSHD();
//cout << "SHD = " << shd << endl;
//delete network;
//delete ref_net;
//}
//
//TEST_F(ExperimentBNSLOnHepar2, pc_stable8) {
//StructureLearning *bnsl = new PCStable(network, 0.05);
//bnsl->StructLearnCompData(trainer, 1, 8, false, false);
//delete trainer;
//
//CustomNetwork *ref_net = new CustomNetwork();
//ref_net->LoadBIFFile(DATA_PATH + "/dataset/hepar2/hepar2.bif");
//
//BNSLComparison comp(ref_net, network);
//int shd = comp.GetSHD();
//cout << "SHD = " << shd << endl;
//delete network;
//delete ref_net;
//}
//
//TEST_F(ExperimentBNSLOnHepar2, pc_stable16) {
//StructureLearning *bnsl = new PCStable(network, 0.05);
//bnsl->StructLearnCompData(trainer, 1, 16, false, false);
//delete trainer;
//
//CustomNetwork *ref_net = new CustomNetwork();
//ref_net->LoadBIFFile(DATA_PATH + "/dataset/hepar2/hepar2.bif");
//
//BNSLComparison comp(ref_net, network);
//int shd = comp.GetSHD();
//cout << "SHD = " << shd << endl;
//delete network;
//delete ref_net;
//}

//TEST_F(ExperimentBNSLOnHepar2, pc_stable32) {
//StructureLearning *bnsl = new PCStable(network, 0.05);
//bnsl->StructLearnCompData(trainer, 1, 32, false, false);
//delete trainer;
//
//CustomNetwork *ref_net = new CustomNetwork();
//ref_net->LoadBIFFile(DATA_PATH + "/dataset/hepar2/hepar2.bif");
//
//BNSLComparison comp(ref_net, network);
//int shd = comp.GetSHD();
//cout << "SHD = " << shd << endl;
//delete network;
//delete ref_net;
//}





//class ExperimentBNSLOnMunin1 : public ::testing::Test {
//protected:
//
//    void SetUp() override {
//        trainer = new Dataset();
//        network = new Network(true);
//        string train_set_file_path = DATA_PATH + "/dataset/munin1/munin1_s5000_o.txt";
//        trainer->LoadCSVData(train_set_file_path, true, true, 0);
//    }
//    Dataset *trainer;
//    Network *network;
//};
//
//TEST_F(ExperimentBNSLOnMunin1, pc_stable) {
//    StructureLearning *bnsl = new PCStable(network, 0.05);
//    bnsl->StructLearnCompData(trainer, 1, 1, false, false);
//    delete trainer;
//
//    CustomNetwork *ref_net = new CustomNetwork();
//    ref_net->LoadBIFFile(DATA_PATH + "/dataset/munin1/munin1.bif");
//
//    BNSLComparison comp(ref_net, network);
//    int shd = comp.GetSHD();
//    cout << "SHD = " << shd << endl;
//    delete network;
//    delete ref_net;
//}
//
//TEST_F(ExperimentBNSLOnMunin1, pc_stable2) {
//StructureLearning *bnsl = new PCStable(network, 0.05);
//bnsl->StructLearnCompData(trainer, 1, 2, false, false);
//delete trainer;
//
//CustomNetwork *ref_net = new CustomNetwork();
//ref_net->LoadBIFFile(DATA_PATH + "/dataset/munin1/munin1.bif");
//
//BNSLComparison comp(ref_net, network);
//int shd = comp.GetSHD();
//cout << "SHD = " << shd << endl;
//delete network;
//delete ref_net;
//}
//
//TEST_F(ExperimentBNSLOnMunin1, pc_stable4) {
//StructureLearning *bnsl = new PCStable(network, 0.05);
//bnsl->StructLearnCompData(trainer, 1, 4, false, false);
//delete trainer;
//
//CustomNetwork *ref_net = new CustomNetwork();
//ref_net->LoadBIFFile(DATA_PATH + "/dataset/munin1/munin1.bif");
//
//BNSLComparison comp(ref_net, network);
//int shd = comp.GetSHD();
//cout << "SHD = " << shd << endl;
//delete network;
//delete ref_net;
//}
//
//TEST_F(ExperimentBNSLOnMunin1, pc_stable8) {
//StructureLearning *bnsl = new PCStable(network, 0.05);
//bnsl->StructLearnCompData(trainer, 1, 8, false, false);
//delete trainer;
//
//CustomNetwork *ref_net = new CustomNetwork();
//ref_net->LoadBIFFile(DATA_PATH + "/dataset/munin1/munin1.bif");
//
//BNSLComparison comp(ref_net, network);
//int shd = comp.GetSHD();
//cout << "SHD = " << shd << endl;
//delete network;
//delete ref_net;
//}
//
//TEST_F(ExperimentBNSLOnMunin1, pc_stable16) {
//StructureLearning *bnsl = new PCStable(network, 0.05);
//bnsl->StructLearnCompData(trainer, 1, 16, false, false);
//delete trainer;
//
//CustomNetwork *ref_net = new CustomNetwork();
//ref_net->LoadBIFFile(DATA_PATH + "/dataset/munin1/munin1.bif");
//
//BNSLComparison comp(ref_net, network);
//int shd = comp.GetSHD();
//cout << "SHD = " << shd << endl;
//delete network;
//delete ref_net;
//}
//
//TEST_F(ExperimentBNSLOnMunin1, pc_stable32) {
//StructureLearning *bnsl = new PCStable(network, 0.05);
//bnsl->StructLearnCompData(trainer, 1, 32, false, false);
//delete trainer;
//
//CustomNetwork *ref_net = new CustomNetwork();
//ref_net->LoadBIFFile(DATA_PATH + "/dataset/munin1/munin1.bif");
//
//BNSLComparison comp(ref_net, network);
//int shd = comp.GetSHD();
//cout << "SHD = " << shd << endl;
//delete network;
//delete ref_net;
//}



//class ExperimentBNSLOnMunin2 : public ::testing::Test {
//protected:
//
//    void SetUp() override {
//        trainer = new Dataset();
//        network = new Network(true);
//        string train_set_file_path = DATA_PATH + "/dataset/munin2/munin2_s5000.txt";
//        trainer->LoadCSVData(train_set_file_path, true, true, 0);
//    }
//    Dataset *trainer;
//    Network *network;
//};
//
//TEST_F(ExperimentBNSLOnMunin2, pc_stable) {
//StructureLearning *bnsl = new PCStable(network, 0.05);
//bnsl->StructLearnCompData(trainer, 1, 1, false, false);
//delete trainer;
//
//CustomNetwork *ref_net = new CustomNetwork();
//ref_net->LoadBIFFile(DATA_PATH + "/dataset/munin2/munin2.bif");
//
//BNSLComparison comp(ref_net, network);
//int shd = comp.GetSHD();
//cout << "SHD = " << shd << endl;
//delete network;
//delete ref_net;
//}
//
//TEST_F(ExperimentBNSLOnMunin2, pc_stable2) {
//StructureLearning *bnsl = new PCStable(network, 0.05);
//bnsl->StructLearnCompData(trainer, 1, 2, false, false);
//delete trainer;
//
//CustomNetwork *ref_net = new CustomNetwork();
//ref_net->LoadBIFFile(DATA_PATH + "/dataset/munin2/munin2.bif");
//
//BNSLComparison comp(ref_net, network);
//int shd = comp.GetSHD();
//cout << "SHD = " << shd << endl;
//delete network;
//delete ref_net;
//}
//
//TEST_F(ExperimentBNSLOnMunin2, pc_stable4) {
//StructureLearning *bnsl = new PCStable(network, 0.05);
//bnsl->StructLearnCompData(trainer, 1, 4, false, false);
//delete trainer;
//
//CustomNetwork *ref_net = new CustomNetwork();
//ref_net->LoadBIFFile(DATA_PATH + "/dataset/munin2/munin2.bif");
//
//BNSLComparison comp(ref_net, network);
//int shd = comp.GetSHD();
//cout << "SHD = " << shd << endl;
//delete network;
//delete ref_net;
//}
//
//TEST_F(ExperimentBNSLOnMunin2, pc_stable8) {
//StructureLearning *bnsl = new PCStable(network, 0.05);
//bnsl->StructLearnCompData(trainer, 1, 8, false, false);
//delete trainer;
//
//CustomNetwork *ref_net = new CustomNetwork();
//ref_net->LoadBIFFile(DATA_PATH + "/dataset/munin2/munin2.bif");
//
//BNSLComparison comp(ref_net, network);
//int shd = comp.GetSHD();
//cout << "SHD = " << shd << endl;
//delete network;
//delete ref_net;
//}
//
//TEST_F(ExperimentBNSLOnMunin2, pc_stable16) {
//StructureLearning *bnsl = new PCStable(network, 0.05);
//bnsl->StructLearnCompData(trainer, 1, 16, false, false);
//delete trainer;
//
//CustomNetwork *ref_net = new CustomNetwork();
//ref_net->LoadBIFFile(DATA_PATH + "/dataset/munin2/munin2.bif");
//
//BNSLComparison comp(ref_net, network);
//int shd = comp.GetSHD();
//cout << "SHD = " << shd << endl;
//delete network;
//delete ref_net;
//}
//
//TEST_F(ExperimentBNSLOnMunin2, pc_stable32) {
//StructureLearning *bnsl = new PCStable(network, 0.05);
//bnsl->StructLearnCompData(trainer, 1, 32, false, false);
//delete trainer;
//
//CustomNetwork *ref_net = new CustomNetwork();
//ref_net->LoadBIFFile(DATA_PATH + "/dataset/munin2/munin2.bif");
//
//BNSLComparison comp(ref_net, network);
//int shd = comp.GetSHD();
//cout << "SHD = " << shd << endl;
//delete network;
//delete ref_net;
//}
//
//
//
//class ExperimentBNSLOnMunin3 : public ::testing::Test {
//protected:
//
//    void SetUp() override {
//        trainer = new Dataset();
//        network = new Network(true);
//        string train_set_file_path = DATA_PATH + "/dataset/munin3/munin3_s5000.txt";
//        trainer->LoadCSVData(train_set_file_path, true, true, 0);
//    }
//    Dataset *trainer;
//    Network *network;
//};
//
//TEST_F(ExperimentBNSLOnMunin3, pc_stable) {
//StructureLearning *bnsl = new PCStable(network, 0.05);
//bnsl->StructLearnCompData(trainer, 1, 1, false, false);
//delete trainer;
//
//CustomNetwork *ref_net = new CustomNetwork();
//ref_net->LoadBIFFile(DATA_PATH + "/dataset/munin3/munin3.bif");
//
//BNSLComparison comp(ref_net, network);
//int shd = comp.GetSHD();
//cout << "SHD = " << shd << endl;
//delete network;
//delete ref_net;
//}
//
//TEST_F(ExperimentBNSLOnMunin3, pc_stable2) {
//StructureLearning *bnsl = new PCStable(network, 0.05);
//bnsl->StructLearnCompData(trainer, 1, 2, false, false);
//delete trainer;
//
//CustomNetwork *ref_net = new CustomNetwork();
//ref_net->LoadBIFFile(DATA_PATH + "/dataset/munin3/munin3.bif");
//
//BNSLComparison comp(ref_net, network);
//int shd = comp.GetSHD();
//cout << "SHD = " << shd << endl;
//delete network;
//delete ref_net;
//}
//
//TEST_F(ExperimentBNSLOnMunin3, pc_stable4) {
//StructureLearning *bnsl = new PCStable(network, 0.05);
//bnsl->StructLearnCompData(trainer, 1, 4, false, false);
//delete trainer;
//
//CustomNetwork *ref_net = new CustomNetwork();
//ref_net->LoadBIFFile(DATA_PATH + "/dataset/munin3/munin3.bif");
//
//BNSLComparison comp(ref_net, network);
//int shd = comp.GetSHD();
//cout << "SHD = " << shd << endl;
//delete network;
//delete ref_net;
//}
//
//TEST_F(ExperimentBNSLOnMunin3, pc_stable8) {
//StructureLearning *bnsl = new PCStable(network, 0.05);
//bnsl->StructLearnCompData(trainer, 1, 8, false, false);
//delete trainer;
//
//CustomNetwork *ref_net = new CustomNetwork();
//ref_net->LoadBIFFile(DATA_PATH + "/dataset/munin3/munin3.bif");
//
//BNSLComparison comp(ref_net, network);
//int shd = comp.GetSHD();
//cout << "SHD = " << shd << endl;
//delete network;
//delete ref_net;
//}
//
//TEST_F(ExperimentBNSLOnMunin3, pc_stable16) {
//StructureLearning *bnsl = new PCStable(network, 0.05);
//bnsl->StructLearnCompData(trainer, 1, 16, false, false);
//delete trainer;
//
//CustomNetwork *ref_net = new CustomNetwork();
//ref_net->LoadBIFFile(DATA_PATH + "/dataset/munin3/munin3.bif");
//
//BNSLComparison comp(ref_net, network);
//int shd = comp.GetSHD();
//cout << "SHD = " << shd << endl;
//delete network;
//delete ref_net;
//}
//
//TEST_F(ExperimentBNSLOnMunin3, pc_stable32) {
//StructureLearning *bnsl = new PCStable(network, 0.05);
//bnsl->StructLearnCompData(trainer, 1, 32, false, false);
//delete trainer;
//
//CustomNetwork *ref_net = new CustomNetwork();
//ref_net->LoadBIFFile(DATA_PATH + "/dataset/munin3/munin3.bif");
//
//BNSLComparison comp(ref_net, network);
//int shd = comp.GetSHD();
//cout << "SHD = " << shd << endl;
//delete network;
//delete ref_net;
//}



//class ExperimentBNSLOnMunin4 : public ::testing::Test {
//protected:
//
//    void SetUp() override {
//        trainer = new Dataset();
//        network = new Network(true);
//        string train_set_file_path = DATA_PATH + "/dataset/munin4/munin4_s5000.txt";
//        trainer->LoadCSVData(train_set_file_path, true, true, 0);
//    }
//    Dataset *trainer;
//    Network *network;
//};
//
//TEST_F(ExperimentBNSLOnMunin4, pc_stable) {
//StructureLearning *bnsl = new PCStable(network, 0.05);
//bnsl->StructLearnCompData(trainer, 1, 1, false, false);
//delete trainer;
//
//CustomNetwork *ref_net = new CustomNetwork();
//ref_net->LoadBIFFile(DATA_PATH + "/dataset/munin4/munin4.bif");
//
//BNSLComparison comp(ref_net, network);
//int shd = comp.GetSHD();
//cout << "SHD = " << shd << endl;
//delete network;
//delete ref_net;
//}
//
//TEST_F(ExperimentBNSLOnMunin4, pc_stable2) {
//StructureLearning *bnsl = new PCStable(network, 0.05);
//bnsl->StructLearnCompData(trainer, 1, 2, false, false);
//delete trainer;
//
//CustomNetwork *ref_net = new CustomNetwork();
//ref_net->LoadBIFFile(DATA_PATH + "/dataset/munin4/munin4.bif");
//
//BNSLComparison comp(ref_net, network);
//int shd = comp.GetSHD();
//cout << "SHD = " << shd << endl;
//delete network;
//delete ref_net;
//}
//
//TEST_F(ExperimentBNSLOnMunin4, pc_stable4) {
//StructureLearning *bnsl = new PCStable(network, 0.05);
//bnsl->StructLearnCompData(trainer, 1, 4, false, false);
//delete trainer;
//
//CustomNetwork *ref_net = new CustomNetwork();
//ref_net->LoadBIFFile(DATA_PATH + "/dataset/munin4/munin4.bif");
//
//BNSLComparison comp(ref_net, network);
//int shd = comp.GetSHD();
//cout << "SHD = " << shd << endl;
//delete network;
//delete ref_net;
//}
//
//TEST_F(ExperimentBNSLOnMunin4, pc_stable8) {
//StructureLearning *bnsl = new PCStable(network, 0.05);
//bnsl->StructLearnCompData(trainer, 1, 8, false, false);
//delete trainer;
//
//CustomNetwork *ref_net = new CustomNetwork();
//ref_net->LoadBIFFile(DATA_PATH + "/dataset/munin4/munin4.bif");
//
//BNSLComparison comp(ref_net, network);
//int shd = comp.GetSHD();
//cout << "SHD = " << shd << endl;
//delete network;
//delete ref_net;
//}
//
//TEST_F(ExperimentBNSLOnMunin4, pc_stable16) {
//StructureLearning *bnsl = new PCStable(network, 0.05);
//bnsl->StructLearnCompData(trainer, 1, 16, false, false);
//delete trainer;
//
//CustomNetwork *ref_net = new CustomNetwork();
//ref_net->LoadBIFFile(DATA_PATH + "/dataset/munin4/munin4.bif");
//
//BNSLComparison comp(ref_net, network);
//int shd = comp.GetSHD();
//cout << "SHD = " << shd << endl;
//delete network;
//delete ref_net;
//}
//
//TEST_F(ExperimentBNSLOnMunin4, pc_stable32) {
//StructureLearning *bnsl = new PCStable(network, 0.05);
//bnsl->StructLearnCompData(trainer, 1, 32, false, false);
//delete trainer;
//
//CustomNetwork *ref_net = new CustomNetwork();
//ref_net->LoadBIFFile(DATA_PATH + "/dataset/munin4/munin4.bif");
//
//BNSLComparison comp(ref_net, network);
//int shd = comp.GetSHD();
//cout << "SHD = " << shd << endl;
//delete network;
//delete ref_net;
//}




//class ExperimentBNSLOnDiabetes : public ::testing::Test {
//protected:
//
//    void SetUp() override {
//        trainer = new Dataset();
//        network = new Network(true);
//
//        string train_set_file_path = DATA_PATH + "/dataset/diabetes/diabetes_s5000.txt";
//
//        trainer->LoadCSVData(train_set_file_path, true, true, 0);
//    }
//
//    Dataset *trainer;
//    Network *network;
//};
//
//TEST_F(ExperimentBNSLOnDiabetes, pc_stable16) {
//    StructureLearning *bnsl = new PCStable(network, 0.05);
//    bnsl->StructLearnCompData(trainer, 1, 16, false, false);
//    delete trainer;
//
//    CustomNetwork *ref_net = new CustomNetwork();
//    ref_net->LoadBIFFile(DATA_PATH + "/dataset/diabetes/diabetes.bif");
//
//    BNSLComparison comp(ref_net, network);
//    int shd = comp.GetSHD();
//    cout << "SHD = " << shd << endl;
//    delete network;
//    delete ref_net;
//}
//
//TEST_F(ExperimentBNSLOnDiabetes, pc_stable32) {
//    StructureLearning *bnsl = new PCStable(network, 0.05);
//    bnsl->StructLearnCompData(trainer, 1, 32, false, false);
//    delete trainer;
//
//    CustomNetwork *ref_net = new CustomNetwork();
//    ref_net->LoadBIFFile(DATA_PATH + "/dataset/diabetes/diabetes.bif");
//
//    BNSLComparison comp(ref_net, network);
//    int shd = comp.GetSHD();
//    cout << "SHD = " << shd << endl;
//    delete network;
//    delete ref_net;
//}
//
//TEST_F(ExperimentBNSLOnDiabetes, pc_stable8) {
//    StructureLearning *bnsl = new PCStable(network, 0.05);
//    bnsl->StructLearnCompData(trainer, 1, 8, false, false);
//    delete trainer;
//
//    CustomNetwork *ref_net = new CustomNetwork();
//    ref_net->LoadBIFFile(DATA_PATH + "/dataset/diabetes/diabetes.bif");
//
//    BNSLComparison comp(ref_net, network);
//    int shd = comp.GetSHD();
//    cout << "SHD = " << shd << endl;
//    delete network;
//    delete ref_net;
//}
//
//TEST_F(ExperimentBNSLOnDiabetes, pc_stable4) {
//    StructureLearning *bnsl = new PCStable(network, 0.05);
//    bnsl->StructLearnCompData(trainer, 1, 4, false, false);
//    delete trainer;
//
//    CustomNetwork *ref_net = new CustomNetwork();
//    ref_net->LoadBIFFile(DATA_PATH + "/dataset/diabetes/diabetes.bif");
//
//    BNSLComparison comp(ref_net, network);
//    int shd = comp.GetSHD();
//    cout << "SHD = " << shd << endl;
//    delete network;
//    delete ref_net;
//}
//
//TEST_F(ExperimentBNSLOnDiabetes, pc_stable2) {
//    StructureLearning *bnsl = new PCStable(network, 0.05);
//    bnsl->StructLearnCompData(trainer, 1, 2, false, false);
//    delete trainer;
//
//    CustomNetwork *ref_net = new CustomNetwork();
//    ref_net->LoadBIFFile(DATA_PATH + "/dataset/diabetes/diabetes.bif");
//
//    BNSLComparison comp(ref_net, network);
//    int shd = comp.GetSHD();
//    cout << "SHD = " << shd << endl;
//    delete network;
//    delete ref_net;
//}
//
//TEST_F(ExperimentBNSLOnDiabetes, pc_stable) {
//StructureLearning *bnsl = new PCStable(network, 0.05);
//bnsl->StructLearnCompData(trainer, 1, 1, false, false);
//delete trainer;
//
//CustomNetwork *ref_net = new CustomNetwork();
//ref_net->LoadBIFFile(DATA_PATH + "/dataset/diabetes/diabetes.bif");
//
//BNSLComparison comp(ref_net, network);
//int shd = comp.GetSHD();
//cout << "SHD = " << shd << endl;
//delete network;
//delete ref_net;
//}




//class ExperimentBNSLOnLink : public ::testing::Test {
//protected:
//
//    void SetUp() override {
//        trainer = new Dataset();
//        network = new Network(true);
//        string train_set_file_path = DATA_PATH + "/dataset/link/link_s5000.txt";
//        trainer->LoadCSVData(train_set_file_path, true, true, 0);
//    }
//
//    Dataset *trainer;
//    Network *network;
//};
//
//TEST_F(ExperimentBNSLOnLink, pc_stable8) {
//    StructureLearning *bnsl = new PCStable(network, 0.05);
//    bnsl->StructLearnCompData(trainer, 1, 8, false, false);
//    delete trainer;
//
//    CustomNetwork *ref_net = new CustomNetwork();
//    ref_net->LoadBIFFile(DATA_PATH + "/dataset/link/link.bif");
//
//    BNSLComparison comp(ref_net, network);
//    int shd = comp.GetSHD();
//    cout << "SHD = " << shd << endl;
//    delete network;
//    delete ref_net;
//}
//
//TEST_F(ExperimentBNSLOnLink, pc_stable4) {
//    StructureLearning *bnsl = new PCStable(network, 0.05);
//    bnsl->StructLearnCompData(trainer, 1, 4, false, false);
//    delete trainer;
//
//    CustomNetwork *ref_net = new CustomNetwork();
//    ref_net->LoadBIFFile(DATA_PATH + "/dataset/link/link.bif");
//
//    BNSLComparison comp(ref_net, network);
//    int shd = comp.GetSHD();
//    cout << "SHD = " << shd << endl;
//    delete network;
//    delete ref_net;
//}
//
//TEST_F(ExperimentBNSLOnLink, pc_stable2) {
//    StructureLearning *bnsl = new PCStable(network, 0.05);
//    bnsl->StructLearnCompData(trainer, 1, 2, false, false);
//    delete trainer;
//
//    CustomNetwork *ref_net = new CustomNetwork();
//    ref_net->LoadBIFFile(DATA_PATH + "/dataset/link/link.bif");
//
//    BNSLComparison comp(ref_net, network);
//    int shd = comp.GetSHD();
//    cout << "SHD = " << shd << endl;
//    delete network;
//    delete ref_net;
//}
//
//TEST_F(ExperimentBNSLOnLink, pc_stable) {
//    StructureLearning *bnsl = new PCStable(network, 0.05);
//    bnsl->StructLearnCompData(trainer, 1, 1, false, false);
//    delete trainer;
//
//    CustomNetwork *ref_net = new CustomNetwork();
//    ref_net->LoadBIFFile(DATA_PATH + "/dataset/link/link.bif");
//
//    BNSLComparison comp(ref_net, network);
//    int shd = comp.GetSHD();
//    cout << "SHD = " << shd << endl;
//    delete network;
//    delete ref_net;
//}
//
//TEST_F(ExperimentBNSLOnLink, pc_stable32) {
//    StructureLearning *bnsl = new PCStable(network, 0.05);
//    bnsl->StructLearnCompData(trainer, 1, 32, false, false);
//    delete trainer;
//
//    CustomNetwork *ref_net = new CustomNetwork();
//    ref_net->LoadBIFFile(DATA_PATH + "/dataset/link/link.bif");
//
//    BNSLComparison comp(ref_net, network);
//    int shd = comp.GetSHD();
//    cout << "SHD = " << shd << endl;
//    delete network;
//    delete ref_net;
//}
//
//TEST_F(ExperimentBNSLOnLink, pc_stable16) {
//    StructureLearning *bnsl = new PCStable(network, 0.05);
//    bnsl->StructLearnCompData(trainer, 1, 16, false, false);
//    delete trainer;
//
//    CustomNetwork *ref_net = new CustomNetwork();
//    ref_net->LoadBIFFile(DATA_PATH + "/dataset/link/link.bif");
//
//    BNSLComparison comp(ref_net, network);
//    int shd = comp.GetSHD();
//    cout << "SHD = " << shd << endl;
//    delete network;
//    delete ref_net;
//}













//class ExperimentBNSLOnPigs : public ::testing::Test {
//protected:
//
//    void SetUp() override {
//        trainer = new Dataset();
//        network = new Network(true);
//        string train_set_file_path = "../../../dataset/pigs/pigs_s1000.txt";
//        trainer->LoadCSVData(train_set_file_path, true, true, 0);
//    }
//
//    Dataset *trainer;
//    Network *network;
//};
//
//TEST_F(ExperimentBNSLOnPigs, pc_stable) {
//    StructureLearning *bnsl = new PCStable(network, 0.05);
//    bnsl->StructLearnCompData(trainer, 1, 8, false, false);
//    delete trainer;
//
//    CustomNetwork *ref_net = new CustomNetwork();
//    ref_net->LoadBIFFile("../../../dataset/pigs/pigs.bif");
//
//    BNSLComparison comp(ref_net, network);
//    int shd = comp.GetSHD();
//    cout << "SHD = " << shd << endl;
//    delete network;
//    delete ref_net;
//}
//
//
//class ExperimentBNSLOnPathfinder : public ::testing::Test {
//protected:
//
//    void SetUp() override {
//        trainer = new Dataset();
//        network = new Network(true);
//        string train_set_file_path = "../../../dataset/pathfinder/pathfinder_s1000.txt";
//        trainer->LoadCSVData(train_set_file_path, true, true, 0);
//    }
//
//    Dataset *trainer;
//    Network *network;
//};
//
//TEST_F(ExperimentBNSLOnPathfinder, pc_stable) {
//    StructureLearning *bnsl = new PCStable(network, 0.05);
//    bnsl->StructLearnCompData(trainer, 1, 8, false, false);
//    delete trainer;
//
//    CustomNetwork *ref_net = new CustomNetwork();
//    ref_net->LoadBIFFile("../../../dataset/pathfinder/pathfinder.bif");
//
//    BNSLComparison comp(ref_net, network);
//    int shd = comp.GetSHD();
//    cout << "SHD = " << shd << endl;
//    delete network;
//    delete ref_net;
//}











