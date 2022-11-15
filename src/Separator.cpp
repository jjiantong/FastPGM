#include "Separator.h"

Separator::Separator() {
    is_in_jt = false;
    weight = -1;
}

Separator::Separator(set<int> set_node_index, Network *net): Clique(set_node_index, net) {
    is_in_jt = false;
    weight = clique_size;
}

/**
 * merge the above two methods
 */
void Separator::UpdateMessage(const PotentialTable &pt) {
    old_ptable = p_table;
    PotentialTable tmp_pt = pt;
//    SumOutExternalVars(tmp_pt);
    set<int> set_external_vars;
    set_difference(tmp_pt.vec_related_variables.begin(), tmp_pt.vec_related_variables.end(),
                   this->clique_variables.begin(), this->clique_variables.end(),
                   inserter(set_external_vars, set_external_vars.begin()));

    tmp_pt.TableMarginalization(set_external_vars);
    tmp_pt.TableDivision(old_ptable);

    p_table = tmp_pt;
}