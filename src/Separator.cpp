//
// Created by LinjianLi on 2019/2/20.
//

#include "Separator.h"

Separator::Separator() {
  is_separator = true;
  weight = -1;
}

Separator::Separator(set<Node*> set_node_ptr): Clique(set_node_ptr, -1) {
  is_separator = true;
  weight = clique_size;
}


Separator* Separator::CopyWithoutPtr() {
  auto s = new Separator(*this);
  s->set_neighbours_ptr.clear();
  s->ptr_upstream_clique = nullptr;
  return s;
}


void Separator::UpdateUseMessage(Factor f) {
  f = SumOutExternalVars(f);
  map_old_potentials = map_potentials;
  map_potentials = f.map_potentials;
}

/**
 * @brief: this is a standard process for constructing the message of the separator cliques.
 */
Factor Separator::ConstructMessage() {
  Factor f(related_variables, set_disc_configs, map_potentials);
  for (auto &comb : set_disc_configs) {
    if (map_old_potentials[comb]==0) {
      f.map_potentials[comb] = 1;
    } else {
      f.map_potentials[comb] /= map_old_potentials[comb];
    }
  }
  return f;
}