//
// Created by jjt on 29/10/23.
//
#include <iostream>
#include <string>
#include <vector>
#include "gtest/gtest.h"

#include "Dataset.h"
#include "Network.h"
#include "StructureLearning.h"
#include "PCStable.h"
#include "Timer.h"

#define GROUP_SIZE 1
#define VERBOSE 1

/**
 * check GetTopoOrd` that generate the topological ordering.
 * we first learn the bayesian network structure, and then generate the topological ordering on the learned DAG.
 */
class TestNetwork : public ::testing::Test {
protected:

    void SetUp() override {
        trainer = new Dataset();
        network = new Network(true);

        bnsl = new PCStable(network, 0.05);
    }

    Dataset *trainer;
    Network *network;
    StructureLearning *bnsl;
};

TEST_F(TestNetwork, alarm_topo) {
    int group_size = GROUP_SIZE;
    int num_threads = 1;
    int verbose  = VERBOSE;

    string train_set_file = "../../dataset/alarm/alarm_s5000.txt";
    trainer->LoadCSVData(train_set_file, true, true, 0);

    bnsl->StructLearnCompData(trainer, group_size, num_threads, true, verbose);

    cout << endl << "generating topo order..." << endl;
    Timer *timer = new Timer();
    timer->Start("topo");
    network->GetTopoOrd();
    timer->Stop("topo");
    timer->Print("topo");
    for (int i = 0; i < network->topo_ord.size(); ++i) {
        cout << network->topo_ord[i] << ", ";
    }
    cout << endl;

    /**
     * check the correctness of the topological ordering based on the resulting edges
     */
    // construct the map of nodes and its position in topological ordering
    vector<int> position(network->topo_ord.size());
    for (int i = 0; i < network->topo_ord.size(); ++i) {
        position[network->topo_ord[i]] = i;
    }
    // traverse each edge, checking if the source node is before the destination node
    bool correct = true;
    for (const Edge& edge: network->vec_edges) {
        int src_index = edge.node1->GetNodeIndex();
        int des_index = edge.node2->GetNodeIndex();
        if (position[src_index] > position[des_index]) {
            correct = false;
            break;
        }
    }
    cout << "correctiveness: " << correct << endl;

    SAFE_DELETE(trainer);
}