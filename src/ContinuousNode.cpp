//
// Created by LinjianLi on 2019/2/9.
//

#include"ContinuousNode.h"
#include "DiscreteNode.h"

ContinuousNode::ContinuousNode(): ContinuousNode(-1) {}

ContinuousNode::ContinuousNode(int index): ContinuousNode(index, to_string(index)) {}

ContinuousNode::ContinuousNode(int index, string name) {
  SetNodeIndex(index);
  is_discrete = false;
  node_name = std::move(name);
}

void ContinuousNode::AddChild(Node *c) {
  if (c->is_discrete) {
    fprintf(stderr, "Error in function %s! \n"
                    "Continuous node must not have discrete child!", __FUNCTION__);
    exit(1);
  }
  int c_idx = c->GetNodeIndex();
  if (set_children_indexes.find(c_idx) == set_children_indexes.end()) {
    set_children_indexes.insert(c_idx);
  } else {
    fprintf(stdout, "Node #%d is already child of Node #%d", c_idx, this->GetNodeIndex());
  }
}

void ContinuousNode::AddParent(Node *p) {

  int p_idx = p->GetNodeIndex();
  if (set_parent_indexes.find(p_idx) == set_parent_indexes.end()) {
    set_parent_indexes.insert(p_idx);
    vec_disc_parent_indexes.push_back(p_idx);
    if (p->is_discrete) {
      map_disc_parents_domain_size[p_idx] = ((DiscreteNode*)p)->GetDomainSize();
    } else {
      contin_par_indexes.push_back(p->GetNodeIndex());
    }
  } else {
    fprintf(stdout, "Node #%d is already parent of Node #%d", p_idx, this->GetNodeIndex());
  }
}

void ContinuousNode::RemoveParent(Node *p) {
  int p_idx = p->GetNodeIndex();
  if (set_parent_indexes.find(p_idx)==set_parent_indexes.end()) {
    fprintf(stderr, "Node #%d does not have parent node #%d!", this->GetNodeIndex(), p_idx);
    return;
  }
  if (!p->is_discrete) {
    auto it = contin_par_indexes.begin();
    while (*it!=p->GetNodeIndex()) { ++it; }
    contin_par_indexes.erase(it);
  }
  set_parent_indexes.erase(p_idx);
  vec_disc_parent_indexes.erase(std::find(vec_disc_parent_indexes.begin(), vec_disc_parent_indexes.end(), p_idx));
}

int ContinuousNode::GetNumParams() {
  int scale = GetNumParentsConfig();
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
  set<int> set_disc_par(vec_disc_parent_indexes.begin(), vec_disc_parent_indexes.end());

  set_difference(set_parent_indexes.begin(), set_parent_indexes.end(),
                 set_disc_par.begin(), set_disc_par.end(),
                 inserter(contin_par_indexes, contin_par_indexes.begin()));
}
