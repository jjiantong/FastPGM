//
// Created by jjt on 25/10/23.
//
#include <iostream>
#include <string>
#include <vector>
#include "gtest/gtest.h"

#include "fastbn/Dataset.h"
#include "fastbn/Network.h"
#include "fastbn/structure_learning/StructureLearning.h"
#include "fastbn/parameter_learning/ParameterLearning.h"
#include "fastbn/structure_learning/PCStable.h"
#include "fastbn/inference/Inference.h"
#include "fastbn/inference/JunctionTree.h"
#include "fastbn/test_common.h"


class TestClassificationAlarm : public ::testing::Test {
protected:

    void SetUp() override {
        trainer = new Dataset();
        string train_set_file = dataset_path + "alarm/alarm_s5000.txt";
        trainer->LoadCSVTrainingData(train_set_file, true, true, 0);

//        string train_set_file = dataset_path + "alarm/test4.txt";
//        trainer->LoadLIBSVMTrainingData(train_set_file, 0);

        tester = new Dataset(trainer);
//        string test_set_file = dataset_path + "alarm/testing_alarm_1k_p20";
//        tester->LoadLIBSVMTestingData(test_set_file, 0);

        string test_set_file = dataset_path + "alarm/testing_alarm_1k_p20_csv.txt";
        tester->LoadCSVTestingData(test_set_file, true, true, 0);

        network = new Network(true);
        bnsl = new PCStable(network, 0.05);
        bnpl = new ParameterLearning(network);
    }

    Dataset *trainer;
    Dataset *tester;
    Network *network;
    StructureLearning *bnsl;
    ParameterLearning *bnpl;
};

TEST_F(TestClassificationAlarm, thread_1) {
    int group_size = GROUP_SIZE;
    int num_threads = 1;
    int verbose  = VERBOSE;

    string struct_file = dataset_path + "alarm/alarm_1k_p20_struct.txt";
    bnsl->StructLearnCompData(trainer, group_size, num_threads,
                              true, true, true, struct_file, verbose);
    cout << endl;
    bnsl->network->CheckEdges();

    string param_file = dataset_path + "alarm/alarm_1k_p20_param.txt";
    bnpl->LearnParamsKnowStructCompData(trainer, 1, true, param_file, verbose); // todo: alpha = 1
    SAFE_DELETE(trainer);

    Inference *inference = new JunctionTree(true, network, tester);
    SAFE_DELETE(tester);

    string pt_file = dataset_path + "dataset/alarm/alarm_1k_pt";
    double accuracy = inference->EvaluateAccuracy(pt_file, num_threads);
    cout << "accuracy = " << accuracy << endl;
    SAFE_DELETE(inference);
    SAFE_DELETE(bnsl);
    SAFE_DELETE(bnpl);
}

