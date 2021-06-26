//
// Created by LinjianLi on 2019/10/7.
//

#include <iostream>
#include <string>
#include <vector>
#include "gtest/gtest.h"
#include "tinyxml2.h"

#include "Dataset.h"
#include "Network.h"
#include "Inference.h"
#include "ExactInference.h"
#include "ApproximateInference.h"
#include "StructureLearning.h"
#include "ParameterLearning.h"
#include "ChowLiuTree.h"
#include "PCStable.h"
#include "JunctionTree.h"
#include "CustomNetwork.h"
#include "ScoreFunction.h"
#include "gadget.h"
#include "ChoiceGenerator.h"


//class ExperimentOnA1a : public ::testing::Test {
//protected:
//
//    void SetUp() override {
//        trainer = new Dataset();
//        tester = new Dataset();
//        network = new ChowLiuTree(true);
//
//        string train_set_file_path = "../../data/dataset/a1a.txt",
//                test_set_file_path = "../../data/dataset/a1a.test.txt";
//
//        trainer->LoadLIBSVMData(train_set_file_path);
//        tester->LoadLIBSVMData(test_set_file_path);
//        network->StructLearnCompData(trainer, true);
//        network->LearnParamsKnowStructCompData(trainer, 2, true);
//    }
//
//    Dataset *trainer;
//    Dataset *tester;
//    Network *network;
//};

//TEST_F(ExperimentOnA1a, ve) {
//    Inference *inference = new ExactInference(network);
//    double accuracy = inference->EvaluateAccuracy(tester, -1, "ve", true);
//    EXPECT_GT(accuracy, 0.8230);
//}
//
//TEST_F(ExperimentOnA1a, ve_partial) {
//    Inference *inference = new ExactInference(network);
//    double accuracy = inference->EvaluateAccuracy(tester, -1, "ve", false);
//    EXPECT_GT(accuracy, 0.8230);
//}
//
//TEST_F(ExperimentOnA1a, brute_force) {
//    Inference *inference = new ExactInference(network);
//    double accuracy = inference->EvaluateAccuracy(tester, -1, "direct", true);
//    EXPECT_GT(accuracy, 0.8230);
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
//
//TEST_F(ExperimentOnA1a, junction_tree) {
//    auto *jt = new JunctionTree(network, false);
//    double accuracy = jt->EvaluateJTAccuracy(0, tester);
//    delete jt;
//    EXPECT_GT(accuracy,0.8150);
//}


//class ExperimentOnCovertype : public ::testing::Test {
// protected:
//
//  void SetUp() override {
//    trainer = new Dataset();
//    tester = new Dataset();
//    network = new ChowLiuTree(true);
//
//    string train_set_file_path = "../../data/dataset/Covertype/covtype_discretization_train.data",
//           test_set_file_path  = "../../data/dataset/Covertype/covtype_discretization_test.data";
//
//
//    trainer->LoadCSVData(train_set_file_path, false, false, 54);
//    tester->LoadCSVData(test_set_file_path, false, false, 54);
//    network->StructLearnCompData(trainer, true);
//    network->LearnParamsKnowStructCompData(trainer, true);
//  }
//
//  Dataset *trainer;
//  Dataset *tester;
//  Network *network;
//};
//
//TEST_F(ExperimentOnCovertype, do_nothing) {
//  cout << "Hello world..." << endl;
//}
//
//TEST_F(ExperimentOnCovertype, ve) {
//    Inference *inference = new ExactInference(network);
//    double accuracy = inference->EvaluateAccuracy(tester, -1, "ve", true);
//    EXPECT_GT(accuracy, 0.600);
//}
//
//TEST_F(ExperimentOnCovertype, ve_partial) {
//    Inference *inference = new ExactInference(network);
//    double accuracy = inference->EvaluateAccuracy(tester, -1, "ve", false);
//    EXPECT_GT(accuracy, 0.600);
//}
//
//TEST_F(ExperimentOnCovertype, brute_force) {
//    Inference *inference = new ExactInference(network);
//    double accuracy = inference->EvaluateAccuracy(tester, -1, "direct", true);
//    EXPECT_GT(accuracy, 0.600);
//}



//
//TEST_F(ExperimentOnCovertype, covertype_test_like_weigh) {
//  double accuracy = network->EvaluateLikelihoodWeightingAccuracy(tester, 50);
//  EXPECT_GT(accuracy, 0.600);
//}
//
//TEST_F(ExperimentOnCovertype, covertype_test_approx) {
//  double accuracy = network->EvaluateApproxInferAccuracy(tester, 100000);
//  EXPECT_GT(accuracy, 0.600);
//}
//
//TEST_F(ExperimentOnCovertype, covertype_test_jun_tree_accuracy) {
//  auto *jt = new JunctionTree(network, false);
//  double accuracy = jt->EvaluateJTAccuracy(54, tester);
//  delete jt;
//  EXPECT_GT(accuracy,0.600);
//}
//


