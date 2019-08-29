//
// Created by LinjianLi on 2019/2/9.
//

#include"ContinuousNode.h"

void ContinuousNode::AddChild(Node *c) {
  if (!c->is_discrete) {
    set_parents_ptrs.insert(c);
  } else {
    fprintf(stderr, "Error in function %s! \n"
                    "Continuous node must not have discrete child!", __FUNCTION__);
    exit(1);
  }
}

void ContinuousNode::AddParent(Node *p) {
  set_parents_ptrs.insert(p);
}