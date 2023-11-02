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
 * we first learn the bayesian network structure, and then test the network-related functionalities on the learned DAG.
 */
class TestNetworkAlarm : public ::testing::Test {
protected:

    void SetUp() override {
        trainer = new Dataset();
        string train_set_file = "../../dataset/alarm/alarm_s5000.txt";
        trainer->LoadCSVData(train_set_file, true, true, 0);

        network = new Network(true);

        bnsl = new PCStable(network, 0.05);
        int group_size = GROUP_SIZE;
        int num_threads = 1;
        int verbose  = VERBOSE;

        bnsl->StructLearnCompData(trainer, group_size, num_threads, true, verbose);
        SAFE_DELETE(trainer);
    }

    Dataset *trainer;
    Network *network;
    StructureLearning *bnsl;
};

//TEST_F(TestNetworkAlarm, topo) {
//    cout << endl << "generating topo order..." << endl;
//    Timer *timer = new Timer();
//    timer->Start("topo");
//    network->GetTopoOrd();
//    timer->Stop("topo");
//    timer->Print("topo");
//    for (int i = 0; i < network->topo_ord.size(); ++i) {
//        cout << network->topo_ord[i] << ", ";
//    }
//    cout << endl;
//
//    /**
//     * check the correctness of the topological ordering based on the resulting edges
//     */
//    // construct the map of nodes and its position in topological ordering
//    vector<int> position(network->topo_ord.size());
//    for (int i = 0; i < network->topo_ord.size(); ++i) {
//        position[network->topo_ord[i]] = i;
//    }
//    // traverse each edge, checking if the source node is before the destination node
//    bool correct = true;
//    for (const Edge& edge: network->vec_edges) {
//        int src_index = edge.node1->GetNodeIndex();
//        int des_index = edge.node2->GetNodeIndex();
//        if (position[src_index] > position[des_index]) {
//            correct = false;
//            break;
//        }
//    }
//    cout << "correctiveness: " << correct << endl;
//}

TEST_F(TestNetworkAlarm, roots_for_subgraph) {
    cout << endl << "finding roots for sub-graphs... ";
    Timer *timer = new Timer();
    timer->Start("roots & add");
    auto pc_bnsl = (PCStable*)bnsl;
    vector<int> roots = pc_bnsl->FindRootsInDAGForest();
    pc_bnsl->AddRootNode(roots);
    timer->Stop("roots & add");
    timer->Print("roots & add");

    cout << "roots: ";
    for (int i = 0; i < roots.size(); ++i) {
        cout << roots[i] << ", ";
    }
    cout << endl;

    cout << "network_name = " << network->network_name << endl;
    cout << "num_nodes = " << network->num_nodes << endl;
    cout << "num_edges = " << network->num_edges << endl;
    cout << "pure_discrete = " << network->pure_discrete << endl;

    cout << "topo_ord: ";
    for (int i = 0; i < network->topo_ord.size(); i++) {
        cout << network->topo_ord[i] << " ";
    }
    cout << endl;

    cout << "vec_default_elim_ord: ";
    for (int i = 0; i < network->vec_default_elim_ord.size(); i++) {
        cout << network->vec_default_elim_ord[i] << " ";
    }
    cout << endl;

    cout << "map_idx_node_ptr: ";
    for (const auto &id_n: network->map_idx_node_ptr) {
        cout << id_n.first << endl;
        Node* node = id_n.second;
        cout << "  node_index = " << node->GetNodeIndex() << endl;
        cout << "  node_name = " << node->node_name << endl;
        cout << "  is_discrete = " << node->is_discrete << endl;
        cout << "  set_discrete_parents_combinations: size = " << node->set_discrete_parents_combinations.size() << endl;
        for (const auto &config: node->set_discrete_parents_combinations) {
            cout << "  ";
            for (const auto &varval: config) {
                cout << varval.first << " = " << varval.second << ", ";
            }
            cout << endl;
        }

        cout << " vec_disc_parent_indexes: ";
        for (int i = 0; i < node->vec_disc_parent_indexes.size(); ++i) {
            cout << node->vec_disc_parent_indexes[i] << ", ";
        }
        cout << endl;

        cout << "  set_parent_indexes: ";
        for (const int &p: node->set_parent_indexes) {
            cout << p << ", ";
        }
        cout << endl;

        cout << "  set_children_indexes: ";
        for (const int &c: node->set_children_indexes) {
            cout << c << ", ";
        }
        cout << endl;

        DiscreteNode* disc_node = (DiscreteNode*)node;
        cout << "  possible_values_ids: ";
        for (const auto &val_id: disc_node->possible_values_ids) {
            cout << val_id.first << ", " << val_id.second << "; ";
        }
        cout << endl;

        cout << "map_cond_prob_table_statistics: " << endl;
        for (const auto &m: disc_node->map_cond_prob_table_statistics) {
            cout << m.first << ": ";
            map<DiscreteConfig, int> m2 = m.second;
            for (const auto &mm: m2) {
                DiscreteConfig config = mm.first;
                for (const auto &s: config) {
                    cout << s.first << " = " << s.second << " ";
                }
                cout << ": " << mm.second << ", ";
            }
            cout << endl;
        }

        cout << "map_cond_prob: " << endl;
        for (const auto &m: disc_node->map_cond_prob) {
            cout << m.first << ": ";
            map<DiscreteConfig, double> m2 = m.second;
            for (const auto &mm: m2) {
                DiscreteConfig config = mm.first;
                for (const auto &s: config) {
                    cout << s.first << " = " << s.second << " ";
                }
                cout << ": " << mm.second << ", ";
            }
            cout << endl;
        }

        cout << "map_total_count_under_parents_config: " << endl;
        for (const auto &mm: disc_node->map_total_count_under_parents_config) {
            DiscreteConfig config = mm.first;
            for (const auto &s: config) {
                cout << s.first << " = " << s.second << " ";
            }
            cout << ": " << mm.second << ", ";
        }
        cout << endl;

        cout << "num_potential_vals = " << disc_node->GetDomainSize() << endl;
    }

    cout << "vec_edges: size = " << network->vec_edges.size() << endl;
}