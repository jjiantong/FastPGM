//
// Created by LinjianLi on 2019/9/1.
//

#include "EliminationTree.h"

EliminationTree::EliminationTree(Network *net)
  : JunctionTree(net, "rev-topo", false) {
  int last_elem_in_rev_topo_ord = (net->GetTopoOrd()).front();
  for (const auto &c : this->set_clique_ptr_container) {
    if (c->elimination_variable_index==last_elem_in_rev_topo_ord) {
      this->strong_root = c;
      break;
    }
  }
}

void EliminationTree::CalcuEachToRoot() {
  // todo: test correctness
  map_to_root[strong_root->elimination_variable_index] = -1;
  queue<Clique*> to_be_calcu;
  to_be_calcu.push(strong_root);
  while (!to_be_calcu.empty()) {
    Clique* this_clique_ptr = to_be_calcu.front();
    to_be_calcu.pop();
    for (const auto &sep : this_clique_ptr->set_neighbours_ptr) {
      for (const auto &clq : sep->set_neighbours_ptr) {
        if (map_to_root.find(clq->elimination_variable_index)==map_to_root.end()) {
          to_be_calcu.push(clq);
          map_to_root[clq->elimination_variable_index] = this_clique_ptr->elimination_variable_index;
        }
      }
    }
  }
}
