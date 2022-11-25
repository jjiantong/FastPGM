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

    if (!disc_node->HasParents()) {
        // if this disc_node has no parents. then the potential table only contains 1 variable
        num_variables = 1;
        vec_related_variables.push_back(node_index);
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
        set<int> set_related_variables;
        set_related_variables.insert(node_index);
        set_related_variables.insert(disc_node->set_parent_indexes.begin(), disc_node->set_parent_indexes.end());
        num_variables = set_related_variables.size();
        vec_related_variables.resize(num_variables);
        int i = 0;
        for (auto &rv: set_related_variables) {
            vec_related_variables[i++] = rv;
        }

        ConstructVarDimsAndCumLevels(net);

        potentials.reserve(table_size);
        for (int i = 0; i < table_size; ++i) {
            // find the discrete config by the table index, the config contains this node and its parents
            DiscreteConfig parent_config;
//            GetConfigByTableIndex(i, net, parent_config);

            int *config_value = new int[num_variables];
            PotentialTableBase::GetConfigValueByTableIndex(i, config_value);
            int j = 0;
            for (auto &v: vec_related_variables) {
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

    vec_related_variables.push_back(node_index); // related_variables is empty initially, because this is a constructor
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
 * @brief: construct: 1. var_dims; 2. cum_levels; 3. table size
 * it is used in the constructor of PotentialTable
 */
void PotentialTable::ConstructVarDimsAndCumLevels(Network *net){
    var_dims.resize(num_variables);
    for (int i = 0; i < num_variables; ++i) {
        var_dims[i] = dynamic_cast<DiscreteNode*>(net->FindNodePtrByIndex(vec_related_variables[i]))->GetDomainSize();
    }

    ConstructCumLevels();
    // compute the table size -- number of possible configurations
    table_size = cum_levels[0] * var_dims[0];
}

/*!
 * @brief: get each value (index) of the configuration corresponding to the given table index
 * @method: a/b = c...d, from left to right
 *          a -- the table index at the beginning, and then d
 *          b -- cum_levels[i]
 *          c -- save in config_value
 */
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
    for (int i = 0; i < num_variables; ++i) {
        if (vec_related_variables[i] == variable) {
            return i;
        }
    }
    return num_variables;
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
void PotentialTable::TableReorganization(const PotentialTable &refer_table) {
    // all things except for potentials are maintained
    PotentialTable new_table = refer_table;

    // the locations of the elements in the old table
    // e.g., locations[0] means the locations of new_table.vec_related_variables[0] in this->vec_related_variables
    // i.e., new_table.vec_related_variables[0] = this->vec_related_variables[locations[0]]
    int *locations = new int[this->num_variables];
    for (int i = 0; i < new_table.num_variables; ++i) { // for each new table's related variable
        int variable = new_table.vec_related_variables[i];
        // find the location of this variable in the old table
        for (int j = 0; j < this->num_variables; ++j) {
            if (variable == this->vec_related_variables[j]) {
                locations[i] = j;
                break;
            }
        }
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

void PotentialTable::TableReorganizationPre(const vector<int> &common_variables, PotentialTable &new_table, int *locations) {
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

int PotentialTable::TableReorganizationMain(int k, int *config1, int *config2, PotentialTable &old_table, int *locations) {
    // 1. get the config value of old table
    old_table.PotentialTableBase::GetConfigValueByTableIndex(k, config1);
    // 2. get the config value of new table from the config value of old table
    for (int l = 0; l < this->num_variables; ++l) {
        config2[l] = config1[locations[l]];
    }
    // 3. obtain the potential index of new table
    return PotentialTableBase::GetTableIndexByConfigValue(config2);
}

// note that this method is used in junction tree class.
// it is not used in the TableReorganization method of this class.
// because the two use different getconfigvaluebytableindex method.
int PotentialTable::TableReorganizationMain(int k, int *config1, int *config2, const vector<int> &cl, int *locations) {
    // note the difference from marginalization/extension main:
    // nv (of old table) = this->num_variables
    // 1. get the config value of old table
    GetConfigValueByTableIndex(k, config1 + k * this->num_variables, this->num_variables, cl);
    // 2. get the config value of new table from the config value of old table
    for (int l = 0; l < this->num_variables; ++l) {
        config2[k * this->num_variables + l] = config1[k * this->num_variables + locations[l]];
    }
    // 3. obtain the potential index of new table
    return PotentialTableBase::GetTableIndexByConfigValue(config2 + k * this->num_variables);
}

void PotentialTable::TableReorganizationPost(const PotentialTable &pt, int *table_index) {
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
    PotentialTableBase::GetConfigValueByTableIndex(i, full_config + i * this->num_variables);
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
 * @brief: table operation 2: table marginalization - factor out a set of nodes by id
 * eliminate variable "id" by summation of the factor over "id"
 */
void PotentialTable::TableMarginalization(const vector<int> &variables, const vector<int> &dims) {
    PotentialTable new_table;
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
        PotentialTableBase::GetConfigValueByTableIndex(i, full_config + i * this->num_variables);
        // 2. get the partial config value from the old table
        for (int j = 0; j < new_table.num_variables; ++j) {
            partial_config[i * new_table.num_variables + j] = full_config[i * this->num_variables + loc_in_old[j]];
        }
        // 3. obtain the potential index
        table_index[i] = new_table.PotentialTableBase::GetTableIndexByConfigValue(partial_config + i * new_table.num_variables);
    }
    SAFE_DELETE_ARRAY(full_config);
    SAFE_DELETE_ARRAY(partial_config);
    SAFE_DELETE_ARRAY(loc_in_old);

    new_table.TableMarginalizationPost(*this, table_index);
    (*this) = new_table;
}

void PotentialTable::TableMarginalizationPre(const vector<int> &variables, const vector<int> &dims) {
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
int PotentialTable::TableMarginalizationMain(int k, int *full_config, int *partial_config,
                                             int nv, const vector<int> &cl, int *loc) {
    // 1. get the full config value of old table
    GetConfigValueByTableIndex(k, full_config + k * nv, nv, cl);
    // 2. get the partial config value from the old table
    for (int l = 0; l < this->num_variables; ++l) {
        partial_config[k * this->num_variables + l] = full_config[k * nv + loc[l]];
    }
    // 3. obtain the potential index
    return PotentialTableBase::GetTableIndexByConfigValue(partial_config + k * this->num_variables);
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
        PotentialTableBase::GetConfigValueByTableIndex(i, full_config + i * this->num_variables);
        // 2. get the partial config value from the old table
        for (int j = 0; j < new_table.num_variables; ++j) {
            partial_config[i * new_table.num_variables + j] = full_config[i * this->num_variables + loc_in_old[j]];
        }
        // 3. obtain the potential index
        table_index[i] = new_table.PotentialTableBase::GetTableIndexByConfigValue(partial_config + i * new_table.num_variables);
    }
    SAFE_DELETE_ARRAY(full_config);
    SAFE_DELETE_ARRAY(partial_config);
    SAFE_DELETE_ARRAY(loc_in_old);

    new_table.TableMarginalizationPost(*this, table_index);
    (*this) = new_table;
}

void PotentialTable::TableMarginalizationPre(int ext_variable, PotentialTable &new_table) {
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
 * @brief table operation 3: table extension - a pre computation of multiplication
 * before doing multiplication, we can first let the two tables have the same entries
 * so we need to extend the tables to let them have the same related variables
 */
void PotentialTable::TableExtension(const vector<int> &variables, const vector<int> &dims) {
    PotentialTable new_table;

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
        new_table.PotentialTableBase::GetConfigValueByTableIndex(i, full_config + i * new_table.num_variables);
        // 2. get the partial config value from the new table
        for (int j = 0; j < this->num_variables; ++j) {
            partial_config[i * this->num_variables + j] = full_config[i * new_table.num_variables + loc_in_new[j]];
        }
        // 3. obtain the potential index
        table_index[i] = PotentialTableBase::GetTableIndexByConfigValue(partial_config + i * this->num_variables);
    }
    SAFE_DELETE_ARRAY(full_config);
    SAFE_DELETE_ARRAY(partial_config);
    SAFE_DELETE_ARRAY(loc_in_new);

    new_table.TableExtensionPost(*this, table_index);
    SAFE_DELETE_ARRAY(table_index);

    (*this) = new_table;
}

void PotentialTable::TableExtensionPre(const vector<int> &variables, const vector<int> &dims) {
    vec_related_variables = variables;
    num_variables = variables.size();

    var_dims = dims;
    ConstructCumLevels();
    table_size = cum_levels[0] * var_dims[0];
    potentials.resize(table_size);
}

int PotentialTable::TableExtensionMain(int k, int *full_config, int *partial_config,
                                        int nv, const vector<int> &cl, int *loc) {
    // 1. get the full config value of new table
    PotentialTableBase::GetConfigValueByTableIndex(k, full_config + k * this->num_variables);
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
//    if (second_table.vec_related_variables.empty()) {
//        return; // directly return this table
////        return (*this);
//    }

    PotentialTable tmp_pt = second_table;

    if (this->num_variables - second_table.num_variables > 0) { // if table2 should be extended and table1 not
        tmp_pt.TableExtension(this->vec_related_variables, this->var_dims);
    }

    // before multiplication, need to first decide whether the orders are the same
    if (this->vec_related_variables != second_table.vec_related_variables) {
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
void PotentialTable::TableMultiplicationTwoExtension(PotentialTable &second_table) {
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
 * @brief table operation 5: table division
 * @param second_table
 */
void PotentialTable::TableDivision(const PotentialTable &second_table) {
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