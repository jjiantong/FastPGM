//
// Created by jjt on 7/11/22.
//
#include "PotentialTableBase.h"

PotentialTableBase::PotentialTableBase() {
    num_variables = 0;
    table_size = 0;
}

/**
 * @brief: construct a potential table given a node;
 * the table consists of the node and all the existing related_variables, i.e., all its parents.
 * in this class, we let the node be the first variable in the related variables and the corresponding potential table
 */
PotentialTableBase::PotentialTableBase(DiscreteNode *disc_node, Network *net) {
    int node_index = disc_node->GetNodeIndex();
    int node_dim = disc_node->GetDomainSize();

    if (!disc_node->HasParents()) {
        // if this disc_node has no parents. then the potential table only contains 1 variable
        num_variables = 1;
        vec_related_variables.resize(1);
        var_dims.resize(1);
        cum_levels.resize(1);

        vec_related_variables[0] = node_index;
        var_dims[0] = node_dim;
        cum_levels[0] = 1;
        table_size = node_dim;

        potentials.resize(table_size);
        DiscreteConfig empty_par_config;
        for (int i = 0; i < table_size; ++i) {
            // potentials[i]
            potentials[i] = disc_node->GetProbability(i, empty_par_config);
        }
    } else { // if this disc_node has parents
        num_variables = disc_node->set_parent_indexes.size() + 1;
        vec_related_variables.resize(num_variables);
        vec_related_variables[0] = node_index;
        int vi = 1;
        for (auto &p: disc_node->set_parent_indexes) {
            vec_related_variables[vi++] = p;
        }

        ConstructVarDimsAndCumLevels(net);

        potentials.resize(table_size);
        for (int i = 0; i < table_size; ++i) {
            // find the discrete config by the table index, the config contains its parents
            DiscreteConfig parent_config;

            int *config_value = new int[num_variables];
            GetConfigValueByTableIndex(i, config_value);

            int node_value = config_value[0]; // get the value of the node

            for (int j = 1; j < num_variables; ++j) { // for each variable except for the first one
                                                      // - because the first one is the node
                parent_config.insert(pair<int, int>(vec_related_variables[j], config_value[j]));
            }
            SAFE_DELETE_ARRAY(config_value);

            // potentials[i]
            potentials[i] = disc_node->GetProbability(node_value, parent_config);
        }
    }//end has parents
}

/**
 * @brief: construct a potential table given an evidence node;
 * the table consists of only the node
 * the location of the observed value is set to 1 and other locations are set to 0
 */
