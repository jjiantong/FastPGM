#include "Separator.h"

Separator::Separator() {
    is_in_jt = false;
  is_separator = true;
  weight = -1;
}

Separator::Separator(set<Node*> set_node_ptr): Clique(set_node_ptr) {
    is_in_jt = false;
  is_separator = true;
  weight = clique_size;
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


void Separator::UpdateUseMessage(const Factor &f, Timer *timer) {
    old_table = table;
    Factor tmp_f = f;
    SumOutExternalVars(tmp_f, timer);
    table = tmp_f;
}

void Separator::UpdateUseMessage2(const PotentialTable &pt, Timer *timer) {
    old_ptable = p_table;
    PotentialTable tmp_pt = pt;
    SumOutExternalVars(tmp_pt, timer);
    p_table = tmp_pt;
}

/**
 * @brief: this is a standard process for constructing the message of the separator cliques.
 */
void Separator::ConstructMessage(Timer *timer) {
//    timer->Start("factor division");
    // if related variable of both new and old are empty
    if (table.related_variables.empty()) {
        // do nothing, just return, because "table" is a constant
        return;
    }

    for (auto &comb : table.set_disc_configs) {
        if (old_table.map_potentials[comb] == 0) {
            table.map_potentials[comb] = 0;
        } else {
            table.map_potentials[comb] /= old_table.map_potentials[comb];
        }
    }
//    timer->Stop("factor division");
}

void Separator::ConstructMessage2(Timer *timer) {
//    timer->Start("factor division");
    p_table.TableDivision(old_ptable);
//    timer->Stop("factor division");
}