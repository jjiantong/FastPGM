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
#include "PotentialTableBase.h"

using namespace std;

//typedef set< pair<int, int> > DiscreteConfig; // set of [variable id, variable value]

/**
 * @brief: this class contains the weights/potentials of each discrete config;
 * the discrete config does not have parent-child relationships.
 */
class PotentialTable: public PotentialTableBase {//this is used only for discrete nodes;
public:

    PotentialTable();
    PotentialTable(DiscreteNode *disc_node, Network *net);
    PotentialTable(DiscreteNode *disc_node, int observed_value);
    void ConstructEmptyPotentialTable(const set<int> &set_node_index, Network *net);

    /**
     * potential table optimization: table reorganization
     */
    void TableReorganizationPre(const vector<int> &common_variables, PotentialTable &new_table, vector<int> &locations);
    void TableReorganizationMain(int k, int *config1, int *config2, PotentialTable &old_table, const vector<int> &locations);

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
    void TableMarginalization(int ext_variable);
    void TableMarginalizationPre(int ext_variable, PotentialTable &new_table);

    /**
     * potential table operation 3: table extension
     */
    void TableExtension(const vector<int> &variables, const vector<int> &dims);
    void TableExtensionPre(const vector<int> &variables, const vector<int> &dims);
    int TableExtensionMain(int k, int *full_config, int *partial_config,
                                           int nv, const vector<int> &cl, int *loc);
    void TableExtensionPost(const PotentialTable &pt, int *table_index);

    /**
     * potential table operation 4: table multiplication
     */
    void TableMultiplication(const PotentialTable &second_table);
    void TableMultiplicationTwoExtension(PotentialTable &second_table);

    /**
     * potential table operation 5: table division
     */
    void TableDivision(const PotentialTable &second_table);

    int GetVariableIndex(const int &variable);

protected:
    void GetConfigValueByTableIndex(const int &table_index, int *config_value, int num_variables, const vector<int> &cum_levels);
    int GetTableIndexByConfigValue(int *config_value, int num_variables, const vector<int> &cum_levels);

    void ConstructVarDimsAndCumLevels(Network *net);
};

#endif //BAYESIANNETWORK_POTENTIALTABLE_H
