//
// Created by jjt on 2021/6/23.
//

#include "fastbo/parameter_learning/ParameterLearning.h"

/**
 * @brief: get each node's conditional probability table
 */
void ParameterLearning::LearnParamsKnowStructCompData(const Dataset *dts, int alpha, bool save_param,
                                                      string param_file, int verbose){
    if (verbose > 0) {
        cout << "==================================================" << '\n'
             << "Begin learning parameters, Laplace smoothing param alpha = " << alpha << endl;
    }

    Timer *timer = new Timer();
    timer->Start("parameter_learning");

    int root_idx = (network->num_nodes == dts->num_vars) ? -1: dts->num_vars;
    network->pts.reserve(network->num_nodes);

    for (int i = 0; i < dts->num_vars; ++i) { // for each variable in the network
        DiscreteNode *this_node = dynamic_cast<DiscreteNode*>(network->FindNodePtrByIndex(i));
        this_node->SetLaplaceSmooth(alpha);

        // construct a simplified pt for this node, mainly to convert configuration to table index.
        PotentialTableBase pt;
        pt.ConstructPotentialTableWithEmptyPotentials(this_node, network);

        // count the frequency.
        vector<int> vec_cond_prob_table_statistics(pt.table_size, 0); // N(AB)
        vector<int> vec_total_count_under_parents_config(pt.cum_levels[0], 0); // N(B)
        if (pt.num_variables == 1) { // if this node has no parent.
            for (int j = 0; j < dts->num_instance; ++j) { // for each instance in the dataset
                int value = dts->dataset_columns[pt.vec_related_variables[0]][j];
                vec_cond_prob_table_statistics[value]++;
                vec_total_count_under_parents_config[0]++;
            }
        } else if (pt.vec_related_variables[1] == root_idx) {
            // if ROOT is the parent of this node, this node only has ROOT as its parent so it stores at index 1.
            // and since ROOT is a virtual node, `dataset_columns` doesn't contain its value, so we randomly select
            // 0 or 1 for it.
            for (int j = 0; j < dts->num_instance; ++j) { // for each instance in the dataset
                // get the values of all the related variables from `dataset_columns`
                int *config = new int[2];
                config[0] = dts->dataset_columns[pt.vec_related_variables[0]][j];
                config[1] = Random01();
                int table_index = pt.GetTableIndexByConfigValue(config);
                int table_index2 = table_index % pt.cum_levels[0]; // cum_levels[0] = table_size / var_dims[0]
                vec_cond_prob_table_statistics[table_index]++;
                vec_total_count_under_parents_config[table_index2]++;
                SAFE_DELETE_ARRAY(config);
            }
        } else { // if this node has parents (not ROOT).
            for (int j = 0; j < dts->num_instance; ++j) { // for each instance in the dataset
                // get the values of all the related variables from `dataset_columns`
                int *config = new int[pt.num_variables];
                for (int k = 0; k < pt.num_variables; ++k) {
                    config[k] = dts->dataset_columns[pt.vec_related_variables[k]][j];
                }
                int table_index = pt.GetTableIndexByConfigValue(config);
                int table_index2 = table_index % pt.cum_levels[0]; // cum_levels[0] = table_size / var_dims[0]
                vec_cond_prob_table_statistics[table_index]++;
                vec_total_count_under_parents_config[table_index2]++;
                SAFE_DELETE_ARRAY(config);
            }
        }

        // compute the probabilities.
        for (int j = 0; j < pt.table_size; ++j) {
            int num1, num2;
            num1 = vec_cond_prob_table_statistics[j];
            if (pt.num_variables == 1) { // if this node has no parent.
                num2 = vec_total_count_under_parents_config[0];
            } else { // if this node has parents.
                num2 = vec_total_count_under_parents_config[j % pt.cum_levels[0]];
            }
            pt.potentials[j] = ((double)num1 + alpha) / ((double)num2 + alpha * pt.var_dims[0]); // P(A|B) = P(AB) / P(B)
        }
        network->pts.push_back(pt);
    }

    // separately handle the virtual ROOT if it exists.
    if (root_idx != -1) {
        // ROOT has no parent. suppose that ROOT has two values, each of which has the same probability.
        DiscreteNode *root = dynamic_cast<DiscreteNode*>(network->FindNodePtrByIndex(root_idx));
        root->SetLaplaceSmooth(alpha);

        // construct a simplified pt for this node, mainly to convert configuration to table index.
        PotentialTableBase pt;
        pt.ConstructPotentialTableWithEmptyPotentials(root, network);

        // compute the probabilities.
        int num1 = 0.5 * dts->num_instance;
        int num2 = dts->num_instance;
        pt.potentials[0] = ((double)num1 + alpha) / ((double)num2 + alpha * pt.var_dims[0]); // P(A|B) = P(AB) / P(B)
        pt.potentials[1] = ((double)num1 + alpha) / ((double)num2 + alpha * pt.var_dims[0]); // P(A|B) = P(AB) / P(B)
        network->pts.push_back(pt);
    }

    timer->Stop("parameter_learning");
    timer->Print("parameter_learning");

    if (save_param) {
        SaveBNParameter(param_file);
    }
}

void ParameterLearning::SaveBNParameter(string param_file) {
    ofstream out_file(param_file);
    if (!out_file.is_open()) {
        fprintf(stderr, "Error in function %s!", __FUNCTION__);
        fprintf(stderr, "Unable to open file %s!", param_file.c_str());
        exit(1);
    }

    for (int i = 0; i < network->num_nodes; ++i) { // for each variable in the network
        DiscreteNode *this_node = dynamic_cast<DiscreteNode*>(network->FindNodePtrByIndex(i));
        string head = "P (" + this_node->node_name + "/v" + to_string(i) + "=";

        PotentialTableBase pt = network->pts[i];
        if (pt.num_variables == 1) {
            // this node has no parent
            for (int j = 0; j < pt.table_size; ++j) {
                string prob = head + this_node->GetValueNameByIndex(j) + "/" + to_string(j)
                        + ") = " + to_string(pt.potentials[j]);
                out_file << prob << endl;
            }
        } else {
            // this node has parent(s).
            int *config = new int[pt.num_variables];
            for (int j = 0; j < pt.table_size; ++j) { // for each entry in this node's pt
                pt.GetConfigValueByTableIndex(j, config);
                // the first element in `config` is the node itself
                string prob = head + this_node->GetValueNameByIndex(config[0])
                        + "/" + to_string(config[0]) + " | ";
                // then handling the left nodes one by one. these nodes are parents of this node
                for (int k = 1; k < pt.num_variables; ++k) {
                    DiscreteNode *par = dynamic_cast<DiscreteNode*>
                            (network->FindNodePtrByIndex(pt.vec_related_variables[k]));
                    prob += par->node_name + "/v" + to_string(pt.vec_related_variables[k]) + "="
                            + par->GetValueNameByIndex(config[k]) + "/" + to_string(config[k]) + ", ";
                }
                prob.pop_back(); prob.pop_back();
                prob += ") = " + to_string(pt.potentials[j]);
                out_file << prob << endl;
            }
        }
    }

    cout << "Learned parameters have been saved in " << param_file << endl;
    out_file.close();
}