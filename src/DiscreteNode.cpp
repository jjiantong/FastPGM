//
// Created by LinjianLi on 2019/2/9.
//

#include "DiscreteNode.h"

void DiscreteNode::AddChild(Node *c) {
  set_children_ptrs.insert(c);
}

void DiscreteNode::AddParent(Node *p) {
  if (p->is_discrete) {
    set_parents_ptrs.insert(p);
  } else {
    fprintf(stderr, "Error in function %s! \n"
                    "Discrete node must not have continuous parent!", __FUNCTION__);
    exit(1);
  }
}