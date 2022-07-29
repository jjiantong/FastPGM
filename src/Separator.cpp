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

///************************* use factor ******************************/
//void Separator::UpdateUseMessage(const Factor &f, Timer *timer) {
//    old_table = table;
//    Factor tmp_f = f;
//    SumOutExternalVars(tmp_f, timer);
//    table = tmp_f;
//}
//
///**
// * @brief: this is a standard process for constructing the message of the separator cliques.
// */
//void Separator::ConstructMessage(Timer *timer) {
//    // if related variable of both new and old are empty
//    if (table.related_variables.empty()) {
//        // do nothing, just return, because "table" is a constant
//        return;
//    }
//
//    for (auto &comb : table.set_disc_configs) {
//        if (old_table.map_potentials[comb] == 0) {
//            table.map_potentials[comb] = 0;
//        } else {
//            table.map_potentials[comb] /= old_table.map_potentials[comb];
//        }
//    }
//}
///************************* use factor ******************************/

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