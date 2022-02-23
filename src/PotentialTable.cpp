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

/*!
 * @brief: get table index given each value (index) of the configuration
 */
int PotentialTable::GetTableIndexByConfigValue(vector<int> config_value) {
    int table_index = 0;
    for (int i = 0; i < num_variables; ++i) {
        table_index += config_value[i] * cum_levels[i];
    }
    return table_index;
}

/*!
 * @brief: get the location of one variable in the related_variables
 * @param variable: one variable in the "related_variables"
 * @return the location of the variable in the "related_variables"
 */
int PotentialTable::GetVariableIndex(int variable) {
    int index = 0;
    for (auto &v: related_variables) {
        if (v == variable) {
            return index;
        }
        index++;
    }
    return index;
}


void PotentialTable::TableExtension(set<int> variables, vector<int> dims) {
    PotentialTable new_table;

    new_table.related_variables = variables;
    new_table.num_variables = variables.size();
    new_table.var_dims = dims;

    new_table.cum_levels.resize(new_table.num_variables);
    // set the right-most one ...
    new_table.cum_levels[new_table.num_variables - 1] = 1;
    // ... then compute the left ones
    for (int i = new_table.num_variables - 2; i >= 0; --i) {
        new_table.cum_levels[i] = new_table.cum_levels[i + 1] * new_table.var_dims[i + 1];
    }
    // compute the table size -- number of possible configurations
    new_table.table_size = new_table.cum_levels[0] * new_table.var_dims[0];

    // generate an array showing the locations of the variables of the old table in the new table
    int *loc_in_new = new int[this->num_variables];
    int i = 0;
    for (auto &v: this->related_variables) {
        loc_in_new[i++] = new_table.GetVariableIndex(v);
    }

    new_table.potentials.reserve(new_table.table_size);
    for (int i = 0; i < new_table.table_size; ++i) {
        // obtain the config value according to loc_in_new
        // 1. get the full config value of new table
        vector<int> full_config = new_table.GetConfigValueByTableIndex(i);
        // 2. get the partial config value from the new table
        vector<int> partial_config;
        partial_config.reserve(this->num_variables);
        for (int j = 0; j < this->num_variables; ++j) {
            partial_config.push_back(full_config[loc_in_new[j]]);
        }
        // obtain the potential index
        int table_index = this->GetTableIndexByConfigValue(partial_config);
        // potentials[i]
        new_table.potentials.push_back(this->potentials[table_index]);
    }

    this->related_variables = new_table.related_variables;
    this->potentials = new_table.potentials;
    this->var_dims = new_table.var_dims;
    this->cum_levels = new_table.cum_levels;
    this->num_variables = new_table.num_variables;
    this->table_size = new_table.table_size;
}

/**
 * @brief: cartesian product on two factors (product of factors)
 * if two factors have shared variables, the conflict ones (i.e. one variable has more than one value) in the results need to be removed.
 * if "related_variables" of one of the factors is empty, then directly return the other factor without multiplication
 * because the case means that this factor is a constant; since we re-normalize at the end, the constant will not affect the result
 */
PotentialTable PotentialTable::TableMultiplication(PotentialTable second_table) {
//    PotentialTable new_table;

    if (this->related_variables.empty()) {
        return second_table;
    }
    if (second_table.related_variables.empty()) {
        return *this;
    }

    set<int> all_related_variables;
    all_related_variables.insert(this->related_variables.begin(), this->related_variables.end());
    all_related_variables.insert(second_table.related_variables.begin(), second_table.related_variables.end());

    // before multiplication, we first extend the two tables to the same size if required
    // get the variables that in the new table but not in the old tables
    set<int> diff1, diff2;
    set_difference(all_related_variables.begin(), all_related_variables.end(),
                   this->related_variables.begin(), this->related_variables.end(),
                   inserter(diff1, diff1.begin()));
    set_difference(all_related_variables.begin(), all_related_variables.end(),
                   second_table.related_variables.begin(), second_table.related_variables.end(),
                   inserter(diff2, diff2.begin()));

    if (diff1.empty() && diff2.empty()) { // if both table1 and table2 should not be extended
        // do nothing
    } else if (!diff1.empty() && diff2.empty()) { // if table1 should be extended and table2 not
        this->TableExtension(all_related_variables, second_table.var_dims);
    } else if (diff1.empty() && !diff2.empty()) { // if table2 should be extended and table1 not
        second_table.TableExtension(all_related_variables, this->var_dims);
    } else { // if both table1 and table2 should be extended
        vector<int> dims; // to save dims of the new related variables
        dims.reserve(all_related_variables.size());
        // to find the location of each new related variable
        for (auto &v: all_related_variables) {
            int loc = this->GetVariableIndex(v);
            if (loc < this->related_variables.size()) { // find it in table1
                dims.push_back(this->var_dims[loc]);
            } else { // cannot find in table1, we need to find it in table2
                loc = second_table.GetVariableIndex(v);
                dims.push_back(second_table.var_dims[loc]);
            }
        }
        this->TableExtension(all_related_variables, dims);
        second_table.TableExtension(all_related_variables, dims);
    }

    // do the multiplication
    for (int i = 0; i < this->table_size; ++i) {
        this->potentials[i] *= second_table.potentials[i];
    }

    return (*this);
}