//class ExperimentOnAlarm : public ::testing::Test {
//protected:
//
//    void SetUp() override {
//        trainer = new Dataset();
//        tester = new Dataset();
//        network = new Network(true);
//
//        string train_set_file_path = "../../data/alarm_s10000.txt";
//
//        trainer->LoadCSVData(train_set_file_path, true, true, 0);
//        tester->LoadCSVData(train_set_file_path, true, true, 0);
//
//        StructureLearning *bnsl = new ChowLiuTree(network);
//        bnsl->StructLearnCompData(trainer, true);
//
//        ParameterLearning *bnpl = new ParameterLearning(network);
//        bnpl->LearnParamsKnowStructCompData(trainer, true);
//    }
//
//    Dataset *trainer;
//    Dataset *tester;
//    Network *network;
//};
//
//TEST_F(ExperimentOnAlarm, brute_force) {
//    Inference *inference = new ExactInference(network);
//    double accuracy = inference->EvaluateAccuracy(tester, -1, "direct", true);
//    EXPECT_GT(accuracy, 0.600);
//}


//
//class ExperimentOnPhishing : public ::testing::Test {
// protected:
//
//  void SetUp() override {
//    trainer = new Dataset();
//    tester = new Dataset();
//    network = new ChowLiuTree(true);
//
//    string train_set_file_path = "../../data/dataset/Phishing/Phishing_Training_Dataset_split_1_of_10.csv",
//            test_set_file_path = "../../data/dataset/Phishing/Phishing_Training_Dataset_split_9_of_10.csv";
//
//
//    trainer->LoadCSVData(train_set_file_path, false, 30);
//    tester->LoadCSVData(test_set_file_path, false, 30);
//    network->StructLearnCompData(trainer, false);
//    network->LearnParamsKnowStructCompData(trainer, 1, false);
//  }
//
//  Dataset *trainer;
//  Dataset *tester;
//  Network *network;
//};
//
//TEST_F(ExperimentOnPhishing, DISABLED_naive_bayes_brute_force) {
//  ChowLiuTree *net = new ChowLiuTree(true); // TODO: why not network?
//  net->root_node_index = 30; // TODO: check about 30
//  net->ConstructNaiveBayesNetwork(trainer);
//  net->LearnParamsKnowStructCompData(trainer, true);
//  double accuracy = network->EvaluateAccuracyGivenAllCompleteInstances(tester);
//  EXPECT_GT(accuracy, 0.8270);
//}
//
//TEST_F(ExperimentOnPhishing, DISABLED_naive_bayes_var_elim) {
//  ChowLiuTree *net = new ChowLiuTree(true); // TODO
//  net->root_node_index = 30;
//  net->ConstructNaiveBayesNetwork(trainer);
//  net->LearnParamsKnowStructCompData(trainer, 2, false);
//  double accuracy = net->EvaluateVarElimAccuracy(tester);
//  EXPECT_GT(accuracy, 0.8360);
//}
//
//TEST_F(ExperimentOnPhishing, DISABLED_naive_bayes_lik_wei) {
//  ChowLiuTree *net = new ChowLiuTree(true); // TODO
//  net->root_node_index = 30;
//  net->ConstructNaiveBayesNetwork(trainer);
//  net->LearnParamsKnowStructCompData(trainer, true);
//  double accuracy = network->EvaluateLikelihoodWeightingAccuracy(tester, 50);
//  EXPECT_GT(accuracy, 0.8270);
//}
//
//TEST_F(ExperimentOnPhishing, DISABLED_var_elim) {
//  double accuracy = network->EvaluateVarElimAccuracy(tester);
//  EXPECT_GT(accuracy, 0.8250);
//}
//
//TEST_F(ExperimentOnPhishing, DISABLED_brute_force) {
//  double accuracy = network->EvaluateAccuracyGivenAllCompleteInstances(tester);
//  EXPECT_GT(accuracy, 0.8250);
//}
//
//TEST_F(ExperimentOnPhishing, DISABLED_like_weigh) {
//  double accuracy = network->EvaluateLikelihoodWeightingAccuracy(tester, 50);
//  EXPECT_GT(accuracy, 0.6);
//}
//
//TEST_F(ExperimentOnPhishing, DISABLED_approx) {
//  double accuracy = network->EvaluateApproxInferAccuracy(tester, 100000);
//  EXPECT_GT(accuracy, 0.6500);
//}
//
//TEST_F(ExperimentOnPhishing, DISABLED_jun_tree_accuracy) {
//  auto *jt = new JunctionTree(network, false);
//  double accuracy = jt->EvaluateJTAccuracy(30, tester);
//  delete jt;
//  EXPECT_GT(accuracy,0.80);
//}


class ExperimentNetwork : public ::testing::Test {
protected:

    void SetUp() override {
        trainer = new Dataset();
        tester = new Dataset();
        network = new Network(true);

        string train_set_file_path = "../../data/alarm_s10000.txt";

        trainer->LoadCSVData(train_set_file_path, true, true, 0);
        tester->LoadCSVData(train_set_file_path, true, true, 0);

//        ParameterLearning *bnpl = new ParameterLearning(network);
//        bnpl->LearnParamsKnowStructCompData(trainer, true);
    }

    Dataset *trainer;
    Dataset *tester;
    Network *network;
};

TEST_F(ExperimentNetwork, do_nothing) {
    StructureLearning *bnsl = new PCStable(network);
    bnsl->StructLearnCompData(trainer, true);

    for (int i = 0; i < network->num_edges; ++i) {
        Edge edge = network->vec_edges.at(i);
        cout << edge.GetNode1()->node_name << " -- " << edge.GetNode2()->node_name << endl;
    }
    cout << "num nodes = " << network->num_nodes << endl;
    cout << "num edges = " << network->num_edges << endl;
}
