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

    vec_related_variables.push_back(node_index); // related_variables is empty initially, because this is a constructor

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
        for (auto &p: disc_node->set_parent_indexes) {
            vec_related_variables.push_back(p);
        }
        num_variables = vec_related_variables.size();

        ConstructVarDimsAndCumLevels(net);

        potentials.reserve(table_size);
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
            potentials.push_back(disc_node->GetProbability(node_value, parent_config));
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

//    int j = 0;
//    for (int i = 0; i < this->table_size; ++i) {
//        if (i % dp == lp) {
//            result[j++] = this->potentials[i];
//        }
//    }
}

/**
 * just like the above method, the difference is that now we also know the value of this node, and thus this method returns one value
 */
double PotentialTableBase::GetReducedPotential(const vector<int> &evidence, int num_threads) {

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

/**
 * like the above method, just return the table index rather than the value of the index
 */
int PotentialTableBase::GetReducedTableIndex(const vector<int> &evidence, int num_threads) {

    int *config = new int[this->num_variables];
    /**
     * construct the config with evidence
     */
    for (int i = 0; i < this->num_variables; ++i) {
        config[i] = evidence[this->vec_related_variables[i]];
    }

    int table_index = GetTableIndexByConfigValue(config); // find the table index of this config
    SAFE_DELETE_ARRAY(config);

    return table_index;
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
        new_table.vec_related_variables.reserve(new_table.num_variables);
        new_table.var_dims.reserve(new_table.num_variables);
        for (int i = 0; i < this->num_variables; ++i) {
            if (this->vec_related_variables[i] != ext_variable) {
                new_table.vec_related_variables.push_back(this->vec_related_variables[i]);
                new_table.var_dims.push_back(this->var_dims[i]);
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
        result = this->potentials;
        return;
    }

    int d = this->var_dims[0]; // get the dimension of the node
    int dp = this->table_size / d; // get the dimension of its parents, which is the number of values to be accumulated
    for (int i = 0; i < this->table_size; ++i) {
        int id = i / dp;
        result[id] += this->potentials[i];
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
 * @brief table operation 6: table addition
 * @param second_table should have the same structure as this table, the only difference should be the values in potentials
 */
void PotentialTableBase::TableAddition(const PotentialTableBase &second_table) {
    for (int i = 0; i < this->table_size; ++i) {
        this->potentials[i] += second_table.potentials[i];
    }
}

void PotentialTableBase::TableSubtraction(const PotentialTableBase &second_table) {
    for (int i = 0; i < this->table_size; ++i) {
        this->potentials[i] -= second_table.potentials[i];
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

void PotentialTableBase::UniformDistribution() {
    double value = 1.0 / this->table_size;
    for (int i = 0; i < this->table_size; ++i) {
        potentials[i] = value;
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
    var_dims.reserve(num_variables);
    for (auto &node_idx: vec_related_variables) { // for each node
        var_dims.push_back(dynamic_cast<DiscreteNode*>(net->FindNodePtrByIndex(node_idx))->GetDomainSize());
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