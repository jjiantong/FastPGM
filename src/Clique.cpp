//
// Created by LinjianLi on 2019/2/16.
//

#include "Clique.h"

Clique::Clique(set<Node*> set_node_ptr) {
  clique_size = set_node_ptr.size();

  set<Combination> set_of_sets;

  for (auto &n : set_node_ptr) {
    related_variables.insert(n->GetNodeIndex());
    Combination c;
    for (int i=0; i<n->num_potential_vals; ++i) {
      c.insert(pair(n->GetNodeIndex(),n->potential_vals[i]));
    }
    set_of_sets.insert(c);
  }

  set_combinations = GenAllCombFromSets(&set_of_sets);

  for (auto &c : set_combinations) {
    map_potentials[c] = 1;  // Initialize clique potential to be 1.
  }
}





void Clique::MultiplyWithFactor(Factor f) {
  Factor f_temp;
  f_temp.related_variables = related_variables;
  f_temp.map_potentials = map_potentials;
  f_temp = f_temp.MultiplyWithFactor(f);

  related_variables = f_temp.related_variables;
  f_temp.map_potentials = f_temp.map_potentials;
}