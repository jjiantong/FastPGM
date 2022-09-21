//
// Created by jjt on 2022/2/22.
//

#ifndef BAYESIANNETWORK_POTENTIALTABLE_H
#define BAYESIANNETWORK_POTENTIALTABLE_H

#include <set>
#include <vector>
#include <utility>
#include <string>
#include <algorithm>
#include "common.h"
#include "Node.h"
#include "DiscreteNode.h"
#include "Network.h"
#include "Timer.h"

using namespace std;

//typedef set< pair<int, int> > DiscreteConfig; // set of [variable id, variable value]
class Network;  // Forward declaration.

/**
 * @brief: this class contains the weights/potentials of each discrete config;
 * the discrete config does not have parent-child relationships.
 */
class PotentialTable {//this is used only for discrete nodes;
public:
    set<int> related_variables; // the variables involved in this factor/potential table
    vector<double> potentials; // the potential table

    vector<int> var_dims; // the dimension of each related variable
    vector<int> cum_levels; // the helper array used to transfer between table index and the config (in array format)
    int num_variables; // i.e., clique size
    int table_size; // number of entries

    PotentialTable() = default;
    PotentialTable(DiscreteNode *disc_node, Network *net);
    void ConstructEmptyPotentialTable(const set<int> &set_node_index, Network *net);

    /**
     * potential table operation 1: table reduction
     */
    void TableReduction(int e_index, int e_value_index, int num_threads);
    int TableReductionPre(int e_index);
    int TableReductionMain(int i, int *full_config, int loc);
    void TableReductionPost(int index, int value_index, int *v_index, int loc);

    /**
     * potential table operation 2: table marginalization
     */
    void TableMarginalization(const set<int> &ext_variables);
    void TableMarginalizationPre(const set<int> &ext_variables, PotentialTable &new_table);
    int TableMarginalizationMain(int k, int *full_config, int *partial_config,
                                 int nv, const vector<int> &cl, int *loc);
    void TableMarginalizationPost(const PotentialTable &pt, int *table_index);

    /**
     * potential table operation 3: table extension
     */
    void TableExtension(const set<int> &variables, const vector<int> &dims);
    void TableExtensionPre(const set<int> &variables, const vector<int> &dims);
    int TableExtensionMain(int k, int *full_config, int *partial_config,
                                           int nv, const vector<int> &cl, int *loc);
    void TableExtensionPost(const PotentialTable &pt, int *table_index);

    /**
     * potential table operation 4: table multiplication
     */
    bool TableMultiplicationPre(PotentialTable &second_table, set<int> &all_related_variables);
    void TableMultiplication(PotentialTable &second_table);

    /**
     * potential table operation 5: table division
     */
    void TableDivision(const PotentialTable &second_table);

    void Normalize();

protected:
    void GetConfigValueByTableIndex(const int &table_index, int *config_value, int num_variables, const vector<int> &cum_levels);
    int GetTableIndexByConfigValue(int *config_value, int num_variables, const vector<int> &cum_levels);
    void GetConfigValueByTableIndex(const int &table_index, int *config_value);
    int GetTableIndexByConfigValue(int *config_value);

    void ConstructVarDimsAndCumLevels(Network *net);
    void ConstructCumLevels();
    int GetVariableIndex(const int &variable);
};

#endif //BAYESIANNETWORK_POTENTIALTABLE_H
