//
// Created by jjt on 25/10/23.
//
#include <iostream>
#include <string>
#include <vector>
#include "gtest/gtest.h"

#include "fastbn/CustomNetwork.h"
#include "fastbn/test_common.h"
#include "fastbn/SampleSetGenerator.h"


class TestSampleSet: public ::testing::Test {
protected:

    void SetUp() override {
        network = new CustomNetwork(true);
    }
    CustomNetwork *network;
};

TEST_F(TestSampleSet, alarm) {
    string net_file = dataset_path + "alarm/alarm.xml";
    network->LoadXMLBIFFile(net_file, ALPHA);

    SampleSetGenerator *sample_set = new SampleSetGenerator(network, 1000, 0);
    sample_set->GenerateSamplesBasedOnCPTs();
    sample_set->OutputLIBSVM(dataset_path + "alarm/alarm_training_libsvm");
}

