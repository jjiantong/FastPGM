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
//#include<bits/stdc++.h>
#include <algorithm>
#include "common.h"

using namespace std;


class Node {

 protected:

  // Indexes for all nodes in a network should be consecutive integers starting at 0.
  int node_index = -1;//the ID of this node in Bayesian network

  void AddDiscreteParent(Node *p);
  void AddContinuousParent(Node *p);

 public:

  string node_name = "";
  bool is_discrete = true;

  //DiscreteConfig: set< pair<int, int> >, [variable id, variable value]
  set<DiscreteConfig> set_discrete_parents_combinations = set<DiscreteConfig>{ DiscreteConfig{} };  // Default: contain ONE empty element

  // =============== refactor like Weka ===============
  /** Important: both vec_disc_parent_indexes and set_parent_indexes are needed in the current version **/
  vector<int> vec_disc_parent_indexes;  // The order matters; the order in this vector is not necessarily based on indexes,
                                        // and can use other ordering.
  set<int> set_parent_indexes; // note: this set contains both discrete parents and continuous parents
  set<int> set_children_indexes;

  // ==================================================

  Node() = default;
  explicit Node(int index);
  Node(int index, string name);

  int GetNodeIndex() const;
  void SetNodeIndex(int index);
  bool HasParents() const;
  virtual void AddChild(Node *node_ptr);
  virtual void AddParent(Node *node_ptr);
  bool IsChildOfThisNode(Node *node_ptr);
  bool IsParentOfThisNode(Node *node_ptr);
  void RemoveChild(Node *node_ptr);
  virtual void RemoveParent(Node *node_ptr);
};


#endif //BAYESIANNETWORK_NODE_H
