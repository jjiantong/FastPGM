#include "Separator.h"

Separator::Separator() {
    is_in_jt = false;
  is_separator = true;
  weight = -1;
}

Separator::Separator(set<int> set_node_index, Network *net): Clique(set_node_index, net) {
    is_in_jt = false;
    is_separator = true;
    weight = clique_size;
}

Separator* Separator::CopyWithoutPtr() {
  auto s = new Separator(*this);
  s->set_neighbours_ptr.clear();
  s->ptr_upstream_clique = nullptr;
  return s;
}

//void Separator::UpdateUseMessage2(const PotentialTable &pt) {
//    old_ptable = p_table;
//    PotentialTable tmp_pt = pt;
//    SumOutExternalVars(tmp_pt);
//    p_table = tmp_pt;
//}
//
//void Separator::ConstructMessage2() {
//    p_table.TableDivision(old_ptable);
//}

/**
 * merge the above two methods
 */
void Separator::UpdateMessage(const PotentialTable &pt) {
    old_ptable = p_table;
    PotentialTable tmp_pt = pt;
//    SumOutExternalVars(tmp_pt);
    set<int> set_external_vars;
    set_difference(tmp_pt.related_variables.begin(), tmp_pt.related_variables.end(),
                   this->clique_variables.begin(), this->clique_variables.end(),
                   inserter(set_external_vars, set_external_vars.begin()));

    tmp_pt.TableMarginalization(set_external_vars);
    tmp_pt.TableDivision(old_ptable);

    p_table = tmp_pt;
}