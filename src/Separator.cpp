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

    if (set_external_vars.empty()) { // two tables have the same size
        // don't need to do marginalization,
        // but before division, need to first decide whether the orders are the same
        if (p_table.vec_related_variables != pt.vec_related_variables) {
            // if not have the same order, change the order to this table's order

//            cout << "same size and different order" << endl;
//            cout << "this separator table: ";
//            for (int i = 0; i < this->p_table.num_variables; ++i) {
//                cout << this->p_table.vec_related_variables[i] << " ";
//            }
//            cout << endl << "potentials: ";
//            for (int i = 0; i < this->p_table.table_size; ++i) {
//                cout << this->p_table.potentials[i] << " ";
//            }
//            cout << endl;
//
//            cout << "its neighboring clique table: ";
//            for (int i = 0; i < pt.num_variables; ++i) {
//                cout << pt.vec_related_variables[i] << " ";
//            }
//            cout << endl << "potentials: ";
//            for (int i = 0; i < pt.table_size; ++i) {
//                cout << pt.potentials[i] << " ";
//            }
//            cout << endl;

            tmp_pt.TableReorganization(p_table);

//            cout << "after reorganization: ";
//            for (int i = 0; i < tmp_pt.num_variables; ++i) {
//                cout << tmp_pt.vec_related_variables[i] << " ";
//            }
//            cout << endl << "potentials: ";
//            for (int i = 0; i < tmp_pt.table_size; ++i) {
//                cout << tmp_pt.potentials[i] << " ";
//            }
//            cout << endl;
        }
    } else {
        // need to do the marginalization
        tmp_pt.TableMarginalization(set_external_vars);
    }


//    if (!set_external_vars.empty()) {
//        tmp_pt.TableMarginalization(set_external_vars);
//    }
//
    tmp_pt.TableDivision(old_ptable);

    p_table = tmp_pt;

//    cout << "related variables: ";
//    for (int i = 0; i < p_table.num_variables; ++i) {
//        cout << p_table.vec_related_variables[i] << ", ";
//    }
//    cout << endl;
//    cout << "potentials: ";
//    for (int i = 0; i < p_table.table_size; ++i) {
//        cout << p_table.potentials[i] << ", ";
//    }
//    cout << endl;
}