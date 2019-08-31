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

void ContinuousNode::AddChild(Node *c) {
  if (c->is_discrete) {
    fprintf(stderr, "Error in function %s! \n"
                    "Continuous node must not have discrete child!", __FUNCTION__);
    exit(1);
  }
  set_parents_ptrs.insert(c);
}
