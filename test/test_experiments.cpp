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
#include "CustomNetwork.h"
#include "BNSLComparison.h"
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




class ExperimentBNSLOnAlarm : public ::testing::Test {
protected:

    void SetUp() override {
        trainer = new Dataset();
        network = new Network(true);

        string train_set_file_path = "../../data/alarm_s10000.txt";

        trainer->LoadCSVData(train_set_file_path, true, true, 0);

//        ParameterLearning *bnpl = new ParameterLearning(network);
//        bnpl->LearnParamsKnowStructCompData(trainer, true);
    }

    Dataset *trainer;
    Network *network;
};

TEST_F(ExperimentBNSLOnAlarm, pc_stable) {
    StructureLearning *bnsl = new PCStable(network, trainer, 0.05);
    bnsl->StructLearnCompData(trainer, true, false);

    CustomNetwork *ref_net = new CustomNetwork();
    ref_net->LoadBIFFile("../../data/alarm.bif");
//    ref_net->PrintEachEdgeWithIndex();

    BNSLComparison comp(ref_net, network);
    int shd = comp.GetSHD();
    cout << "SHD = " << shd << endl;
//    EXPECT_EQ(shd, 3);
//    EXPECT_EQ(((PCStable*)bnsl)->num_ci_test, 6265);
//    EXPECT_EQ(((PCStable*)bnsl)->num_dependence_judgement, 5643);
}


//class ExperimentBNSLOnInsurance : public ::testing::Test {
//protected:
//
//    void SetUp() override {
//        trainer = new Dataset();
//        network = new Network(true);
//
//        string train_set_file_path = "../../data/insurance_s10000.txt";
//
//        trainer->LoadCSVData(train_set_file_path, true, true, 0);
//
////        ParameterLearning *bnpl = new ParameterLearning(network);
////        bnpl->LearnParamsKnowStructCompData(trainer, true);
//    }
//
//    Dataset *trainer;
//    Network *network;
//};
//
//TEST_F(ExperimentBNSLOnInsurance, pc_stable) {
//    StructureLearning *bnsl = new PCStable(network, trainer, 0.05);
//    bnsl->StructLearnCompData(trainer, true, false);
//
//    CustomNetwork *ref_net = new CustomNetwork();
//    ref_net->LoadBIFFile("../../data/insurance.bif");
////    ref_net->PrintEachEdgeWithIndex();
//
//    BNSLComparison comp(ref_net, network);
//    int shd = comp.GetSHD();
//    cout << "SHD = " << shd << endl;
////    EXPECT_EQ(shd, 3);
////    EXPECT_EQ(((PCStable*)bnsl)->num_ci_test, 6265);
////    EXPECT_EQ(((PCStable*)bnsl)->num_dependence_judgement, 5643);
//}

//class ExperimentBNSLOnWin95pts : public ::testing::Test {
//protected:
//
//    void SetUp() override {
//        trainer = new Dataset();
//        network = new Network(true);
//
//        string train_set_file_path = "../../data/win95pts_s100000.txt";
//
//        trainer->LoadCSVData(train_set_file_path, true, true, 0);
//
////        ParameterLearning *bnpl = new ParameterLearning(network);
////        bnpl->LearnParamsKnowStructCompData(trainer, true);
//    }
//
//    Dataset *trainer;
//    Network *network;
//};
//
//TEST_F(ExperimentBNSLOnWin95pts, pc_stable) {
//    StructureLearning *bnsl = new PCStable(network, trainer, 0.05);
//    bnsl->StructLearnCompData(trainer, true, false);
//
//    CustomNetwork *ref_net = new CustomNetwork();
//    ref_net->LoadBIFFile("../../data/win95pts.bif");
////    ref_net->PrintEachEdgeWithIndex();
//
//    BNSLComparison comp(ref_net, network);
//    int shd = comp.GetSHD();
//    cout << "SHD = " << shd << endl;
////    EXPECT_EQ(shd, 31);
////    EXPECT_EQ(((PCStable*)bnsl)->num_ci_test, 21346);
////    EXPECT_EQ(((PCStable*)bnsl)->num_dependence_judgement, 18589);
//}


