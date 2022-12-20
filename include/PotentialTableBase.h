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
    /**
     * it is used in sampling based approximate inference algorithms
     * where potentials is CPT and potentialICPT is ICPT
     */
    vector<double> potentialsICPT;

    vector<int> var_dims; // the dimension of each related variable
    vector<int> cum_levels; // the helper array used to transfer between table index and the config (in array format)
    int num_variables; // i.e., clique size
    int table_size; // number of entries

    PotentialTableBase();
    PotentialTableBase(DiscreteNode *disc_node, Network *net);
    PotentialTableBase(DiscreteNode *disc_node, int observed_value);

    /**
     * potential table operation: table reduction
     */
    void GetReducedPotentials(vector<double> &result, const vector<int> &evidence, int num_threads);
    void GetReducedICPTPotentials(vector<double> &result, const vector<int> &evidence, int num_threads);
    double GetReducedPotential(const vector<int> &evidence, int num_threads);
    double GetReducedIndexAndPotential(const vector<int> &evidence, int &index, int num_threads);

    /**
     * potential table operation: table marginalization
     */
    void TableMarginalizationSimplified();
    void TableMarginalizationOneVariablePost(const PotentialTableBase &pt, int *table_index);
    void TableMarginalizationOneVariable(int ext_variable);
    void TableMarginalizationOneVariablePre(int ext_variable, PotentialTableBase &new_table);
    void GetMarginalizedProbabilities(vector<double> &result, int num_threads);

    /**
     * potential table operation: table multiplication
     */
    void TableMultiplicationOneVariable(const PotentialTableBase &second_table);

    /**
     * potential table operation: table addition
     */
    void TableAddition(const PotentialTableBase &second_table);
    void TableAdditionAndNormalization(const PotentialTableBase &second_table);
    void TableSubtraction(const PotentialTableBase &second_table);

    void Normalize();
    void NormalizeCPT();
    void NormalizeICPT();

    void UniformDistribution();

    int GetVariableIndex(const int &variable);

protected:
    void GetConfigValueByTableIndex(const int &table_index, int *config_value);
    int GetTableIndexByConfigValue(int *config_value);
    int GetRelativeIndexByConfigValue(int *config_value);

    void ConstructVarDimsAndCumLevels(Network *net);
    void ConstructCumLevels();
};

#endif //BAYESIANNETWORK_POTENTIALTABLEBASE_H
