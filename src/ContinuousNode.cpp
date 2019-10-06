//
// Created by LinjianLi on 2019/2/9.
//

#include"ContinuousNode.h"

ContinuousNode::ContinuousNode() {
  is_discrete = false;
}

ContinuousNode::ContinuousNode(int index) {
  SetNodeIndex(index);
  is_discrete = false;
}

ContinuousNode::ContinuousNode(int index, string name): ContinuousNode(index) {
  node_name = name;
}

void ContinuousNode::AddChild(Node *c) {
  if (c->is_discrete) {
    fprintf(stderr, "Error in function %s! \n"
                    "Continuous node must not have discrete child!", __FUNCTION__);
    exit(1);
  }
  set_children_ptrs.insert(c);
}

void ContinuousNode::AddParent(Node *p) {
  set_parents_ptrs.insert(p);
  if (!p->is_discrete) {
    contin_par_indexes.push_back(p->GetNodeIndex());
  }
}

void ContinuousNode::RemoveParent(Node *p) {
  if (set_parents_ptrs.find(p)==set_parents_ptrs.end()) {
    fprintf(stderr, "Node #%d does not have parent node #%d!", this->GetNodeIndex(), p->GetNodeIndex());
    return;
  }
  set_parents_ptrs.erase(p);
  if (!p->is_discrete) {
    auto it = contin_par_indexes.begin();
    while (*it!=p->GetNodeIndex()) { ++it; }
    contin_par_indexes.erase(it);
  }
}

int ContinuousNode::GetNumParams() const {
  int scale = this->set_discrete_parents_combinations.empty() ? 1 : this->set_discrete_parents_combinations.size();
  int num_params_for_a_config = 0;
  num_params_for_a_config += 2;  // For mu and variance.
  num_params_for_a_config += contin_par_indexes.size();  // For coefficients.
  return num_params_for_a_config * scale;
}

void ContinuousNode::ClearParams() {
  fprintf(stderr, "Function [%s] not implemented yet!", __FUNCTION__);
  exit(1);
}

void ContinuousNode::IdentifyContPar() {
  contin_par_indexes.clear();
  for (const auto &p : set_parents_ptrs) {
    if (!p->is_discrete) {
      contin_par_indexes.push_back(p->GetNodeIndex());
    }
  }
}
