//
// Created by LinjianLi on 2019/2/20.
//

#include "Separator.h"

Separator::Separator(set<Node*> set_node_ptr) {
  is_separator = true;
  weight = clique_size = set_node_ptr.size();
  if (weight!=0) {InitializeClique(set_node_ptr);}
}

void Separator::UpdateUseMessage(Factor &f) {
  SumOutExternalVars(f);
  map_old_potentials = map_potentials;
  map_potentials = f.map_potentials;
}

Factor Separator::ConstructMessage() {
  // todo: implement
  Factor f;
  f.SetMembers(related_variables,set_combinations,map_potentials);
  for (auto &comb : set_combinations) {
    if (map_old_potentials[comb]==0) {
      f.map_potentials[comb] = 1;
    } else {
      f.map_potentials[comb] /= map_old_potentials[comb];
    }
  }
  return f;
}