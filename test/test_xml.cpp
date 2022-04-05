//
// Created by jjt on 2022/4/5.
//

#include <iostream>
#include <vector>
#include "gtest/gtest.h"
#include "gadget.h"

#include "XMLBIFParser.h"
#include "Node.h"
#include "DiscreteNode.h"

class XMLTest: public ::testing::Test {
protected:

    void SetUp() override {
        string file_path = "/home/zeyiwen/jiantong/BN/Bayesian-network/data/interchange-format-file/dog-problem.xml";
        xmlbif_parser = new XMLBIFParser(file_path);
    }
    XMLBIFParser *xmlbif_parser;
};

TEST_F(XMLTest, dog_node) {
    vector<Node*> nodes = xmlbif_parser->GetConnectedNodes();
    for (auto &node: nodes) {
        cout << "node index = " << node->GetNodeIndex() << ", node name = " << node->node_name << endl;

        cout << "set parent indexes = ";
        for (auto &par: node->set_parent_indexes) {
            cout << par << " ";
        }
        cout << endl;

        cout << "vec disc parent indexes = ";
        for (auto &par: node->vec_disc_parent_indexes) {
            cout << par << " ";
        }
        cout << endl;

        cout << "set children indexes = ";
        for (auto &child: node->set_children_indexes) {
            cout << child << " ";
        }
        cout << endl;

        cout << "map disc parents domain size = ";
        for (auto &par: node->map_disc_parents_domain_size) {
            cout << par.first << ", " << par.second << "; ";
        }
        cout << endl;

        cout << "set discrete parents combinations: ";
        for (auto &single_par_config: node->set_discrete_parents_combinations) {
            cout << "config: ";
            for (auto &pair: single_par_config) {
                cout << pair.first << "=" << pair.second << " ";
            }
        }
        cout << endl;

        DiscreteNode* dis_node = dynamic_cast<DiscreteNode*>(node);
        cout << "num potential vals = " << dis_node->GetNumPotentialVals() << endl;

        cout << "vec str potential vals = ";
        for (auto &str: dis_node->vec_str_potential_vals) {
            cout << str << " ";
        }
        cout << endl;

        cout << "vec potential vals = ";
        for (auto &val: dis_node->vec_potential_vals) {
            cout << val << " ";
        }
        cout << endl;

        if (dis_node->HasParents()) {    // If this node has parents
            for(int i = 0; i < dis_node->GetDomainSize(); ++i) {    // For each head variable of CPT (i.e., value of child)
                int query = dis_node->vec_potential_vals[i];
                auto it = dis_node->set_discrete_parents_combinations.begin();
                for (int j = 0; j < dis_node->GetNumParentsConfig(); ++j){  // For tail variables of CPT (i.e., parent configuration)
                    DiscreteConfig parcfg = *it;

                    cout << "query " << query << ", ";
                    cout << "config ";
                    for (auto &pair: parcfg) {
                        cout << pair.first << "=" << pair.second << " ";
                    }
                    cout << ": " << dis_node->GetProbability(query, parcfg) << endl;
                    ++it;
                }
            }
        } else {
            DiscreteConfig parcfg;
            for(int i = 0; i < dis_node->GetDomainSize(); ++i) {    // For each row of CPT
                int query = dis_node->vec_potential_vals[i];
                cout << "query " << query;
                cout << ": " << dis_node->GetProbability(query, parcfg) << endl;
            }
        }

    }
    cout << "finish" << endl;
}