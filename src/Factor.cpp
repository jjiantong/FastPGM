//
// Created by Linjian Li on 2018/11/29.
//

#include "Factor.h"


Factor::Factor() {}


void Factor::ConstructFactor(Node *node) {

  related_variables.insert(node->GetNodeIndex());

  set<pair<int,int>> set_pair_temp;
  for (int i=0; i<node->num_potential_vals; ++i) {
    Combination comb_temp;
    pair<int, int> pair_temp;
    pair_temp.first = node->GetNodeIndex();
    pair_temp.second = node->potential_vals[i];
    set_pair_temp.insert(pair_temp);
  }

  // If this node has no parents.
  if (node->set_parents_ptrs.empty()) {
    for (auto &p : set_pair_temp) {
      Combination c;
      c.insert(p);
      set_combinations.insert(c);
      map_potentials[c] = node->map_marg_prob_table[p.second];
    }
    return;
  }

  // If this node has parents, the outer loop is for the node, and the inner loop is for the parents.
  for (auto &p : node->set_parents_ptrs) {
    related_variables.insert(p->GetNodeIndex());
  }
  for (auto &p : set_pair_temp) {
    for (auto it_pc=node->set_parents_combinations.begin(); it_pc!=node->set_parents_combinations.end(); ++it_pc) {
      Combination c = (*it_pc);
      c.insert(p);
      set_combinations.insert(c);
      map_potentials[c] = node->map_cond_prob_table[p.second][*it_pc];
    }
  }
}


Factor Factor::MultiplyWithFactor(Factor second_factor) {
  Factor newFactor;

  newFactor.related_variables.insert(this->related_variables.begin(),this->related_variables.end());
  newFactor.related_variables.insert(second_factor.related_variables.begin(),second_factor.related_variables.end());

  set<int> common_related_variables;
  set_intersection(this->related_variables.begin(),this->related_variables.end(),
            second_factor.related_variables.begin(),second_factor.related_variables.end(),
            std::inserter(common_related_variables,common_related_variables.begin()));

  for (auto first: set_combinations) {
    for (auto second : second_factor.set_combinations) {

      // solve the bug about common variables
      // If two combinations have different values on common variables,
      // then these two combinations can not form a legal entry.
      if (!FirstCompatibleSecond(&first, &second)) continue;

      Combination new_comb;
      new_comb.insert(first.begin(),first.end());
      new_comb.insert(second.begin(),second.end());
      newFactor.set_combinations.insert(new_comb);
      newFactor.map_potentials[new_comb] = this->map_potentials[first] * second_factor.map_potentials[second];
    }
  }
  return newFactor;
}


Factor Factor::SumProductOverVariable(Node *node) {
  Factor newFactor;
  this->related_variables.erase(node->GetNodeIndex());
  newFactor.related_variables = this->related_variables;
  for (auto comb : set_combinations) {
    pair<int, int> pair_to_be_erased;
    for (auto p : comb) {
      if (p.first==node->GetNodeIndex()) {
        pair_to_be_erased = p;
        break;
      }
    }
    double temp = this->map_potentials[comb];
    comb.erase(pair_to_be_erased);
    if (newFactor.set_combinations.find(comb)!=newFactor.set_combinations.end()) {
      newFactor.map_potentials[comb] += temp;
    } else {
      newFactor.set_combinations.insert(comb);
      newFactor.map_potentials[comb] = temp;
    }
  }
  return newFactor;
}


void Factor::Normalize() {
  double denominator = 0;
  for (auto &comb : set_combinations) {
    denominator += map_potentials[comb];
  }
  for (auto &comb : set_combinations) {
    map_potentials[comb] /= denominator;
  }
}