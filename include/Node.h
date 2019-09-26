//
// Created by Linjian Li on 2018/11/29.
//

#ifndef BAYESIANNETWORK_NODE_H
#define BAYESIANNETWORK_NODE_H

#include <set>
#include <map>
#include <utility>
#include <string>
#include <random>
#include <chrono>
#include "gadget.h"

using namespace std;


class Node {

 protected:

  // Indexes for all nodes in a network should start at 0.
  int node_index;

 public:

  string node_name;
  bool is_discrete;

  set<Node*> set_parents_ptrs;
  set<Node*> set_children_ptrs;
  set<DiscreteConfig> set_discrete_parents_combinations;

  Node() = default;
  explicit Node(int index);
  Node(int index, string name);
  int GetNodeIndex() const;
  void SetNodeIndex(int index);
  virtual void AddChild(Node *node_ptr);
  virtual void AddParent(Node *node_ptr);
  void RemoveChild(Node *node_ptr);
  virtual void RemoveParent(Node *node_ptr);
  void GenDiscParCombs();
  virtual void ClearParams() = 0;
};


#endif //BAYESIANNETWORK_NODE_H
