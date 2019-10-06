//
// Created by Linjian Li on 2018/11/29.
//

#include "Node.h"
#include "DiscreteNode.h"

Node::Node(int index) {
  SetNodeIndex(index);
}

Node::Node(int index, string name): Node(index) {
  node_name = name;
}

int Node::GetNodeIndex() const {
  return node_index;
}


void Node::SetNodeIndex(int i) {
  if (i<0) {
    fprintf(stderr, "Error in function %s! \nInvalid index!", __FUNCTION__);
    exit(1);
  }
  node_index = i;
}


void Node::AddChild(Node *c) {
  set_children_ptrs.insert(c);
}


void Node::AddParent(Node *p) {
  set_parents_ptrs.insert(p);
}


void Node::RemoveChild(Node *c) {
  if (set_children_ptrs.find(c)==set_children_ptrs.end()) {
    fprintf(stderr, "Node #%d does not have parent node #%d!", this->GetNodeIndex(), c->GetNodeIndex());
    return;
  }
  set_children_ptrs.erase(c);
}


void Node::RemoveParent(Node *p) {
  if (set_parents_ptrs.find(p)==set_parents_ptrs.end()) {
    fprintf(stderr, "Node #%d does not have parent node #%d!", this->GetNodeIndex(), p->GetNodeIndex());
    return;
  }
  set_parents_ptrs.erase(p);
}


void Node::GenDiscParCombs() {
  set_discrete_parents_combinations.clear();

  // Preprocess. Construct set of sets.
  set<DiscreteConfig> set_of_sets;
  if (set_parents_ptrs.empty()) {return;}
  for (const auto par_ptr : set_parents_ptrs) {
    if (!par_ptr->is_discrete) { continue; }
    DiscreteConfig cb;
    pair<int, int> ele;
    for (int i=0; i<dynamic_cast<DiscreteNode*>(par_ptr)->num_potential_vals; i++) {
      ele.first = par_ptr->node_index;
      ele.second = dynamic_cast<DiscreteNode*>(par_ptr)->potential_vals[i];
      cb.insert(ele);
    }
    set_of_sets.insert(cb);
  }

  // Generate
  set_discrete_parents_combinations = GenAllCombinationsFromSets(&set_of_sets);

}

void Node::ClearParents() {
  set_parents_ptrs.clear();
  set_discrete_parents_combinations.clear();
}