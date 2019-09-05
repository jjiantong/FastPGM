//
// Created by LinjianLi on 2019/9/1.
//

#include "EliminationTree.h"

EliminationTree::EliminationTree(Network *net)
  : JunctionTree(net, "rev-topo", false) {
  int last_elem_in_rev_topo_ord = elimination_ordering.back();
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

void EliminationTree::InitCGRegressions() {
  // todo: test correctness
  // Described in
  // [Local Propagation in Conditional Gaussian Bayesian Networks (Cowell, 2005)]
  // section 5.5.
  vector<int> topo_ord = this->network->GetTopoOrd();
  for (const auto &v : elimination_ordering) {
    if (map_elim_var_to_clique[v]->pure_discrete) { break; }
    Clique *clq = map_elim_var_to_clique[v];
    // Sort the CG regressions so that the head variables are compatible with topological ordering.
    sort(
      clq->post_bag.begin(),
      clq->post_bag.end(),
      [&topo_ord](CGRegression &a, CGRegression &b) {
        return OccurInCorrectOrder(a.head_var_index, b.head_var_index, topo_ord);
      }
    );
    while (!clq->post_bag.empty()) {
      CGRegression R = clq->post_bag.front();
      clq->post_bag.erase(clq->post_bag.begin());
      if (R.set_all_tail_index.find(clq->elimination_variable_index)!=R.set_all_tail_index.end()) {
        CGRegression::Exchange(R, clq->lp_potential.front());
      }
      Clique *c_ji = map_elim_var_to_clique[map_to_root[clq->elimination_variable_index]];
      if (c_ji->elimination_variable_index==R.head_var_index) {
        c_ji->lp_potential.push_back(R);
      } else {
        c_ji->post_bag.push_back(R);
      }
    }
  }
}