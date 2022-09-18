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
#include<bits/stdc++.h>
#include "common.h"

using namespace std;


class Node {

 protected:

  // Indexes for all nodes in a network should be consecutive integers starting at 0.
  int node_index = -1;//the ID of this node in Bayesian network
  //TODO: "num_parents_config" is initialized in "GetNumParentsConfig"
  //TODO: but it is not updated anymore (maybe in "AddParent", "AddDiscreteParent", "RemoveParent", "ClearParents")
  int num_parents_config = -1;//number of parent configurations; this parameter is for discrete node only.

  void AddDiscreteParent(Node *p);
  void AddContinuousParent(Node *p);

 public:

  string node_name = "";
  bool is_discrete = true;

  //DiscreteConfig: set< pair<int, int> >, [variable id, variable value]
  //TODO: this member variable may be moved to DiscreteNode class. -> disc par config
  set<DiscreteConfig> set_discrete_parents_combinations = set<DiscreteConfig>{ DiscreteConfig{} };  // Default: contain ONE empty element

  // =============== refactor like Weka ===============
  /** Important: both vec_disc_parent_indexes and set_parent_indexes are needed in the current version **/
  //TODO: why. check the usage. move to DiscreteNode?
  vector<int> vec_disc_parent_indexes;  // The order matters; the order in this vector is not necessarily based on indexes,
                                        // and can use other ordering.
  set<int> set_parent_indexes;//for the ease of parent lookup; this set contains both discrete parents and continuous parents
  map<int, int> map_disc_parents_domain_size;  //Key: parent index. Value: parent's domain size,
                                               // i.e., # of possible values for the parent variable.

  set<int> set_children_indexes;//for the ease of child lookup

  int GetNumParentsConfig(); // get # of parent configs by computing using map_disc_parents_domain_size
  DiscreteConfig GetDiscParConfigGivenAllVarValue(DiscreteConfig &all_var_val);
  DiscreteConfig GetDiscParConfigGivenAllVarValue(vector<int> &all_var_val);
  // ==================================================

  Node() = default;
  explicit Node(int index);
  Node(int index, string name);

  int GetNodeIndex() const;
  void SetNodeIndex(int index);
  bool HasParents() const;
  int GetNumParents() const;
  int GetNumDiscParents() const;
  int GetNumChildren() const;
  virtual void AddChild(Node *node_ptr);
  virtual void AddParent(Node *node_ptr);
  bool IsChildOfThisNode(Node *node_ptr);
  bool IsParentOfThisNode(Node *node_ptr);
  void RemoveChild(Node *node_ptr);
  virtual void RemoveParent(Node *node_ptr);
  void GenDiscParCombs(set<Node*> set_parent_ptrs);
  void ClearParents();
  void ClearChildren();
  virtual int GetNumParams() = 0;
//  virtual void ClearParams() = 0;
};


#endif //BAYESIANNETWORK_NODE_H
