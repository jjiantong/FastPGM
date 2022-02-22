//
// Created by jjt on 2022/2/22.
//

#include "PotentialTable.h"

/**
 * @brief: construct a potential table given a node;
 * the table consists of the node and all the existing related_variables, i.e., all its parents.
 */
PotentialTable::PotentialTable(DiscreteNode *disc_node, Network *net) {
    int node_index = disc_node->GetNodeIndex();
    int node_dim = disc_node->GetDomainSize();

    related_variables.insert(node_index); // related_variables is empty initially, because this is a constructor

    // If this disc_node has no parents.
    if (!disc_node->HasParents()) {
        num_variables = 1;
        var_dims.reserve(num_variables);
        var_dims.push_back(node_dim);
        cum_levels.reserve(num_variables);
        cum_levels.push_back(1);
        table_size = node_dim;

        potentials.reserve(table_size);
        DiscreteConfig empty_par_config;
        for (int i = 0; i < table_size; ++i) {
            // potentials[i]
            potentials.push_back(disc_node->GetProbability(disc_node->vec_potential_vals.at(i), empty_par_config));
        }
    }
    else {// If this disc_node has parents
        related_variables.insert(disc_node->set_parent_indexes.begin(), disc_node->set_parent_indexes.end());
        num_variables = related_variables.size();

        var_dims.reserve(num_variables);
        for (auto &node_idx: related_variables) {
            if (node_idx == node_index) { // it is exactly this node
                var_dims.push_back(node_dim);
            } else { // if not, find its domain
                var_dims.push_back(dynamic_cast<DiscreteNode*>(net->FindNodePtrByIndex(node_idx))->GetDomainSize());
            }
        }

        cum_levels.resize(num_variables);
        // set the right-most one ...
        cum_levels[num_variables - 1] = 1;
        // ... then compute the left ones
        for (int i = num_variables - 2; i >= 0; --i) {
            cum_levels[i] = cum_levels[i + 1] * var_dims[i + 1];
        }
        // compute the table size -- number of possible configurations
        table_size = cum_levels[0] * var_dims[0];

        potentials.reserve(table_size);
        for (int i = 0; i < table_size; ++i) {
            // find the discrete config by the table index, the config contains this node and its parents
            DiscreteConfig parent_config = GetConfigByTableIndex(i, net);
            int node_value;
            // we need the config for its parents -- remove the node
            for (auto it = parent_config.begin(); it != parent_config.end();) {
                if ((*it).first == node_index) { // find the node
                    node_value = (*it).second; // mark its value
                    parent_config.erase(it++); // erase from the parent config
                } else {
                    it++;
                }
            }
            // potentials[i]
            potentials.push_back(disc_node->GetProbability(node_value, parent_config));
        }
    }//end has parents
}

/*!
 * @brief: get discrete configuration given table index
 * note: DiscreteConfig: set< pair<int, int> >,
 *       the second int of pair is the value itself, not the index of the value!!
 */
DiscreteConfig PotentialTable::GetConfigByTableIndex(int table_index, Network *net) {
    vector<int> config_value = GetConfigValueByTableIndex(table_index);
    DiscreteConfig config;
    int i = 0;
    for (auto &v: related_variables) {
        DiscreteNode *node = dynamic_cast<DiscreteNode*>(net->FindNodePtrByIndex(v));
        pair<int, int> tmp_pair;
        tmp_pair.first = v;
        tmp_pair.second = node->vec_potential_vals.at(config_value[i++]);
        config.insert(tmp_pair);
    }
    return config;
}

/*!
 * @brief: get each value (index) of the configuration corresponding to the given table index
 * @method: a/b = c...d, from left to right
 *          a -- the table index at the beginning, and then d
 *          b -- cum_levels[i]
 *          c -- save in config_value
 */
vector<int> PotentialTable::GetConfigValueByTableIndex(int table_index) {
    vector<int> config_value;
    config_value.resize(num_variables);
    int a = table_index;
    for (int i = 0; i < num_variables; ++i) {
        int c = a / cum_levels[i];
        int d = a % cum_levels[i];
        config_value[i] = c;
        a = d;
    }
    return config_value;
}


//int PotentialTable::GetTableIndexByConfigValue() {}


//int PotentialTable::GetVariableIndex() {}

