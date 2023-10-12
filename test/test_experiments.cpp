////
//// Created by LinjianLi on 2019/10/7.
////
//
//#include <iostream>
//#include <string>
//#include <vector>
//#include "gtest/gtest.h"
//#include "tinyxml2.h"
//
//#include "Dataset.h"
//#include "Network.h"
//#include "ChowLiuTree.h"
//#include "JunctionTree.h"
//#include "CustomNetwork.h"
//#include "ScoreFunction.h"
//#include "gadget.h"
//
//class ExperimentOnCovertype : public ::testing::Test {
// protected:
//
//  void SetUp() override {
//    trainer = new Dataset();
//    tester = new Dataset();
//    network = new ChowLiuTree(true);
//
//    string train_set_file_path = "../../data/dataset/Covertype/covtype_discretization_train.data",
//           test_set_file_path = "../../data/dataset/Covertype/covtype_discretization_test.data";
//
//
//    trainer->LoadCSVDataAutoDetectConfig(train_set_file_path, false, 54);
//    tester->LoadCSVDataAutoDetectConfig(test_set_file_path, false, 54);
//    network->StructLearnCompData(trainer, true);
//    network->LearnParamsKnowStructCompData(trainer, true);
//  }
//
//
//  Dataset *trainer;
//  Dataset *tester;
//  Network *network;
//};
//
//TEST_F(ExperimentOnCovertype, covertype_test_var_elim) {
//  double accuracy = network->EvaluateVarElimAccuracy(tester);
//  EXPECT_GT(accuracy, 0.600);
//}
//
//TEST_F(ExperimentOnCovertype, DISABLED_covertype_test_brute_force) {
//  double accuracy = network->EvaluateAccuracyGivenAllCompleteInstances(tester);
//  EXPECT_GT(accuracy, 0.600);
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
//    trainer->LoadCSVDataAutoDetectConfig(train_set_file_path, false, 30);
//    tester->LoadCSVDataAutoDetectConfig(test_set_file_path, false, 30);
//    network->StructLearnCompData(trainer, false);
//    network->LearnParamsKnowStructCompData(trainer, 1, false);
//  }
//
//
//  Dataset *trainer;
//  Dataset *tester;
//  Network *network;
//};
//
//TEST_F(ExperimentOnPhishing, DISABLED_naive_bayes_brute_force) {
//  ChowLiuTree *net = new ChowLiuTree(true);
//  net->root_node_index = 30;
//  net->ConstructNaiveBayesNetwork(trainer);
//  net->LearnParamsKnowStructCompData(trainer, true);
//  double accuracy = network->EvaluateAccuracyGivenAllCompleteInstances(tester);
//  EXPECT_GT(accuracy, 0.8270);
//}
//
//TEST_F(ExperimentOnPhishing, DISABLED_naive_bayes_var_elim) {
//  ChowLiuTree *net = new ChowLiuTree(true);
//  net->root_node_index = 30;
//  net->ConstructNaiveBayesNetwork(trainer);
//  net->LearnParamsKnowStructCompData(trainer, 2, false);
//  double accuracy = net->EvaluateVarElimAccuracy(tester);
//  EXPECT_GT(accuracy, 0.8360);
//}
//
//TEST_F(ExperimentOnPhishing, DISABLED_naive_bayes_lik_wei) {
//  ChowLiuTree *net = new ChowLiuTree(true);
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
//
//class ExperimentOnA1a : public ::testing::Test {
// protected:
//
//  void SetUp() override {
//    trainer = new Dataset();
//    tester = new Dataset();
//    network = new ChowLiuTree(true);
//
//    string train_set_file_path = "../../data/dataset/a1a.txt",
//            test_set_file_path = "../../data/dataset/a1a.test.txt";
//
//
//    trainer->LoadLIBSVMDataAutoDetectConfig(train_set_file_path);
//    tester->LoadLIBSVMDataAutoDetectConfig(test_set_file_path);
//    network->StructLearnCompData(trainer, false);
//    network->LearnParamsKnowStructCompData(trainer, 2, false);
//  }
//
//
//  Dataset *trainer;
//  Dataset *tester;
//  Network *network;
//};
//
//TEST_F(ExperimentOnA1a, DISABLED_var_elim) {
//  double accuracy = network->EvaluateVarElimAccuracy(tester);
//  EXPECT_GT(accuracy, 0.8230);
//}
//
//TEST_F(ExperimentOnA1a, DISABLED_brute_force) {
//  double accuracy = network->EvaluateAccuracyGivenAllCompleteInstances(tester);
//  EXPECT_GT(accuracy, 0.8230);
//}
//
//TEST_F(ExperimentOnA1a, DISABLED_like_weigh) {
//  double accuracy = network->EvaluateLikelihoodWeightingAccuracy(tester, 50);
//  EXPECT_GT(accuracy, 0.8150);
//}
//
//TEST_F(ExperimentOnA1a, DISABLED_jun_tree_accuracy) {
//  auto *jt = new JunctionTree(network, false);
//  double accuracy = jt->EvaluateJTAccuracy(0, tester);
//  delete jt;
//  EXPECT_GT(accuracy,0.8150);
//}
//
//TEST_F(ExperimentOnA1a, approx) {
//  double accuracy = network->EvaluateApproxInferAccuracy(tester, 100000);
//  EXPECT_GT(accuracy, 0.650);
//}
//
