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

class Network;
// TODO: change to PotentialTable after integrating approximate inference algorithms, and also simplify this class
class PotentialTableBase {//this is used only for discrete nodes;
public:
    vector<int> vec_related_variables; // the variables involved in this factor/potential table
    vector<double> potentials; // the potential table
    /**
     * it is used in sampling based approximate inference algorithms
     * where potentials is CPT and potentialICPT is ICPT
     */
    vector<double> potentialsICPT;
    /**
     * it is used in sampling based approximate inference algorithms that need to update importance function
     */
    vector<double> pt_scores;

    vector<int> var_dims; // the dimension of each related variable
    vector<int> cum_levels; // the helper array used to transfer between table index and the config (in array format)
    int num_variables; // i.e., clique size
    int table_size; // number of entries

    PotentialTableBase();
    PotentialTableBase(DiscreteNode *disc_node, Network *net);
    PotentialTableBase(DiscreteNode *disc_node, int observed_value);
    void ConstructPotentialTableWithEmptyPotentials(DiscreteNode *disc_node, Network *net);
    void ConstructEmptyPotentialTable(const set<int> &set_node_index, Network *net);

    /**
     * potential table optimization: table reorganization
     */
    void TableReorganization(const PotentialTableBase &refer_table);
    void TableReorganizationPre(const vector<int> &common_variables, PotentialTableBase &new_table, int *locations);
    int TableReorganizationMain(int k, int *config1, int *config2, PotentialTableBase &old_table, int *locations);
    int TableReorganizationMain(int k, int *config1, int *config2, const vector<int> &cl, int *locations);
    void TableReorganizationPost(const PotentialTableBase &pt, int *table_index);

    /**
     * potential table operation 1: table reduction
     */
    void TableReduction(int e_index, int e_value_index, int num_threads);
    int TableReductionPre(int e_index);
    int TableReductionMain(int i, int *full_config, int loc);
    void TableReductionPost(int index, int value_index, int *v_index, int loc);
    void GetReducedPotentials(vector<double> &result, const vector<int> &evidence, int num_threads);
    void GetReducedICPTPotentials(vector<double> &result, const vector<int> &evidence, int num_threads);
    double GetReducedPotential(const vector<int> &evidence, int num_threads);
    double GetReducedIndexAndPotential(const vector<int> &evidence, int &index, int num_threads);

    /**
     * potential table operation 2: table marginalization
     */
    void TableMarginalization(const vector<int> &variables, const vector<int> &dims);
    void TableMarginalizationPre(const vector<int> &variables, const vector<int> &dims);
    int TableMarginalizationMain(int k, int *full_config, int *partial_config,
                                 int nv, const vector<int> &cl, int *loc);
    void TableMarginalizationPost(const PotentialTableBase &pt, int *table_index);
    void TableMarginalization(int ext_variable);
    void TableMarginalizationPre(int ext_variable, PotentialTableBase &new_table);
    void TableMarginalizationSimplified();
    void TableMarginalizationOneVariablePost(const PotentialTableBase &pt, int *table_index);
    void TableMarginalizationOneVariable(int ext_variable);
    void TableMarginalizationOneVariablePre(int ext_variable, PotentialTableBase &new_table);
    void GetMarginalizedProbabilities(vector<double> &result, int num_threads);

    /**
     * potential table operation 3: table extension
     */
    void TableExtension(const vector<int> &variables, const vector<int> &dims);
    void TableExtensionPre(const vector<int> &variables, const vector<int> &dims);
    int TableExtensionMain(int k, int *full_config, int *partial_config,
                           int nv, const vector<int> &cl, int *loc);
    void TableExtensionPost(const PotentialTableBase &pt, int *table_index);

    /**
     * potential table operation 4: table multiplication
     */
    void TableMultiplication(const PotentialTableBase &second_table);
    void TableMultiplicationTwoExtension(PotentialTableBase &second_table);
    void TableMultiplicationOneVariable(const PotentialTableBase &second_table);

    /**
    * potential table operation 5: table division
    */
    void TableDivision(const PotentialTableBase &second_table);

//    /**
//     * potential table operation: table addition
//     */
//    void TableAddition();
//    void TableAdditionAndNormalization();
//    void TableSubtraction();

    void Normalize();
    void NormalizeCPT();
    void NormalizeICPT();
    void NormalizePtScore();

    void UniformDistribution();

    int GetVariableIndex(const int &variable);

//protected:
    void GetConfigValueByTableIndex(const int &table_index, int *config_value, int num_variables, const vector<int> &cum_levels);
    int GetTableIndexByConfigValue(int *config_value, int num_variables, const vector<int> &cum_levels);
    void GetConfigValueByTableIndex(const int &table_index, int *config_value);
    int GetTableIndexByConfigValue(int *config_value);
    int GetRelativeIndexByConfigValue(int *config_value);

    void ConstructVarDimsAndCumLevels(Network *net);
    void ConstructCumLevels();
};

#endif //BAYESIANNETWORK_POTENTIALTABLEBASE_H
