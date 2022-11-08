//
// Created by jjt on 7/11/22.
//

#ifndef BAYESIANNETWORK_POTENTIALTABLEBASE_H
#define BAYESIANNETWORK_POTENTIALTABLEBASE_H

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

class PotentialTableBase {//this is used only for discrete nodes;
public:
    vector<int> vec_related_variables; // the variables involved in this factor/potential table
    vector<double> potentials; // the potential table

    vector<int> var_dims; // the dimension of each related variable
    vector<int> cum_levels; // the helper array used to transfer between table index and the config (in array format)
    int num_variables; // i.e., clique size
    int table_size; // number of entries

    PotentialTableBase();
    PotentialTableBase(DiscreteNode *disc_node, Network *net);
    PotentialTableBase(DiscreteNode *disc_node, int observed_value);
//    void ConstructEmptyPotentialTable(const set<int> &set_node_index, Network *net);
//
//    /**
//     * potential table operation 1: table reduction
//     */
//    void TableReduction(int e_index, int e_value_index, int num_threads);
//    int TableReductionPre(int e_index);
//    int TableReductionMain(int i, int *full_config, int loc);
//    void TableReductionPost(int index, int value_index, int *v_index, int loc);
    void GetReducedPotentials(vector<double> &result, const vector<int> &evidence, int node_index, int num_threads);
    double GetReducedPotential(const vector<int> &evidence, int num_threads);
    int GetReducedTableIndex(const vector<int> &evidence, int num_threads);
//
//    /**
//     * potential table operation 2: table marginalization
//     */
    void TableMarginalizationSimplified();
//    void TableMarginalizationPre(PotentialTable &new_table);
//    int TableMarginalizationMain(int k, int *full_config, int *partial_config,
//                                 int nv, const vector<int> &cl, int *loc);
    void TableMarginalizationOneVariablePost(const PotentialTableBase &pt, int *table_index);
    void TableMarginalizationOneVariable(int ext_variable);
    void TableMarginalizationOneVariablePre(int ext_variable, PotentialTableBase &new_table);
    void GetMarginalizedProbabilities(vector<double> &result, int node_index, int num_threads);
//
//    /**
//     * potential table operation 3: table extension
//     */
//    void TableExtension(const set<int> &variables, const vector<int> &dims);
//    void TableExtensionPre(const set<int> &variables, const vector<int> &dims);
//    int TableExtensionMain(int k, int *full_config, int *partial_config,
//                           int nv, const vector<int> &cl, int *loc);
//    void TableExtensionPost(const PotentialTable &pt, int *table_index);
//
//    /**
//     * potential table operation 4: table multiplication
//     */
//    void TableMultiplication(const PotentialTable &second_table);
//    bool TableMultiplicationPre(const PotentialTable &second_table);
    void TableMultiplicationOneVariable(const PotentialTableBase &second_table);
//    void TableMultiplicationTwoExtension(PotentialTable &second_table);
//    void TableMultiplicationPre(const PotentialTable &second_table, set<int> &all_related_variables, set<int> &diff1, set<int> &diff2);
//
//    /**
//     * potential table operation 5: table division
//     */
//    void TableDivision(const PotentialTable &second_table);
//
//    /**
//     * potential table operation 6: table addition
//     */
    void TableAddition(const PotentialTableBase &second_table);
    void TableSubtraction(const PotentialTableBase &second_table);

    void Normalize();

    void UniformDistribution();

    int GetVariableIndex(const int &variable);

protected:
//    void GetConfigValueByTableIndex(const int &table_index, int *config_value, int num_variables, const vector<int> &cum_levels);
//    int GetTableIndexByConfigValue(int *config_value, int num_variables, const vector<int> &cum_levels);
    void GetConfigValueByTableIndex(const int &table_index, int *config_value);
    int GetTableIndexByConfigValue(int *config_value);
    int GetRelativeIndexByConfigValue(int *config_value);

    void ConstructVarDimsAndCumLevels(Network *net);
    void ConstructCumLevels();
};

#endif //BAYESIANNETWORK_POTENTIALTABLEBASE_H
