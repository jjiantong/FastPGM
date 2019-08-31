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

void DiscreteNode::AddParent(Node *p) {
  if (!p->is_discrete) {
    fprintf(stderr, "Error in function %s! \n"
                    "Discrete node must not have continuous parent!", __FUNCTION__);
    exit(1);
  }
  set_parents_ptrs.insert(p);
}