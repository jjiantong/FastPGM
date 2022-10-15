//
// Created by jjt on 2022/2/22.
//

#include "PotentialTable.h"

PotentialTable::PotentialTable() {
    num_variables = 0;
    table_size = 0;
}

/**
 * @brief: construct a potential table given a node;
 * the table consists of the node and all the existing related_variables, i.e., all its parents.
 */
PotentialTable::PotentialTable(DiscreteNode *disc_node, Network *net) {
    int node_index = disc_node->GetNodeIndex();
    int node_dim = disc_node->GetDomainSize();

    related_variables.insert(node_index); // related_variables is empty initially, because this is a constructor

    if (!disc_node->HasParents()) {
        // if this disc_node has no parents. then the potential table only contains 1 variable
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
            potentials.push_back(disc_node->GetProbability(i, empty_par_config));
        }
    } else { // if this disc_node has parents
        related_variables.insert(disc_node->set_parent_indexes.begin(), disc_node->set_parent_indexes.end());
        num_variables = related_variables.size();

        ConstructVarDimsAndCumLevels(net);

        potentials.reserve(table_size);
        for (int i = 0; i < table_size; ++i) {
            // find the discrete config by the table index, the config contains this node and its parents
            DiscreteConfig parent_config;
//            GetConfigByTableIndex(i, net, parent_config);

            int *config_value = new int[num_variables];
            GetConfigValueByTableIndex(i, config_value);
            int j = 0;
            for (auto &v: related_variables) {
                parent_config.insert(pair<int, int>(v, config_value[j++]));
            }
            SAFE_DELETE_ARRAY(config_value);

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

/**
 * @brief: construct a potential table given an evidence node;
 * the table consists of only the node
 * the location of the observed value is set to 1 and other locations are set to 0
 */
PotentialTable::PotentialTable(DiscreteNode *disc_node, int observed_value) {
    int node_index = disc_node->GetNodeIndex();
    int node_dim = disc_node->GetDomainSize();

    related_variables.insert(node_index); // related_variables is empty initially, because this is a constructor
    num_variables = 1;
    var_dims.reserve(num_variables);
    var_dims.push_back(node_dim);
    cum_levels.reserve(num_variables);
    cum_levels.push_back(1);
    table_size = node_dim;

    potentials.resize(table_size);
    for (int i = 0; i < table_size; ++i) {
        if (i == observed_value) {
            potentials[i] = 1;
        } else {
            potentials[i] = 0;
        }
    }
}

/**
 * @brief: it is actually a constructor to construct a potential table according a set of nodes
 * all entries in the potential table are initialized to 1
 * it is used in class Clique (when constructing a clique)
 */
void PotentialTable::ConstructEmptyPotentialTable(const set<int> &set_node_index, Network *net){
    related_variables = set_node_index;
    num_variables = set_node_index.size();

    ConstructVarDimsAndCumLevels(net);

    potentials.reserve(table_size);
    for (int i = 0; i < table_size; ++i) {
        potentials.push_back(1);
    }
}

/**
 * @brief: construct: 1. var_dims; 2. cum_levels; 3. table size
 * it is used in the constructor of PotentialTable
 */
void PotentialTable::ConstructVarDimsAndCumLevels(Network *net){
    var_dims.reserve(num_variables);
    for (auto &node_idx: related_variables) { // for each node
        var_dims.push_back(dynamic_cast<DiscreteNode*>(net->FindNodePtrByIndex(node_idx))->GetDomainSize());
    }

    ConstructCumLevels();
    // compute the table size -- number of possible configurations
    table_size = cum_levels[0] * var_dims[0];
}

/**
 * construct "cum_levels" according to "var_dims"
 */
void PotentialTable::ConstructCumLevels() {
    cum_levels.resize(num_variables);
    // set the right-most one ...
    cum_levels[num_variables - 1] = 1;
    // ... then compute the left ones
    for (int i = num_variables - 2; i >= 0; --i) {
        cum_levels[i] = cum_levels[i + 1] * var_dims[i + 1];
    }
}

/*!
 * @brief: get each value (index) of the configuration corresponding to the given table index
 * @method: a/b = c...d, from left to right
 *          a -- the table index at the beginning, and then d
 *          b -- cum_levels[i]
 *          c -- save in config_value
 */
void PotentialTable::GetConfigValueByTableIndex(const int &table_index, int *config_value) {
    int a = table_index;
    for (int i = 0; i < num_variables; ++i) {
        int c = a / cum_levels[i];
        int d = a % cum_levels[i];
        config_value[i] = c;
        a = d;
    }
}

void PotentialTable::GetConfigValueByTableIndex(const int &table_index, int *config_value, int num_variables, const vector<int> &cum_levels) {
    int a = table_index;
    for (int i = 0; i < num_variables; ++i) {
        int c = a / cum_levels[i];
        int d = a % cum_levels[i];
        config_value[i] = c;
        a = d;
    }
}

/*!
 * @brief: get table index given each value (index) of the configuration
 */
int PotentialTable::GetTableIndexByConfigValue(int *config_value) {
    int table_index = 0;
    for (int i = 0; i < num_variables; ++i) {
        table_index += config_value[i] * cum_levels[i];
    }
    return table_index;
}

int PotentialTable::GetTableIndexByConfigValue(int *config_value, int num_variables, const vector<int> &cum_levels) {
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
int PotentialTable::GetVariableIndex(const int &variable) {
    int index = 0;
    for (auto &v: related_variables) {
        if (v == variable) {
            return index;
        }
        index++;
    }
    return index;
}

/**
 * @brief: table operation 1: table reduction - reduce factors given evidence
 * @example:    a0 b0 c0    0.3             b0 c0    0.3
 *              a0 b0 c1    0.7             b0 c1    0.7
 *              a0 b1 c0    0.4     -->     b1 c0    0.4
 *              a0 b1 c1    0.6             b1 c1    0.6
 *              a1 b0 c0    0.1
 *              a1 b0 c1    0.9         (if we get the evidence that a = 0,
 *              a1 b1 c0    0.2         the line that conflict with this evidence will be removed,
 *              a1 b1 c1    0.8         and the variable a is also removed from the table)
 * in the example, the scope of the reduced factor becomes to be {b, c}
 * @param e_index: the variable index of the evidence
 * @param e_value_index: the value (index) of the evidence
 */
void PotentialTable::TableReduction(int e_index, int e_value_index, int num_threads) {
    // in table reduction, we first update potentials, then consider the other things
    /**
     * want to reduce the computations, but don't know why it leads to the wrong results
     */
//    if (this->num_variables == 1) {
//        // then new table's num_variable = 0
//        this->related_variables.erase(e_index);
//        this->num_variables = 0;
//        this->var_dims = vector<int>();
//        this->cum_levels = vector<int>();
//        this->table_size = 1;
//        vector<double> new_potentials(1);
//        this->potentials = new_potentials;
//        return;
//    }

    // find the location of the evidence in the old table
    int e_loc = this->TableReductionPre(e_index);

    int *full_config = new int[this->table_size * this->num_variables];
    int *value_index = new int[this->table_size];

//    omp_set_num_threads(num_threads);
//#pragma omp parallel for //schedule(dynamic, 1)
    for (int i = 0; i < this->table_size; ++i) {
        value_index[i] = this->TableReductionMain(i, full_config, e_loc);
    }
    SAFE_DELETE_ARRAY(full_config);

    this->TableReductionPost(e_index, e_value_index, value_index, e_loc);
    SAFE_DELETE_ARRAY(value_index);
}

int PotentialTable::TableReductionPre(int e_index) {
    return this->GetVariableIndex(e_index);
}

int PotentialTable::TableReductionMain(int i, int *full_config, int loc) {
    // 1. get the full config value of old table
    this->GetConfigValueByTableIndex(i, full_config + i * this->num_variables);
    // 2. get the value of the evidence variable from the new table
    return full_config[i * this->num_variables + loc];
}

void PotentialTable::TableReductionPost(int index, int value_index, int *v_index, int loc) {
    int new_size = this->table_size / this->var_dims[loc];
    vector<double> new_potentials(new_size);

    for (int i = 0, j = 0; i < this->table_size; ++i) {
        // 3. whether it is consistent with the evidence
        if (v_index[i] == value_index) {
            new_potentials[j++] = this->potentials[i];
        }
    }
    this->potentials = new_potentials;
    this->related_variables.erase(index);
    this->num_variables -= 1;

    if (this->num_variables > 0) {
        vector<int> dims;
        dims.reserve(this->num_variables);
        for (int i = 0; i < this->num_variables + 1; ++i) {
            if (i != loc) {
                dims.push_back(this->var_dims[i]);
            }
        }
        this->var_dims = dims;

        this->ConstructCumLevels();
        // table size -- number of possible configurations
        this->table_size = new_size;
    } else {
        this->var_dims = vector<int>();
        this->cum_levels = vector<int>();
        this->table_size = 1;
    }
}

/**
 * @brief: get the reduced potential tables simplified version (must satisfy some specific conditions!!!)
 * @param result the resulting table values (reduced pt.potentials)
 * @param evidence a vector of the values of all the nodes, the index is consistent with the node id in the network
 * @param node_index the node that the reduced table corresponding to
 * conditions here:
 *      1. the related variables of the original potential table should be the node "node_index" and its parents (i.e., it should be like the node's CPT)
 *      2. "evidence" should include the observations of all the parents of the node
 *      3. we reduce the original potential table according to the observations and thus the resulting table only include the node
 * improved method:
 *      the resulting table only contains the node, so the table size = dimension of the node (this->var_dims[node_loc] ==> d)
 *      we first get each of the d configurations (store in "config"), then find the "table index" of the "config" and store in "result"
 *      in this way, we only need to do the config & index mapping for d times
 *      but in the original version of table reduction, we need to do such mapping for "this->table_size" times
 */
void PotentialTable::GetReducedPotentials(vector<double> &result, const vector<int> &evidence, int node_index, int num_threads) {

    int *config = new int[this->num_variables];
    /**
     * store the evidence into config
     */
    int k = 0;
    for (auto &rv: this->related_variables) {
        if (rv == node_index) {
            // if it is the node, do nothing
        } else {
            // if it is one of the node's parent, store its value in config
            config[k] = evidence[rv];
        }
        k++;
    }

    /**
     * store each possible value of the node into config to construct each possible config
     */
    int node_loc = this->GetVariableIndex(node_index);
    for (int i = 0; i < this->var_dims[node_loc]; ++i) { // for each possible value of the node
        config[node_loc] = i; // store this value of the node into config
        int table_index = GetTableIndexByConfigValue(config); // find the table index of this config
        result[i] = this->potentials[table_index]; // get one value of the resulting vector
    }

    SAFE_DELETE_ARRAY(config);
}

/**
 * just like the above method, the difference is that now we also know the value of this node, and thus this method returns a value
 */
double PotentialTable::GetReducedPotential(int &table_index, const vector<int> &evidence, int node_index, int node_value, int num_threads) {

    int *config = new int[this->num_variables];
    /**
     * construct the config with evidence and the value of this node
     */
    int k = 0;
    for (auto &rv: this->related_variables) {
        if (rv == node_index) {
            // if it is the node, store its value "node_value"
            config[k++] = node_value;
        } else {
            // if it is one of the node's parent, store its value in config
            config[k++] = evidence[rv];
        }
    }

    table_index = GetTableIndexByConfigValue(config); // find the table index of this config
    SAFE_DELETE_ARRAY(config);

    return this->potentials[table_index];
}

/**
 * @brief: table operation 2: table marginalization - factor out a set of nodes by id
 * eliminate variable "id" by summation of the factor over "id"
 */
void PotentialTable::TableMarginalization(const set<int> &ext_variables) {
    PotentialTable new_table;
    this->TableMarginalizationPre(ext_variables, new_table);

    // generate an array showing the locations of the variables of the new table in the old table
    int *loc_in_old = new int[new_table.num_variables];
    int i = 0;
    for (auto &v: new_table.related_variables) {
        loc_in_old[i++] = this->GetVariableIndex(v);
    }

    int *full_config = new int[this->table_size * this->num_variables];
    int *partial_config = new int[this->table_size * new_table.num_variables];
    int *table_index = new int[this->table_size];

//#pragma omp taskloop
//    omp_set_num_threads(num_threads);
//#pragma omp parallel for
    for (int i = 0; i < this->table_size; ++i) {
        // 1. get the full config value of old table
        this->GetConfigValueByTableIndex(i, full_config + i * this->num_variables);
        // 2. get the partial config value from the old table
        for (int j = 0; j < new_table.num_variables; ++j) {
            partial_config[i * new_table.num_variables + j] = full_config[i * this->num_variables + loc_in_old[j]];
        }
        // 3. obtain the potential index
        table_index[i] = new_table.GetTableIndexByConfigValue(partial_config + i * new_table.num_variables);
    }
    SAFE_DELETE_ARRAY(full_config);
    SAFE_DELETE_ARRAY(partial_config);
    SAFE_DELETE_ARRAY(loc_in_old);

    new_table.TableMarginalizationPost(*this, table_index);
    (*this) = new_table;
}

void PotentialTable::TableMarginalizationPre(const set<int> &ext_variables, PotentialTable &new_table) {
    // update the new table's related variables and num variables
    new_table.related_variables = this->related_variables;
    for (auto &ext_var: ext_variables) {
        new_table.related_variables.erase(ext_var);
    }
    new_table.num_variables = this->num_variables - ext_variables.size();

    // update the new table's var dims, cum levels and table size
    if (new_table.num_variables == 0) {
        new_table.var_dims = vector<int>();
        new_table.cum_levels = vector<int>();
        new_table.table_size = 1;
    } else {
        new_table.var_dims.reserve(new_table.num_variables);
        int k = 0;
        for (auto &v: this->related_variables) {
            if (ext_variables.find(v) == ext_variables.end()) { // v is not in ext_variables
                new_table.var_dims.push_back(this->var_dims[k]);
            }
            k++;
        }
        new_table.ConstructCumLevels();
        new_table.table_size = new_table.cum_levels[0] * new_table.var_dims[0];
    }

    new_table.potentials.resize(new_table.table_size);
}

// note that this method is used in junction tree class.
// it is not used in the tablemarginalization method of this class.
// because the two use different getconfigvaluebytableindex method.
int PotentialTable::TableMarginalizationMain(int k, int *full_config, int *partial_config,
                                             int nv, const vector<int> &cl, int *loc) {
    // 1. get the full config value of old table
    this->GetConfigValueByTableIndex(k, full_config + k * nv, nv, cl);
    // 2. get the partial config value from the old table
    for (int l = 0; l < this->num_variables; ++l) {
        partial_config[k * this->num_variables + l] = full_config[k * nv + loc[l]];
    }
    // 3. obtain the potential index
    return this->GetTableIndexByConfigValue(partial_config + k * this->num_variables);
}

void PotentialTable::TableMarginalizationPost(const PotentialTable &pt, int *table_index) {
    for (int k = 0; k < pt.table_size; ++k) {
        // 4. potential[table_index]
        this->potentials[table_index[k]] += pt.potentials[k];
    }
}

void PotentialTable::TableMarginalization(int ext_variable) {
    PotentialTable new_table;
    this->TableMarginalizationPre(ext_variable, new_table);

    // generate an array showing the locations of the variables of the new table in the old table
    int *loc_in_old = new int[new_table.num_variables];
    int i = 0;
    for (auto &v: new_table.related_variables) {
        loc_in_old[i++] = this->GetVariableIndex(v);
    }

    int *full_config = new int[this->table_size * this->num_variables];
    int *partial_config = new int[this->table_size * new_table.num_variables];
    int *table_index = new int[this->table_size];

//#pragma omp taskloop
//    omp_set_num_threads(num_threads);
//#pragma omp parallel for
    for (int i = 0; i < this->table_size; ++i) {
        // 1. get the full config value of old table
        this->GetConfigValueByTableIndex(i, full_config + i * this->num_variables);
        // 2. get the partial config value from the old table
        for (int j = 0; j < new_table.num_variables; ++j) {
            partial_config[i * new_table.num_variables + j] = full_config[i * this->num_variables + loc_in_old[j]];
        }
        // 3. obtain the potential index
        table_index[i] = new_table.GetTableIndexByConfigValue(partial_config + i * new_table.num_variables);
    }
    SAFE_DELETE_ARRAY(full_config);
    SAFE_DELETE_ARRAY(partial_config);
    SAFE_DELETE_ARRAY(loc_in_old);

    new_table.TableMarginalizationPost(*this, table_index);
    (*this) = new_table;
}

void PotentialTable::TableMarginalizationPre(int ext_variable, PotentialTable &new_table) {
    // update the new table's related variables and num variables
    new_table.related_variables = this->related_variables;
    new_table.related_variables.erase(ext_variable);
    new_table.num_variables = this->num_variables - 1;

    // update the new table's var dims, cum levels and table size
    if (new_table.num_variables == 0) {
        new_table.var_dims = vector<int>();
        new_table.cum_levels = vector<int>();
        new_table.table_size = 1;
    } else {
        new_table.var_dims.reserve(new_table.num_variables);
        int k = 0;
        for (auto &v: this->related_variables) {
            if (v != ext_variable) { // v is not the ext_variable
                new_table.var_dims.push_back(this->var_dims[k]);
            }
            k++;
        }
        new_table.ConstructCumLevels();
        new_table.table_size = new_table.cum_levels[0] * new_table.var_dims[0];
    }

    new_table.potentials.resize(new_table.table_size);
}

/**
 * @brief table operation 3: table extension - a pre computation of multiplication
 * before doing multiplication, we can first let the two tables have the same entries
 * so we need to extend the tables to let them have the same related variables
 */
void PotentialTable::TableExtension(const set<int> &variables, const vector<int> &dims) {
    PotentialTable new_table;

    new_table.TableExtensionPre(variables, dims);

    // generate an array showing the locations of the variables of the old table in the new table
    int *loc_in_new = new int[this->num_variables];
    int i = 0;
    for (auto &v: this->related_variables) {
        loc_in_new[i++] = new_table.GetVariableIndex(v);
    }

    int *full_config = new int[new_table.table_size * new_table.num_variables];
    int *partial_config = new int[new_table.table_size * this->num_variables];
    int *table_index = new int[new_table.table_size];

//#pragma omp taskloop
//    omp_set_num_threads(num_threads);
//#pragma omp parallel for
    for (int i = 0; i < new_table.table_size; ++i) {
        // obtain the config value according to loc_in_new
        // 1. get the full config value of new table
        new_table.GetConfigValueByTableIndex(i, full_config + i * new_table.num_variables);
        // 2. get the partial config value from the new table
        for (int j = 0; j < this->num_variables; ++j) {
            partial_config[i * this->num_variables + j] = full_config[i * new_table.num_variables + loc_in_new[j]];
        }
        // 3. obtain the potential index
        table_index[i] = this->GetTableIndexByConfigValue(partial_config + i * this->num_variables);
    }
    SAFE_DELETE_ARRAY(full_config);
    SAFE_DELETE_ARRAY(partial_config);
    SAFE_DELETE_ARRAY(loc_in_new);

    new_table.TableExtensionPost(*this, table_index);
    SAFE_DELETE_ARRAY(table_index);

    (*this) = new_table;
}

void PotentialTable::TableExtensionPre(const set<int> &variables, const vector<int> &dims) {
    related_variables = variables;
    num_variables = variables.size();

    var_dims = dims;
    ConstructCumLevels();
    table_size = cum_levels[0] * var_dims[0];
    potentials.resize(table_size);
}

int PotentialTable::TableExtensionMain(int k, int *full_config, int *partial_config,
                                        int nv, const vector<int> &cl, int *loc) {
    // 1. get the full config value of new table
    this->GetConfigValueByTableIndex(k, full_config + k * this->num_variables);
    // 2. get the partial config value from the new table
    for (int l = 0; l < nv; ++l) {
        partial_config[k * nv + l] = full_config[k * this->num_variables + loc[l]];
    }
    // 3. obtain the potential index
    return this->GetTableIndexByConfigValue(partial_config + k * nv, nv, cl);
}

void PotentialTable::TableExtensionPost(const PotentialTable &pt, int *table_index) {
    for (int k = 0; k < this->table_size; ++k) {
        // 4. potential[table_index]
        this->potentials[k] = pt.potentials[table_index[k]];
    }
}


/**
 * @brief: table operation 4: table multiplication
 * cartesian product on two factors (product of factors)
 * if two factors have shared variables, the conflict ones (i.e. one variable has more than one value) in the results need to be removed.
 * if "related_variables" of one of the factors is empty, then directly return the other factor without multiplication
 * because it means that this factor is a constant; since we re-normalize at the end, the constant will not affect the result
 * @important: double-check before using this function: "this" should be always bigger than "second_table"
 * which means, there is no variable in "second_table" but not in "this"
 * if this condition cannot be satisfied, use TableMultiplicationTwoExtention, which checks whether the two tables need to be extended
 *
 * @input: this table and "second_table"
 * @output: this table
 */
void PotentialTable::TableMultiplication(const PotentialTable &second_table) {
    if (this->related_variables.empty()) {
        (*this) = second_table; // directly return "second_table"
//        return second_table;
    }
    if (second_table.related_variables.empty()) {
        return; // directly return this table
//        return (*this);
    }

    bool to_be_extended = this->TableMultiplicationPre(second_table);

    if (to_be_extended) { // if table2 should be extended and table1 not
        PotentialTable tmp_pt = second_table;
        tmp_pt.TableExtension(this->related_variables, this->var_dims);

        for (int i = 0; i < this->table_size; ++i) {
            this->potentials[i] *= tmp_pt.potentials[i];
        }
    } else {
        for (int i = 0; i < this->table_size; ++i) {
            this->potentials[i] *= second_table.potentials[i];
        }
    }
}

bool PotentialTable::TableMultiplicationPre(const PotentialTable &second_table) {
    set<int> diff;

    // before multiplication, we first extend the separator table to the same size if required
    // get the variables that in the clique table but not in the separator table
    set_difference(this->related_variables.begin(), this->related_variables.end(),
                   second_table.related_variables.begin(), second_table.related_variables.end(),
                   inserter(diff, diff.begin()));
    return !diff.empty();
}

/**
 * do table multiplication for the case where second_table contains only one related variable
 * and this table also contains this variable
 * implement this version because lbp always multiplies with a table that contains only one variable
 */
void PotentialTable::TableMultiplicationOneVariable(const PotentialTable &second_table) {
    if (this->related_variables.empty()) {
        (*this) = second_table; // directly return "second_table"
//        return second_table;
    }
    if (second_table.related_variables.empty()) {
        return; // directly return this table
//        return (*this);
    }

    if (second_table.related_variables.size() > 1) {
        cout << "error in TableMultiplicationOneVariable: the second table has " << second_table.related_variables.size() << " variables" << endl;
    }

    int variable_index = *second_table.related_variables.begin();

    /**
     * it is just the "to_be_extended", to decide whether table 1 is bigger
     * if not, which means both table 1 and 2 have one variable
     * then directly do the multiplication
     */
    bool first_is_bigger = this->TableMultiplicationPre(second_table);

    if (first_is_bigger) {
        /**
         * in this case, the main structure of table 1 is not changed;
         * the change is that every cell of the potential table is multiplied with a value
         */
        // get the location of the one variable
        int loc = this->GetVariableIndex(variable_index);

        vector<double> potential_values = second_table.potentials;
        int *full_config = new int[this->num_variables];

        // change each cell of table 1
        for (int i = 0; i < this->table_size; ++i) {
            // get the full config value of table 1
            this->GetConfigValueByTableIndex(i, full_config);
            // get the value of the one variable and update
            this->potentials[i] *= potential_values[full_config[loc]];
        }
        SAFE_DELETE_ARRAY(full_config);
    } else {
        for (int i = 0; i < this->table_size; ++i) {
            this->potentials[i] *= second_table.potentials[i];
        }
    }
}

/**
 * this function is used when we don't know which table is bigger
 * note that table 2 may be changed after this process
 */
void PotentialTable::TableMultiplicationTwoExtension(PotentialTable &second_table) {
    if (this->related_variables.empty()) {
        (*this) = second_table; // directly return "second_table"
//        return second_table;
    }
    if (second_table.related_variables.empty()) {
        return; // directly return this table
//        return (*this);
    }

    set<int> all_related_variables, diff1, diff2;
    this->TableMultiplicationPre(second_table, all_related_variables, diff1, diff2);

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

//#pragma omp taskloop
    for (int i = 0; i < this->table_size; ++i) {
        this->potentials[i] *= second_table.potentials[i];
    }
}

void PotentialTable::TableMultiplicationPre(const PotentialTable &second_table, set<int> &all_related_variables,
                                            set<int> &diff1, set<int> &diff2) {
    all_related_variables.insert(this->related_variables.begin(), this->related_variables.end());
    all_related_variables.insert(second_table.related_variables.begin(), second_table.related_variables.end());

    // before multiplication, we first extend the separator table to the same size if required
    // get the variables that in the clique table but not in the separator table
    set_difference(all_related_variables.begin(), all_related_variables.end(),
                   this->related_variables.begin(), this->related_variables.end(),
                   inserter(diff1, diff1.begin()));
    set_difference(all_related_variables.begin(), all_related_variables.end(),
                   second_table.related_variables.begin(), second_table.related_variables.end(),
                   inserter(diff2, diff2.begin()));
}


/**
 * @brief table operation 5: table division
 * @param second_table
 */
void PotentialTable::TableDivision(const PotentialTable &second_table) {
    // if related variable of both are empty
    if (this->related_variables.empty()) {
        // do nothing, just return, because "table" is a constant
        return;
    }

//#pragma omp taskloop
    for (int i = 0; i < this->table_size; ++i) {
        if (second_table.potentials[i] == 0) {
            this->potentials[i] = 0;
        } else {
            this->potentials[i] /= second_table.potentials[i];
        }
    }
}



/**
 * @brief table operation 6: table addition
 * @param second_table should have the same structure as this table, the only difference should be the values in potentials
 */
void PotentialTable::TableAddition(const PotentialTable &second_table) {
    for (int i = 0; i < this->table_size; ++i) {
        this->potentials[i] += second_table.potentials[i];
    }
}



void PotentialTable::Normalize() {
    double denominator = 0;

    // compute the denominator for each of the configurations
    for (int i = 0; i < table_size; ++i) {
        denominator += potentials[i];
    }

    // normalize for each of the configurations
    for (int i = 0; i < table_size; ++i) {
        potentials[i] /= denominator;
    }
}