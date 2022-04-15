//
// Created by jjt on 2022/4/5.
//

#include <iostream>
#include <vector>
#include "gtest/gtest.h"
#include "gadget.h"

#include "Node.h"
#include "DiscreteNode.h"
#include "Network.h"
#include "CustomNetwork.h"

/**
 * this text was used when testing the correctness of the xmlbif parser
 * now it is used to generate random samples from the input xmlbif reference BNs
 * for this purpose, go to XMLBIFParser.cpp, find vector<Node*> XMLBIFParser::GetConnectedNodes()
 *                   remember to use "./test_exe 2>&1 | tee filename"
 */
//class XMLTest: public ::testing::Test {
//protected:
//
//    void SetUp() override {
//        network = new CustomNetwork(true);
//
//        string file_path = "/home/zeyiwen/jiantong/BN/dataset/win95pts/win95pts.xml";
//        network->GetNetFromXMLBIFFile(file_path);
//    }
//    CustomNetwork *network;
//};
//
//TEST_F(XMLTest, empty) {}

//TEST_F(XMLTest, print) {
//    cout << "network name: " << network->network_name << endl;
//    for (auto &node: network->map_idx_node_ptr) {
//        Node *node_ptr = node.second;
//        int node_index = node.first;
//        cout << "node index = " << node_index << ", node name = " << node_ptr->node_name << endl;
//
//        cout << "set parent indexes = ";
//        for (auto &par: node_ptr->set_parent_indexes) {
//            cout << par << " ";
//        }
//        cout << endl;
//
//        cout << "vec disc parent indexes = ";
//        for (auto &par: node_ptr->vec_disc_parent_indexes) {
//            cout << par << " ";
//        }
//        cout << endl;
//
//        cout << "set children indexes = ";
//        for (auto &child: node_ptr->set_children_indexes) {
//            cout << child << " ";
//        }
//        cout << endl;
//
//        cout << "map disc parents domain size = ";
//        for (auto &par: node_ptr->map_disc_parents_domain_size) {
//            cout << par.first << ", " << par.second << "; ";
//        }
//        cout << endl;
//
//        cout << "set discrete parents combinations: ";
//        for (auto &single_par_config: node_ptr->set_discrete_parents_combinations) {
//            cout << "config: ";
//            for (auto &pair: single_par_config) {
//                cout << pair.first << "=" << pair.second << " ";
//            }
//        }
//        cout << endl;
//
//        DiscreteNode* dis_node = dynamic_cast<DiscreteNode*>(node_ptr);
//        cout << "num potential vals = " << dis_node->GetNumPotentialVals() << endl;
//
//        cout << "vec str potential vals = ";
//        for (auto &str: dis_node->vec_str_potential_vals) {
//            cout << str << " ";
//        }
//        cout << endl;
//
//        cout << "vec potential vals = ";
//        for (auto &val: dis_node->vec_potential_vals) {
//            cout << val << " ";
//        }
//        cout << endl;
//
//        if (dis_node->HasParents()) {    // If this node has parents
//            for(int i = 0; i < dis_node->GetDomainSize(); ++i) {    // For each head variable of CPT (i.e., value of child)
//                int query = dis_node->vec_potential_vals[i];
//                auto it = dis_node->set_discrete_parents_combinations.begin();
//                for (int j = 0; j < dis_node->GetNumParentsConfig(); ++j){  // For tail variables of CPT (i.e., parent configuration)
//                    DiscreteConfig parcfg = *it;
//
//                    cout << "query " << query << ", ";
//                    cout << "config ";
//                    for (auto &pair: parcfg) {
//                        cout << pair.first << "=" << pair.second << " ";
//                    }
//                    cout << ": " << dis_node->GetProbability(query, parcfg) << endl;
//                    ++it;
//                }
//            }
//        } else {
//            DiscreteConfig parcfg;
//            for(int i = 0; i < dis_node->GetDomainSize(); ++i) {    // For each row of CPT
//                int query = dis_node->vec_potential_vals[i];
//                cout << "query " << query;
//                cout << ": " << dis_node->GetProbability(query, parcfg) << endl;
//            }
//        }
//
//    }
//    cout << "finish" << endl;
//}