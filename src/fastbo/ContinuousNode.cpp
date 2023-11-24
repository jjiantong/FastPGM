#include"fastbo/ContinuousNode.h"

ContinuousNode::ContinuousNode(): ContinuousNode(-1) {}

ContinuousNode::ContinuousNode(int index): ContinuousNode(index, to_string(index)) {}

ContinuousNode::ContinuousNode(int index, string name) {
  SetNodeIndex(index);
  is_discrete = false;
  node_name = std::move(name);
}

/**
 * @brief: add child; can be merged with AddChild in the base class.
 */
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

/**
 * @brief: remove parent; possibly only used in structure learning. merge?
 */
void ContinuousNode::RemoveParent(Node *p) {
  Node::RemoveParent(p);

  if (!p->is_discrete) {
    //remove the parent from the vector based on parent id
    auto it = contin_par_indexes.begin();
    while (*it!=p->GetNodeIndex()) { ++it; }
    contin_par_indexes.erase(it);
  }
}

//void ContinuousNode::ClearParams() {
//  //TODO: incomplete implementation
//  fprintf(stderr, "Function [%s] not implemented yet!", __FUNCTION__);
//  exit(1);
//}