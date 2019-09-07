//
// Created by LinjianLi on 2019/2/9.
//

#include "DiscreteNode.h"

DiscreteNode::DiscreteNode() {
  is_discrete = true;
}

DiscreteNode::DiscreteNode(int index) {
  is_discrete = true;
  SetNodeIndex(index);
}

DiscreteNode::DiscreteNode(int index, string name): DiscreteNode(index) {
  node_name = name;
}

void DiscreteNode::SetDomain(vector<string> str_domain) {
  num_potential_vals = str_domain.size();
  vec_str_potential_vals = str_domain;
  for (const auto &s : str_domain) {
    vec_potential_vals.push_back(vec_potential_vals.size());
  }
  potential_vals = new int[num_potential_vals];
  for (int i=0; i<num_potential_vals; ++i) {
    potential_vals[i] = i;
  }
}

void DiscreteNode::SetDomain(vector<int> int_domain) {
  num_potential_vals = int_domain.size();
  vec_potential_vals = int_domain;
  potential_vals = new int[num_potential_vals];
  for (int i=0; i<num_potential_vals; ++i) {
    potential_vals[i] = int_domain.at(i);
  }
}

void DiscreteNode::AddParent(Node *p) {
  if (!p->is_discrete) {
    fprintf(stderr, "Error in function %s! \n"
                    "Discrete node must not have continuous parent!", __FUNCTION__);
    exit(1);
  }
  set_parents_ptrs.insert(p);
}