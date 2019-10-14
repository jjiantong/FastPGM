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
#include "ChowLiuTree.h"
#include "JunctionTree.h"
#include "CustomNetwork.h"
#include "ScoreFunction.h"
#include "gadget.h"

class ExperimentOnCovertype : public ::testing::Test {
 protected:

  void SetUp() override {
    trainer = new Dataset();
    tester = new Dataset();
    network = new ChowLiuTree(true);

    string train_set_file_path = "../../data/dataset/Covertype/covtype_discretization_train.data",
           test_set_file_path = "../../data/dataset/Covertype/covtype_discretization_test.data";


    trainer->LoadCSVDataAutoDetectConfig(train_set_file_path, false, 54);
    tester->LoadCSVDataAutoDetectConfig(test_set_file_path, false, 54);
    network->StructLearnCompData(trainer, true);
    network->LearnParamsKnowStructCompData(trainer, true);
  }


  Dataset *trainer;
  Dataset *tester;
  Network *network;
};

TEST_F(ExperimentOnCovertype, covertype_test_var_elim) {
  double accuracy = network->TestNetReturnAccuracy(tester);
  EXPECT_GT(accuracy, 0.65);
}

TEST_F(ExperimentOnCovertype, covertype_test_like_weigh) {
  double accuracy = network->TestAccuracyByLikelihoodWeighting(tester, 50);
  EXPECT_GT(accuracy, 0.65);
}

TEST_F(ExperimentOnCovertype, covertype_test_approx) {
  double accuracy = network->TestNetByApproxInferReturnAccuracy(tester,100000);
  EXPECT_GT(accuracy, 0.65);
}

TEST_F(ExperimentOnCovertype, covertype_test_jun_tree_accuracy) {
  auto *jt = new JunctionTree(network, false);
  double accuracy = jt->TestNetReturnAccuracy(54,tester);
  delete jt;
  EXPECT_GT(accuracy,0.65);
}


class ExperimentOnPhishing : public ::testing::Test {
 protected:

  void SetUp() override {
    trainer = new Dataset();
    tester = new Dataset();
    network = new ChowLiuTree(true);

    string train_set_file_path = "../../data/dataset/Phishing/Phishing_Training_Dataset_split_1_of_10.csv",
            test_set_file_path = "../../data/dataset/Phishing/Phishing_Training_Dataset_split_9_of_10.csv";


    trainer->LoadCSVDataAutoDetectConfig(train_set_file_path, false, 30);
    tester->LoadCSVDataAutoDetectConfig(test_set_file_path, false, 30);
    network->StructLearnCompData(trainer, false);
    network->LearnParamsKnowStructCompData(trainer, false);
  }


  Dataset *trainer;
  Dataset *tester;
  Network *network;
};

TEST_F(ExperimentOnPhishing, naive_bayes_var_elim) {
  ChowLiuTree *net = new ChowLiuTree(true);
  net->root_node_index = 30;
  net->ConstructNaiveBayesNetwork(trainer);
  net->LearnParamsKnowStructCompData(trainer, true);

  ScoreFunction sf(net, tester);
  cout << "Scores\n"
       << "LogLikelihood: " << sf.LogLikelihood()  << '\n'
       << "AIC: " <<  sf.AIC() << '\n'
       << "BIC: " <<  sf.BIC() << '\n'
       << "LogK2: " <<  sf.LogK2()  << '\n'
       << "LogBDeu: " <<  sf.LogBDeu() << endl;

  double accuracy = net->TestNetReturnAccuracy(tester);
  EXPECT_GT(accuracy, 0.8);
}

TEST_F(ExperimentOnPhishing, naive_bayes_lik_wei) {
  ChowLiuTree *net = new ChowLiuTree(true);
  net->root_node_index = 30;
  net->ConstructNaiveBayesNetwork(trainer);
  net->LearnParamsKnowStructCompData(trainer, true);
  double accuracy = network->TestAccuracyByLikelihoodWeighting(tester, 500);
  EXPECT_GT(accuracy, 0.601);
}

TEST_F(ExperimentOnPhishing, var_elim) {
  double accuracy = network->TestNetReturnAccuracy(tester);
  EXPECT_GT(accuracy, 0.8);
}

TEST_F(ExperimentOnPhishing, like_weigh) {
  double accuracy = network->TestAccuracyByLikelihoodWeighting(tester, 50);
  EXPECT_GT(accuracy, 0.6);
}

TEST_F(ExperimentOnPhishing,approx) {
  double accuracy = network->TestNetByApproxInferReturnAccuracy(tester,100000);
  EXPECT_GT(accuracy, 0.65);
}

TEST_F(ExperimentOnPhishing, jun_tree_accuracy) {
  auto *jt = new JunctionTree(network, false);
  double accuracy = jt->TestNetReturnAccuracy(30, tester);
  delete jt;
  EXPECT_GT(accuracy,0.8);
}

class ExperimentOnA1a : public ::testing::Test {
 protected:

  void SetUp() override {
    trainer = new Dataset();
    tester = new Dataset();
    network = new ChowLiuTree(true);

    string train_set_file_path = "../../data/dataset/a1a.txt",
            test_set_file_path = "../../data/dataset/a1a.test.txt";


    trainer->LoadLIBSVMDataAutoDetectConfig(train_set_file_path);
    tester->LoadLIBSVMDataAutoDetectConfig(test_set_file_path);
    network->StructLearnCompData(trainer, false);
    network->LearnParamsKnowStructCompData(trainer, false);
  }


  Dataset *trainer;
  Dataset *tester;
  Network *network;
};

TEST_F(ExperimentOnA1a, var_elim) {
  double accuracy = network->TestNetReturnAccuracy(tester);
  EXPECT_GT(accuracy, 0.65);
}

TEST_F(ExperimentOnA1a, like_weigh) {
  double accuracy = network->TestAccuracyByLikelihoodWeighting(tester, 50);
  EXPECT_GT(accuracy, 0.65);
}

TEST_F(ExperimentOnA1a, jun_tree_accuracy) {
  auto *jt = new JunctionTree(network, false);
  double accuracy = jt->TestNetReturnAccuracy(0,tester);
  delete jt;
  EXPECT_GT(accuracy,0.65);
}

TEST_F(ExperimentOnA1a, approx) {
  double accuracy = network->TestNetByApproxInferReturnAccuracy(tester,100000);
  EXPECT_GT(accuracy, 0.65);
}