//class ExperimentBNSLOnHailfinder : public ::testing::Test {
//protected:
//
//    void SetUp() override {
//        trainer = new Dataset();
//        network = new Network(true);
//
//        string train_set_file_path = "../../data/hailfinder_s10000.txt";
//
//        trainer->LoadCSVData(train_set_file_path, true, true, 0);
//
////        ParameterLearning *bnpl = new ParameterLearning(network);
////        bnpl->LearnParamsKnowStructCompData(trainer, true);
//    }
//
//    Dataset *trainer;
//    Network *network;
//};
//
//TEST_F(ExperimentBNSLOnHailfinder, pc_stable) {
//    StructureLearning *bnsl = new PCStable(network, trainer, 0.05);
//    bnsl->StructLearnCompData(trainer, true, false);
//
//    CustomNetwork *ref_net = new CustomNetwork();
//    ref_net->LoadBIFFile("../../data/hailfinder.bif");
////    ref_net->PrintEachEdgeWithIndex();
//
//    BNSLComparison comp(ref_net, network);
//    int shd = comp.GetSHD();
//    cout << "SHD = " << shd << endl;
////    EXPECT_EQ(shd, 3);
////    EXPECT_EQ(((PCStable*)bnsl)->num_ci_test, 6265);
////    EXPECT_EQ(((PCStable*)bnsl)->num_dependence_judgement, 5643);
//}

//class ExperimentBNSLOnAndes : public ::testing::Test {
//protected:
//
//    void SetUp() override {
//        trainer = new Dataset();
//        tester = new Dataset();
//        network = new Network(true);
//
//        string train_set_file_path = "../../data/andes_s10000.txt";
//
//        trainer->LoadCSVData(train_set_file_path, true, true, 0);
//        tester->LoadCSVData(train_set_file_path, true, true, 0);
//
////        ParameterLearning *bnpl = new ParameterLearning(network);
////        bnpl->LearnParamsKnowStructCompData(trainer, true);
//    }
//
//    Dataset *trainer;
//    Dataset *tester;
//    Network *network;
//};
//
//TEST_F(ExperimentBNSLOnAndes, pc_stable) {
//    StructureLearning *bnsl = new PCStable(network, trainer, 0.05);
//    bnsl->StructLearnCompData(trainer, true, false);
//
//    CustomNetwork *ref_net = new CustomNetwork();
//    ref_net->LoadBIFFile("../../data/andes.bif");
////    ref_net->PrintEachEdgeWithIndex();
//
//    BNSLComparison comp(ref_net, network);
//    int shd = comp.GetSHD();
//    cout << "SHD = " << shd << endl;
////    EXPECT_EQ(shd, 3);
////    EXPECT_EQ(((PCStable*)bnsl)->num_ci_test, 6265);
////    EXPECT_EQ(((PCStable*)bnsl)->num_dependence_judgement, 5643);
//}

class ExperimentBNSLOnPigs : public ::testing::Test {
protected:

    void SetUp() override {
        trainer = new Dataset();
        network = new Network(true);

        string train_set_file_path = "../../data/pigs_s10000.txt";

        trainer->LoadCSVData(train_set_file_path, true, true, 0);

//        ParameterLearning *bnpl = new ParameterLearning(network);
//        bnpl->LearnParamsKnowStructCompData(trainer, true);
    }

    Dataset *trainer;
    Network *network;
};

TEST_F(ExperimentBNSLOnPigs, pc_stable) {
    StructureLearning *bnsl = new PCStable(network, trainer, 0.05);
    bnsl->StructLearnCompData(trainer, true, true);

    CustomNetwork *ref_net = new CustomNetwork();
    ref_net->LoadBIFFile("../../data/pigs.bif");
//    ref_net->PrintEachEdgeWithIndex();

    BNSLComparison comp(ref_net, network);
    int shd = comp.GetSHD();
    cout << "SHD = " << shd << endl;
//    EXPECT_EQ(shd, 3);
//    EXPECT_EQ(((PCStable*)bnsl)->num_ci_test, 6265);
//    EXPECT_EQ(((PCStable*)bnsl)->num_dependence_judgement, 5643);
}