PotentialTableBase::PotentialTableBase(DiscreteNode *disc_node, int observed_value) {
    int node_index = disc_node->GetNodeIndex();
    int node_dim = disc_node->GetDomainSize();

    vec_related_variables.push_back(node_index); // related_variables is empty initially, because this is a constructor
    num_variables = 1;
    var_dims.resize(1);
    var_dims[0] = node_dim;
    cum_levels.resize(1);
    cum_levels[0] = 1;
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
void PotentialTableBase::ConstructEmptyPotentialTable(const set<int> &set_node_index, Network *net){
    num_variables = set_node_index.size();
    vec_related_variables.resize(num_variables);
    int i = 0;
    for (auto &rv: set_node_index) {
        vec_related_variables[i++] = rv;
    }

    ConstructVarDimsAndCumLevels(net);

    potentials.reserve(table_size);
    for (int i = 0; i < table_size; ++i) {
        potentials.push_back(1);
    }
}

/**
 * @brief: table operation: table re-organization - re-organize the configuration order of the potential table
 * the related variables remain the same, but their order is changed
 * there are two types of table re-organization:
 * 1. change the order according to another potential table
 *    this is use before multiplication in the collection procedure (practically can be used before all multiplications)
 *    TableReorganization() here implements this type
 * 2. change the order according to some optimization rules
 *    refer to ReorganizeTableStorage() in JunctionTree
 *    TableReorganizationPre() below is for this type (also showing the optimization rules)
 * The main part of them (TableReorganizationMain() below) are the same
 */
void PotentialTableBase::TableReorganization(const PotentialTableBase &refer_table) {
    // all things except for potentials are maintained
    PotentialTableBase new_table = refer_table;

    // the locations of the elements in the old table
    // e.g., locations[0] means the locations of new_table.vec_related_variables[0] in this->vec_related_variables
    // i.e., new_table.vec_related_variables[0] = this->vec_related_variables[locations[0]]
    int *locations = new int[this->num_variables];
    for (int i = 0; i < new_table.num_variables; ++i) { // for each new table's related variable
        locations[i] = this->GetVariableIndex(new_table.vec_related_variables[i]);
    }

    int *config1 = new int[this->num_variables];
    int *config2 = new int[this->num_variables];
    int *table_index = new int[this->table_size];

    // the main loop
//#pragma omp taskloop
//    omp_set_num_threads(num_threads);
//#pragma omp parallel for
    for (int k = 0; k < new_table.table_size; ++k) {
        table_index[k] = new_table.TableReorganizationMain(k, config1, config2, *this, locations);
    }
    new_table.TableReorganizationPost(*this, table_index);

    (*this) = new_table;

    SAFE_DELETE_ARRAY(locations);
    SAFE_DELETE_ARRAY(config1);
    SAFE_DELETE_ARRAY(config2);
    SAFE_DELETE_ARRAY(table_index);
}

void PotentialTableBase::TableReorganizationPre(const vector<int> &common_variables, PotentialTableBase &new_table, int *locations) {
    // maintain
    new_table.num_variables = this->num_variables;
    new_table.table_size = this->table_size;
    new_table.potentials.resize(new_table.table_size);

    new_table.vec_related_variables.resize(new_table.num_variables);
    new_table.var_dims.resize(new_table.num_variables);
    vector<int> tmp_loc(common_variables.size()); // to record the location of the common variables

    // remove common_variables from this->table.related_variables
    // and handle the new var_dims at the same time
    int i = 0, j = 0, k = 0; // i for this table, j for common, k for new table
    int m = 0; // for tmp loc
    while (i < this->num_variables && j < common_variables.size()) {
        while (this->vec_related_variables[i] != common_variables[j]) {
            // this variable is not in common variables, keep it
            locations[k] = i;
            new_table.vec_related_variables[k] = this->vec_related_variables[i];
            new_table.var_dims[k++] = this->var_dims[i++];
        } // end of while, now this table i == common j
        // this variable is in common variables, skip it
        tmp_loc[m++] = i;
        i++;
        j++;
    } // end of while, two possible cases: 1. i = this->num_variables; 2. j = common_variables.size()

    // if only 2 but not 1, post-process the left elements
    while (i < this->num_variables) {
        // this variable is not in common variables, keep it
        locations[k] = i;
        new_table.vec_related_variables[k] = this->vec_related_variables[i];
        new_table.var_dims[k++] = this->var_dims[i++];
    }

    // after those variables that are not in common_variables, add common_variables
    for (int l = 0; l < common_variables.size(); ++l) {
        new_table.vec_related_variables[k + l] = common_variables[l];
        new_table.var_dims[k + l] = this->var_dims[tmp_loc[l]];
        locations[k + l] = tmp_loc[l];
    }

    new_table.ConstructCumLevels();
}

int PotentialTableBase::TableReorganizationMain(int k, int *config1, int *config2, PotentialTableBase &old_table, int *locations) {
    // 1. get the config value of old table
    old_table.GetConfigValueByTableIndex(k, config1);
    // 2. get the config value of new table from the config value of old table
    for (int l = 0; l < this->num_variables; ++l) {
        config2[l] = config1[locations[l]];
    }
    // 3. obtain the potential index of new table
    return GetTableIndexByConfigValue(config2);
}

// note that this method is used in junction tree class.
// it is not used in the TableReorganization method of this class.
// because the two use different getconfigvaluebytableindex method.
int PotentialTableBase::TableReorganizationMain(int k, int *config1, int *config2, const vector<int> &cl, int *locations) {
    // note the difference from marginalization/extension main:
    // nv (of old table) = this->num_variables
    // 1. get the config value of old table
    GetConfigValueByTableIndex(k, config1 + k * this->num_variables, this->num_variables, cl);
    // 2. get the config value of new table from the config value of old table
    for (int l = 0; l < this->num_variables; ++l) {
        config2[k * this->num_variables + l] = config1[k * this->num_variables + locations[l]];
    }
    // 3. obtain the potential index of new table
    return GetTableIndexByConfigValue(config2 + k * this->num_variables);
}

void PotentialTableBase::TableReorganizationPost(const PotentialTableBase &pt, int *table_index) {
    for (int k = 0; k < this->table_size; ++k) {
        // 4. potential[table_index]
        this->potentials[table_index[k]] = pt.potentials[k];
    }
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
void PotentialTableBase::TableReduction(int e_index, int e_value_index, int num_threads) {
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

int PotentialTableBase::TableReductionPre(int e_index) {
    return this->GetVariableIndex(e_index);
}

int PotentialTableBase::TableReductionMain(int i, int *full_config, int loc) {
    // 1. get the full config value of old table
    GetConfigValueByTableIndex(i, full_config + i * this->num_variables);
    // 2. get the value of the evidence variable from the new table
    return full_config[i * this->num_variables + loc];
}

void PotentialTableBase::TableReductionPost(int index, int value_index, int *v_index, int loc) {
    int new_size = this->table_size / this->var_dims[loc];
    vector<double> new_potentials(new_size);

    for (int i = 0, j = 0; i < this->table_size; ++i) {
        // 3. whether it is consistent with the evidence
        if (v_index[i] == value_index) {
            new_potentials[j++] = this->potentials[i];
        }
    }
    this->potentials = new_potentials;

    this->num_variables -= 1;
    if (this->num_variables > 0) {
        // remove index from this->related_variables
        // store the dim of the left variables at the same time
        vector<int> new_related_variables(this->num_variables);
        vector<int> dims(this->num_variables);
        int i = 0;
        while (this->vec_related_variables[i] != index) {
            new_related_variables[i] = this->vec_related_variables[i];
            dims[i] = this->var_dims[i];
            i++;
        } // end while, now this->related_variables[i] = index
        i++; // skip the index
        while (i < this->num_variables + 1) {
            new_related_variables[i - 1] = this->vec_related_variables[i];
            dims[i - 1] = this->var_dims[i];
            i++;
        }
        this->vec_related_variables = new_related_variables;
        this->var_dims = dims;

        this->ConstructCumLevels();
        // table size -- number of possible configurations
        this->table_size = new_size;
    } else {
        this->vec_related_variables = vector<int>();
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
 *      we let the node in the first location of the related variables
 *      so we can first compute the dimensionality of its parents dp, and the parent configuration location according to its parent configuration lp
 *      then we can find the values of the indexes satisfying index % dp == lp
 */
void PotentialTableBase::GetReducedPotentials(vector<double> &result, const vector<int> &evidence, int num_threads) {
    if (this->num_variables == 1) { // if the node has no parent, directly return its table
        result = this->potentials;
        return;
    }

    /**
     * the dimensionality of its parents dp
     */
    int dp = this->cum_levels[0];

    /**
     * compute the parent configuration location lp
     * according to the parent configuration, we can compute the relative index, or say, partial index? parent config index?
     */
    int *par_config = new int[this->num_variables - 1];
    // store the evidence into parent configuration
    for (int i = 1; i < this->vec_related_variables.size(); ++i) {
        par_config[i - 1] = evidence[this->vec_related_variables[i]];
    }
    // find the relative location
    int lp = GetRelativeIndexByConfigValue(par_config);

    SAFE_DELETE_ARRAY(par_config);

    /**
     * the indexes should satisfy index % dp == lp
     * so the first index is lp, then lp + dp, then lp + dp + dp, ...
     */
    int index = lp;
    result[0] = this->potentials[index];
    for (int i = 1; i < this->var_dims[0]; ++i) {
        index += dp;
        result[i] = this->potentials[index];
    }

//    int *full_config = new int[this->num_variables];
//    for (int i = 1; i < this->vec_related_variables.size(); ++i) {
//        full_config[i] = evidence[this->vec_related_variables[i]];
//    }
//    for (int i = 0; i < this->var_dims[0]; ++i) {
//        full_config[0] = i;
//        int table_index = GetTableIndexByConfigValue(full_config);
//        result[i] = this->potentials[table_index];
//    }
//    SAFE_DELETE_ARRAY(full_config);
}

void PotentialTableBase::GetReducedICPTPotentials(vector<double> &result, const vector<int> &evidence, int num_threads) {
    if (this->num_variables == 1) { // if the node has no parent, directly return its table
        result = this->potentialsICPT;
        return;
    }

    /**
     * the dimensionality of its parents dp
     */
    int dp = this->cum_levels[0];

    /**
     * compute the parent configuration location lp
     * according to the parent configuration, we can compute the relative index, or say, partial index? parent config index?
     */
    int *par_config = new int[this->num_variables - 1];
    // store the evidence into parent configuration
    for (int i = 1; i < this->vec_related_variables.size(); ++i) {
        par_config[i - 1] = evidence[this->vec_related_variables[i]];
    }
    // find the relative location
    int lp = GetRelativeIndexByConfigValue(par_config);
    SAFE_DELETE_ARRAY(par_config);

    /**
     * the indexes should satisfy index % dp == lp
     * so the first index is lp, then lp + dp, then lp + dp + dp, ...
     */
    int index = lp;
    result[0] = this->potentialsICPT[index];
    for (int i = 1; i < this->var_dims[0]; ++i) {
        index += dp;
        result[i] = this->potentialsICPT[index];
    }

//    int *full_config = new int[this->num_variables];
//    for (int i = 1; i < this->vec_related_variables.size(); ++i) {
//        full_config[i] = evidence[this->vec_related_variables[i]];
//    }
//    for (int i = 0; i < this->var_dims[0]; ++i) {
//        full_config[0] = i;
//        int table_index = GetTableIndexByConfigValue(full_config);
//        result[i] = this->potentialsICPT[table_index];
//    }
//    SAFE_DELETE_ARRAY(full_config);
}

/**
 * just like the above method, the difference is that now we also know the value of this node, and thus this method returns one value
 */
double PotentialTableBase::GetReducedPotential(const vector<int> &evidence, int num_threads) {
    if (this->num_variables == 1) { // if the node has no parent, directly return its table
        return this->potentials[evidence[this->vec_related_variables[0]]];
    }

    int *config = new int[this->num_variables];
    /**
     * construct the config with evidence and the value of this node
     */
    for (int i = 0; i < this->num_variables; ++i) {
        config[i] = evidence[this->vec_related_variables[i]];
    }

    int table_index = GetTableIndexByConfigValue(config); // find the table index of this config
    SAFE_DELETE_ARRAY(config);

    return this->potentials[table_index];
}

double PotentialTableBase::GetReducedIndexAndPotential(const vector<int> &evidence, int &index, int num_threads) {
    if (this->num_variables == 1) {
        index = evidence[this->vec_related_variables[0]];
        return this->potentials[index];
    }

    int *config = new int[this->num_variables];
    /**
     * construct the config with evidence and the value of this node
     */
    for (int i = 0; i < this->num_variables; ++i) {
        config[i] = evidence[this->vec_related_variables[i]];
    }

    index = GetTableIndexByConfigValue(config); // find the table index of this config
    SAFE_DELETE_ARRAY(config);

    return this->potentials[index];
}

/**
 * @brief: table operation 2: table marginalization - factor out a set of nodes by id
 * eliminate variable "id" by summation of the factor over "id"
 */
void PotentialTableBase::TableMarginalization(const vector<int> &variables, const vector<int> &dims) {
    PotentialTableBase new_table;
    new_table.TableMarginalizationPre(variables, dims);

    // generate an array showing the locations of the variables of the new table in the old table
    int *loc_in_old = new int[new_table.num_variables];
    for (int i = 0; i < new_table.num_variables; ++i) {
        loc_in_old[i] = this->GetVariableIndex(new_table.vec_related_variables[i]);
    }

    int *full_config = new int[this->table_size * this->num_variables];
    int *partial_config = new int[this->table_size * new_table.num_variables];
    int *table_index = new int[this->table_size];

//#pragma omp taskloop
//    omp_set_num_threads(num_threads);
//#pragma omp parallel for
    for (int i = 0; i < this->table_size; ++i) {
        // 1. get the full config value of old table
        GetConfigValueByTableIndex(i, full_config + i * this->num_variables);
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

void PotentialTableBase::TableMarginalizationPre(const vector<int> &variables, const vector<int> &dims) {
    vec_related_variables = variables;
    num_variables = variables.size();

    var_dims = dims;

    if (num_variables != 0) {
        ConstructCumLevels();
        table_size = cum_levels[0] * var_dims[0];
    } else {
        cum_levels = vector<int>();
        table_size = 1;
    }

    potentials.resize(table_size);
}

// note that this method is used in junction tree class.
// it is not used in the tablemarginalization method of this class.
// because the two use different getconfigvaluebytableindex method.
int PotentialTableBase::TableMarginalizationMain(int k, int *full_config, int *partial_config,
                                             int nv, const vector<int> &cl, int *loc) {
    // 1. get the full config value of old table
    GetConfigValueByTableIndex(k, full_config + k * nv, nv, cl);
    // 2. get the partial config value from the old table
    for (int l = 0; l < this->num_variables; ++l) {
        partial_config[k * this->num_variables + l] = full_config[k * nv + loc[l]];
    }
    // 3. obtain the potential index
    return GetTableIndexByConfigValue(partial_config + k * this->num_variables);
}

void PotentialTableBase::TableMarginalizationPost(const PotentialTableBase &pt, int *table_index) {
    for (int k = 0; k < pt.table_size; ++k) {
        // 4. potential[table_index]
        this->potentials[table_index[k]] += pt.potentials[k];
    }
}

void PotentialTableBase::TableMarginalization(int ext_variable) {
    PotentialTableBase new_table;
    this->TableMarginalizationPre(ext_variable, new_table);

    // generate an array showing the locations of the variables of the new table in the old table
    int *loc_in_old = new int[new_table.num_variables];
    for (int i = 0; i < new_table.num_variables; ++i) {
        loc_in_old[i] = this->GetVariableIndex(new_table.vec_related_variables[i]);
    }

    int *full_config = new int[this->table_size * this->num_variables];
    int *partial_config = new int[this->table_size * new_table.num_variables];
    int *table_index = new int[this->table_size];

//#pragma omp taskloop
//    omp_set_num_threads(num_threads);
//#pragma omp parallel for
    for (int i = 0; i < this->table_size; ++i) {
        // 1. get the full config value of old table
        GetConfigValueByTableIndex(i, full_config + i * this->num_variables);
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

void PotentialTableBase::TableMarginalizationPre(int ext_variable, PotentialTableBase &new_table) {
    new_table.num_variables = this->num_variables - 1;

    if (new_table.num_variables == 0) {
        new_table.vec_related_variables = vector<int>();
        new_table.var_dims = vector<int>();
        new_table.cum_levels = vector<int>();
        new_table.table_size = 1;
    } else {
        // update the new table's related variables and num variables
        new_table.vec_related_variables.resize(new_table.num_variables);
        int i = 0;
        while (this->vec_related_variables[i] != ext_variable) {
            new_table.vec_related_variables[i] = this->vec_related_variables[i];
            i++;
        } // end while, now this->related_variables[i] = index
        i++; // skip the ext_variable
        while (i < this->num_variables) {
            new_table.vec_related_variables[i - 1] = this->vec_related_variables[i];
            i++;
        }

        // update the new table's var dims, cum levels and table size
        new_table.var_dims.reserve(new_table.num_variables);
        int k = 0;
        for (auto &v: this->vec_related_variables) {
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
 * do table marginalization for the case where we only need to sum out one variable
 * implement this version because lbp always sum out one variable
 */
void PotentialTableBase::TableMarginalizationOneVariable(int ext_variable) {
    PotentialTableBase new_table;
    this->TableMarginalizationOneVariablePre(ext_variable, new_table);

    // generate an array showing the locations of the variables of the new table in the old table
    int *loc_in_old = new int[new_table.num_variables];
    for (int i = 0; i < new_table.num_variables; ++i) {
        loc_in_old[i] = this->GetVariableIndex(new_table.vec_related_variables[i]);
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

    new_table.TableMarginalizationOneVariablePost(*this, table_index);
    (*this) = new_table;
}

void PotentialTableBase::TableMarginalizationOneVariablePre(int ext_variable, PotentialTableBase &new_table) {
    // update the new table's related variables and num variables
    new_table.num_variables = this->num_variables - 1;

    // update the new table's var dims, cum levels and table size
    if (new_table.num_variables == 0) {
        new_table.vec_related_variables = vector<int>();
        new_table.var_dims = vector<int>();
        new_table.cum_levels = vector<int>();
        new_table.table_size = 1;
    } else {
        new_table.vec_related_variables.resize(new_table.num_variables);
        new_table.var_dims.resize(new_table.num_variables);

        int index = 0;
        for (int i = 0; i < this->num_variables; ++i) {
            if (this->vec_related_variables[i] != ext_variable) {
                new_table.vec_related_variables[index] = this->vec_related_variables[i];
                new_table.var_dims[index++] = this->var_dims[i];
            }
        }
        new_table.ConstructCumLevels();
        new_table.table_size = new_table.cum_levels[0] * new_table.var_dims[0];
    }

    new_table.potentials.resize(new_table.table_size);
}

void PotentialTableBase::TableMarginalizationOneVariablePost(const PotentialTableBase &pt, int *table_index) {
    for (int k = 0; k < pt.table_size; ++k) {
        // 4. potential[table_index]
        this->potentials[table_index[k]] += pt.potentials[k];
    }
}

/**
 * @brief: get the marginalized potential tables simplified version (must satisfy some specific conditions!!!)
 * NOTE: it is now only used in Heuristic Uniform Distribution in Importance Sampling
 * @param result the resulting table values (marginalized pt.potentials)
 * @param node_index the node that the reduced table corresponding to
 * conditions here:
 *      1. the related variables of the original potential table should be the node "node_index" and its parents (i.e., it should be like the node's CPT)
 *      2. we sum out all the variables except for the node and thus the resulting table only include the node
 * improved method:
 *      the resulting table only contains the node, so the table size = dimension of the node (d)
 *      since the location of the node is 0, we accumulate the first d values -> result[0],
 *                                              accumulate the second d values -> result[1], ...
 */
void PotentialTableBase::GetMarginalizedProbabilities(vector<double> &result, int num_threads) {
    if (this->num_variables == 1) { // if the node has no parent, directly return its potential table
        result = this->potentialsICPT;
        return;
    }

    for (int i = 0; i < this->table_size; ++i) {
        int id = i / this->cum_levels[0];
        result[id] += this->potentialsICPT[i];
    }
}

/**
 * @brief: it is also a simplified version of table marginalization (factor out a set of nodes)
 * the conditions are the same as the above method, and the implementation is also similar to the above method
 */
void PotentialTableBase::TableMarginalizationSimplified() {
    if (this->num_variables == 1) { // if the node has no parent, do nothing
        return;
    }

    int d = this->var_dims[0]; // get the dimension of the node

    PotentialTableBase new_table;
    // update the new table's related variables and num variables
    new_table.vec_related_variables.push_back(this->vec_related_variables[0]);
    new_table.num_variables = 1;

    // update the new table's var dims, cum levels and table size
    new_table.var_dims.resize(1);
    new_table.var_dims[0] = d;
    new_table.cum_levels.resize(1);
    new_table.cum_levels[0] = 1;
    new_table.table_size = d;
    new_table.potentials.resize(d);

    int dp = this->table_size / d; // get the dimension of its parents, which is the number of values to be accumulated
    for (int i = 0; i < this->table_size; ++i) {
        int id = i / dp;
        new_table.potentials[id] += this->potentials[i];
    }

    (*this) = new_table;
}

/**
 * @brief table operation 3: table extension - a pre computation of multiplication
 * before doing multiplication, we can first let the two tables have the same entries
 * so we need to extend the tables to let them have the same related variables
 */
void PotentialTableBase::TableExtension(const vector<int> &variables, const vector<int> &dims) {
    PotentialTableBase new_table;

    new_table.TableExtensionPre(variables, dims);

    // generate an array showing the locations of the variables of the old table in the new table
    int *loc_in_new = new int[this->num_variables];
    for (int i = 0; i < this->num_variables; ++i) {
        loc_in_new[i] = new_table.GetVariableIndex(this->vec_related_variables[i]);
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
        table_index[i] = GetTableIndexByConfigValue(partial_config + i * this->num_variables);
    }
    SAFE_DELETE_ARRAY(full_config);
    SAFE_DELETE_ARRAY(partial_config);
    SAFE_DELETE_ARRAY(loc_in_new);

    new_table.TableExtensionPost(*this, table_index);
    SAFE_DELETE_ARRAY(table_index);

    (*this) = new_table;
}

void PotentialTableBase::TableExtensionPre(const vector<int> &variables, const vector<int> &dims) {
    vec_related_variables = variables;
    num_variables = variables.size();

    var_dims = dims;
    ConstructCumLevels();
    table_size = cum_levels[0] * var_dims[0];
    potentials.resize(table_size);
}

int PotentialTableBase::TableExtensionMain(int k, int *full_config, int *partial_config,
                                       int nv, const vector<int> &cl, int *loc) {
    // 1. get the full config value of new table
    GetConfigValueByTableIndex(k, full_config + k * this->num_variables);
    // 2. get the partial config value from the new table
    for (int l = 0; l < nv; ++l) {
        partial_config[k * nv + l] = full_config[k * this->num_variables + loc[l]];
    }
    // 3. obtain the potential index
    return this->GetTableIndexByConfigValue(partial_config + k * nv, nv, cl);
}

void PotentialTableBase::TableExtensionPost(const PotentialTableBase &pt, int *table_index) {
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
void PotentialTableBase::TableMultiplication(const PotentialTableBase &second_table) {
//    if (second_table.vec_related_variables.empty()) {
//        return; // directly return this table
////        return (*this);
//    }

    PotentialTableBase tmp_pt = second_table;

    if (this->num_variables - second_table.num_variables > 0) { // if table2 should be extended and table1 not
        tmp_pt.TableExtension(this->vec_related_variables, this->var_dims);
    }

    // before multiplication, need to first decide whether the orders are the same
    if (this->vec_related_variables != tmp_pt.vec_related_variables) {
        // if not have the same order, change the order to this table's order
        tmp_pt.TableReorganization(*this);
    }

    for (int i = 0; i < this->table_size; ++i) {
        this->potentials[i] *= tmp_pt.potentials[i];
    }
}

/**
 * this function is used when we don't know which table is bigger
 * note that table 2 may be changed after this process
 * note that now this method is only used in ve, if want to use for other cases, take care of the call for table extension
 */
void PotentialTableBase::TableMultiplicationTwoExtension(PotentialTableBase &second_table) {
    if (this->vec_related_variables.empty()) {
        (*this) = second_table; // directly return "second_table"
//        return second_table;
    }
    if (second_table.vec_related_variables.empty()) {
        return; // directly return this table
//        return (*this);
    }

    set<int> set_all_related_variables;
    set_all_related_variables.insert(this->vec_related_variables.begin(), this->vec_related_variables.end());
    set_all_related_variables.insert(second_table.vec_related_variables.begin(), second_table.vec_related_variables.end());
    int d1 = set_all_related_variables.size() - this->num_variables;
    int d2 = set_all_related_variables.size() - second_table.num_variables;

    if (d1 == 0 && d2 == 0) { // if both table1 and table2 should not be extended
        // do nothing
    } else if (d1 > 0 && d2 == 0) { // if table1 should be extended and table2 not
        this->TableExtension(second_table.vec_related_variables, second_table.var_dims);
    } else if (d1 == 0 && d2 > 0) { // if table2 should be extended and table1 not
        second_table.TableExtension(this->vec_related_variables, this->var_dims);
    } else { // if both table1 and table2 should be extended
        vector<int> all_related_variables(set_all_related_variables.size());
        vector<int> dims(set_all_related_variables.size());

        // store elements in set into vector
        // and store their dim at the same time
        int i = 0;
        for (auto &v: set_all_related_variables) {
            all_related_variables[i] = v;
            // to find the location of each new related variable
            int loc = this->GetVariableIndex(v);
            if (loc < this->num_variables) { // find it in table1
                dims[i] = this->var_dims[loc];
            } else { // cannot find in table1, we need to find it in table2
                loc = second_table.GetVariableIndex(v);
                dims[i] = second_table.var_dims[loc];
            }
            i++;
        }

        this->TableExtension(all_related_variables, dims);
        second_table.TableExtension(all_related_variables, dims);
    }

//#pragma omp taskloop
    for (int i = 0; i < this->table_size; ++i) {
        this->potentials[i] *= second_table.potentials[i];
    }
}

/**
 * do table multiplication for the case where second_table contains only one related variable
 * and this table also contains this variable
 * implement this version because lbp always multiplies with a table that contains only one variable
 */
void PotentialTableBase::TableMultiplicationOneVariable(const PotentialTableBase &second_table) {
    if (this->vec_related_variables.empty()) {
        (*this) = second_table; // directly return "second_table"
//        return second_table;
    }
    if (second_table.vec_related_variables.empty()) {
        return; // directly return this table
//        return (*this);
    }

    if (second_table.num_variables > 1) {
        cout << "error in TableMultiplicationOneVariable: the second table has " << second_table.vec_related_variables.size() << " variables" << endl;
    }

    int variable_index = second_table.vec_related_variables[0];

    /**
     * it is just the "to_be_extended", to decide whether table 1 is bigger
     * if not, which means both table 1 and 2 have one variable
     * then directly do the multiplication
     */
    if (this->num_variables > 1) {
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
 * @brief table operation 5: table division
 * @param second_table
 */
void PotentialTableBase::TableDivision(const PotentialTableBase &second_table) {
    // if related variable of both are empty
//    if (this->vec_related_variables.empty()) {
//        // do nothing, just return, because "table" is a constant
//        return;
//    }

//#pragma omp taskloop
    for (int i = 0; i < this->table_size; ++i) {
        if (second_table.potentials[i] == 0) {
            this->potentials[i] = 0;
        } else {
            this->potentials[i] /= second_table.potentials[i];
        }
    }
}

void PotentialTableBase::Normalize() {
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

/**
 * under each possible parent configuration, the sum of probabilities should be 1
 * so we need to do p_dim times normalization, each of which contains dim entries
 * note: in the last loop, denominator can be 0 in the "current stage CPT" generated by SIS and AIS-BN
 *       so we do the judgement
 */
void PotentialTableBase::NormalizeCPT() {
    // do normalization p_dim times
    int p_dim = cum_levels[0];
    int n_dim = var_dims[0];
    for (int i = 0; i < p_dim; ++i) {
        double denominator = 0.0;
        for (int j = 0; j < n_dim; ++j) {
            int index = i + j * p_dim;
            denominator += potentials[index];
        }

        if (denominator > 0) {
            for (int j = 0; j < n_dim; ++j) {
                int index = i + j * p_dim;
                potentials[index] /= denominator;
            }
        }
    }
}

void PotentialTableBase::NormalizeICPT() {
    // do normalization p_dim times
    int p_dim = cum_levels[0];
    int n_dim = var_dims[0];
    for (int i = 0; i < p_dim; ++i) {
        double denominator = 0.0;
        for (int j = 0; j < n_dim; ++j) {
            int index = i + j * p_dim;
            denominator += potentialsICPT[index];
        }

        if (denominator > 0) {
            for (int j = 0; j < n_dim; ++j) {
                int index = i + j * p_dim;
                potentialsICPT[index] /= denominator;
            }
        }
    }
}

void PotentialTableBase::NormalizePtScore() {
    // do normalization p_dim times
    int p_dim = cum_levels[0];
    int n_dim = var_dims[0];
    for (int i = 0; i < p_dim; ++i) {
        double denominator = 0.0;
        for (int j = 0; j < n_dim; ++j) {
            int index = i + j * p_dim;
            denominator += pt_scores[index];
        }

        if (denominator > 0) {
            for (int j = 0; j < n_dim; ++j) {
                int index = i + j * p_dim;
                pt_scores[index] /= denominator;
            }
        }
    }
}

/**
 * under each possible parent configuration, the sum of probabilities should be 1
 * which means the total probabilities of the table should be p_dim, rather than 1
 * so each value should be 1/dim, rather than 1/tab_size
 * NOTE: it is now only used in Heuristic Uniform Distribution in Importance Sampling
 */
void PotentialTableBase::UniformDistribution() {
    double value = 1.0 / this->var_dims[0];
    for (int i = 0; i < this->table_size; ++i) {
        potentialsICPT[i] = value;
    }
}

/*!
 * @brief: get the location of one variable in the related_variables
 * @param variable: one variable in the "related_variables"
 * @return the location of the variable in the "related_variables"
 */
int PotentialTableBase::GetVariableIndex(const int &variable) {
    for (int i = 0; i < this->num_variables; ++i) {
        if (vec_related_variables[i] == variable) {
            return i;
        }
    }
    return this->num_variables;
}

/*!
 * @brief: get each value (index) of the configuration corresponding to the given table index
 * @method: a/b = c...d, from left to right
 *          a -- the table index at the beginning, and then d
 *          b -- cum_levels[i]
 *          c -- save in config_value
 */
void PotentialTableBase::GetConfigValueByTableIndex(const int &table_index, int *config_value, int num_variables, const vector<int> &cum_levels) {
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
int PotentialTableBase::GetTableIndexByConfigValue(int *config_value, int num_variables, const vector<int> &cum_levels) {
    int table_index = 0;
    for (int i = 0; i < num_variables; ++i) {
        table_index += config_value[i] * cum_levels[i];
    }
    return table_index;
}

/*!
 * @brief: get each value (index) of the configuration corresponding to the given table index
 * @method: a/b = c...d, from left to right
 *          a -- the table index at the beginning, and then d
 *          b -- cum_levels[i]
 *          c -- save in config_value
 */
void PotentialTableBase::GetConfigValueByTableIndex(const int &table_index, int *config_value) {
    int a = table_index;
    for (int i = 0; i < num_variables; ++i) {
        int c = a / cum_levels[i];
        int d = a % cum_levels[i];
        config_value[i] = c;
        a = d;
    }
}

/**
 * @brief: get table index given each value (index) of the configuration
 */
int PotentialTableBase::GetTableIndexByConfigValue(int *config_value) {
    int table_index = 0;
    for (int i = 0; i < num_variables; ++i) {
        table_index += config_value[i] * cum_levels[i];
    }
    return table_index;
}

/**
 * @brief: get relative table index (for the variables except for the first variable)
 *         given each value (except for the first value) of the configuration
 */
int PotentialTableBase::GetRelativeIndexByConfigValue(int *config_value) {
    int table_index = 0;
    for (int i = 1; i < num_variables; ++i) {
        table_index += config_value[i - 1] * cum_levels[i];
    }
    return table_index;
}

/**
 * @brief: construct: 1. var_dims; 2. cum_levels; 3. table size
 * it is used in the constructor of PotentialTable
 */
void PotentialTableBase::ConstructVarDimsAndCumLevels(Network *net){
    var_dims.resize(num_variables);
    for (int i = 0; i < num_variables; ++i) {
        int node_idx = vec_related_variables[i];
        var_dims[i] = dynamic_cast<DiscreteNode*>(net->FindNodePtrByIndex(node_idx))->GetDomainSize();
    }

    ConstructCumLevels();
    // compute the table size -- number of possible configurations
    table_size = cum_levels[0] * var_dims[0];
}

/**
 * construct "cum_levels" according to "var_dims"
 */
void PotentialTableBase::ConstructCumLevels() {
    cum_levels.resize(num_variables);
    // set the right-most one ...
    cum_levels[num_variables - 1] = 1;
    // ... then compute the left ones
    for (int i = num_variables - 2; i >= 0; --i) {
        cum_levels[i] = cum_levels[i + 1] * var_dims[i + 1];
    }
}