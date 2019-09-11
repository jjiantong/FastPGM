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

void ContinuousNode::IdentifyContPar() {
  contin_par_indexes.clear();
  for (const auto &p : set_parents_ptrs) {
    if (!p->is_discrete) {
      contin_par_indexes.push_back(p->GetNodeIndex());
    }
  }
}
