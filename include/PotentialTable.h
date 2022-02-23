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

#include "gadget.h"
#include "Node.h"
#include "DiscreteNode.h"
#include "Network.h"

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

    DiscreteConfig GetConfigByTableIndex(int table_index, Network *net);
    vector<int> GetConfigValueByTableIndex(int table_index);
    int GetTableIndexByConfigValue(vector<int> config_value);
    int GetVariableIndex(int variable);

    void TableExtension(set<int> variables, vector<int> dims);
    PotentialTable TableMultiplication(PotentialTable second_table);
    void TableReduction(int e_index, int e_value_index);

//    Factor MultiplyWithFactor(Factor second_factor);
//    Factor SumOverVar(DiscreteNode *);
//    Factor SumOverVar(int);
//    void FactorReduction(DiscreteConfig evidence);
//    void Normalize();
//
//    void PrintPotentials() const;
};

#endif //BAYESIANNETWORK_POTENTIALTABLE_H
