#include "Separator.h"

Separator::Separator() {
    is_in_jt = false;
}

Separator::Separator(set<int> set_node_index, Network *net): Clique(set_node_index, net) {
    is_in_jt = false;
}

/**
 * merge the above two methods
 */
void Separator::UpdateMessage(const PotentialTable &pt) {
    old_ptable = p_table;
    PotentialTable tmp_pt = pt;

    set<int> set_external_vars;
//    set_difference(tmp_pt.vec_related_variables.begin(), tmp_pt.vec_related_variables.end(),
//                   this->p_table.vec_related_variables.begin(), this->p_table.vec_related_variables.end(),
//                   inserter(set_external_vars, set_external_vars.begin()));

    // TODO: a clique has the same order with its parent separator, but doesn't have the same order with its children separators,
    //  so for distribution, this implementation is wrong -- maybe need to consider the two different cases
//    // implement set_difference: since set_difference requires the two containers have ordered elements
//    // remove this->p_table.vec_related_variables (j) from tmp_pt.vec_related_variables (i)
//    int i = 0, j = 0;
//    while (i < tmp_pt.num_variables && j < this->p_table.num_variables) {
//        while (tmp_pt.vec_related_variables[i] != this->p_table.vec_related_variables[j]) {
//            // this variable of tmp_pt is not in this separator, keep it
//            set_external_vars.insert(tmp_pt.vec_related_variables[i]);
//            i++;
//        } // end of while, now tmp pt i == this j
//        // this variable is in this separator, skip it
//        i++;
//        j++;
//    } // end of while, two possible cases: 1. i = tmp_pt.num_variables; 2. j = this->p_table.num_variables
//
//    // if only 2 but not 1, post-process the left elements
//    while (i < tmp_pt.num_variables) {
//        // this variable of tmp_pt is not in this separator, keep it
//        set_external_vars.insert(tmp_pt.vec_related_variables[i]);
//        i++;
//    }

    vector<int> big = pt.vec_related_variables;
    vector<int> small = this->p_table.vec_related_variables;
    sort(big.begin(), big.end());
    sort(small.begin(), small.end());
    set_difference(big.begin(), big.end(),small.begin(), small.end(),
                   inserter(set_external_vars, set_external_vars.begin()));

    if (!set_external_vars.empty()) {
        // need to do the marginalization
        tmp_pt.TableMarginalization(set_external_vars);
    }

    // after marginalization, two tables have the same size
    // or if marginalization is not required, then the two tables already have the same size
    // but before division, we need to check whether the orders are the same
    if (p_table.vec_related_variables != pt.vec_related_variables) {
        // if not have the same order, change the order to this table's order
        tmp_pt.TableReorganization(p_table);

    }

    tmp_pt.TableDivision(old_ptable);
    p_table = tmp_pt;
}