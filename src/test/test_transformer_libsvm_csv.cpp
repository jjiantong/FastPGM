//
// Created by jjt on 25/11/23.
//
#include <iostream>
#include <string>
#include <vector>
#include "gtest/gtest.h"

#include "fastbo/Dataset.h"

string dataset_path = "../../../dataset/";

class TestClassificationAlarm : public ::testing::Test {
protected:

    void SetUp() override {
        trainer = new Dataset();
        string train_set_file = dataset_path + "alarm/alarm_s5000.txt";
        trainer->LoadCSVTrainingData(train_set_file, true, true, 0);

        tester = new Dataset(trainer);
        string test_set_file = dataset_path + "alarm/testing_alarm_1k_p20";
        tester->LoadLIBSVMTestingData(test_set_file, trainer->num_vars,0);
    }

    Dataset *trainer;
    Dataset *tester;
};

TEST_F(TestClassificationAlarm, thread_1) {
    tester->vec_var_names = trainer->vec_var_names;

    string csv_file = dataset_path + "alarm/test3.txt";
    tester->StoreCSVData(csv_file, true, true);

    SAFE_DELETE(trainer);
    SAFE_DELETE(tester);
}

