#include <utility>

//
// Created by Linjian Li on 2018/11/29.
//

#include "Node.h"
#include "DiscreteNode.h"
#include "ContinuousNode.h"

Node::Node(int index) : Node(index, to_string(index)){

}

Node::Node(int index, string name) {
  SetNodeIndex(index);
  node_name = std::move(name);
}

int Node::GetNodeIndex() const {
  return node_index;
}

bool Node::HasParents() const {
  return !set_parent_indexes.empty();
}


int Node::GetNumParents() const {
  return set_parent_indexes.size();
}

int Node::GetNumDiscParents() const {
  return vec_disc_parent_indexes.size();
}

int Node::GetNumChildren() const {
  return set_children_indexes.size();
}

/*
 * @brief: get the total number of parent combinations
 */
int Node::GetNumParentsConfig() {
  if (num_parents_config < 0) {
    num_parents_config = 1;
    for (auto const &par : vec_disc_parent_indexes) {
      // If the node has no parent, this for loop will not be executed.
      // And the number of parents config will be 1.
      num_parents_config *= map_disc_parents_domain_size[par];
    }
  }
  return num_parents_config;
}

/**
 * @brief: similar to GetDiscParConfigGivenAllVarValue(vector<int> &all_var_val).
 * get parents given a set of [variable id, variable value].
 */
DiscreteConfig Node::GetDiscParConfigGivenAllVarValue(DiscreteConfig &all_var_val) {
  DiscreteConfig par_var_val;

  if (!HasParents()) {
    return par_var_val;  // Return an empty config.
  }

  assert(all_var_val.size() >= GetNumDiscParents());

  for (auto const var_val : all_var_val) {
    auto node_id = var_val.first;
    if (std::find(vec_disc_parent_indexes.begin(), vec_disc_parent_indexes.end(), node_id) != vec_disc_parent_indexes.end()) {
      //this node is one of the parent of the current node
      par_var_val.insert(var_val);
    }
  }
  return par_var_val;
}

/**
 * @brief: get the parent configuration (i.e., condition) of current node;
 * @param: all_var_val contains the values of each node of the Bayesian network
 */
DiscreteConfig Node::GetDiscParConfigGivenAllVarValue(vector<int> &all_var_val) {
  // This version is 2x FASTER than the version that accept DiscreteConfig as argument.
  DiscreteConfig par_var_val;

  if (!HasParents()) {
    return par_var_val;  // Return an empty config.
  }

  assert(all_var_val.size() >= GetNumDiscParents());

  for (auto const par : vec_disc_parent_indexes) {
    int parent_value = all_var_val.at(par);
    par_var_val.insert(DiscVarVal(par, parent_value));
  }
  return par_var_val;
}

void Node::SetNodeIndex(int i) {
  if (i<0) {
    fprintf(stderr, "Error in function %s! \nInvalid index!", __FUNCTION__);
    exit(1);
  }
  node_index = i;
}

/**
 * @brief: add a discrete child node
 */
void Node::AddChild(Node *c) {
  //Reduce redundancy: combine this function with "AddChild" in ContinuousNode
  int c_idx = c->GetNodeIndex();
  if (set_children_indexes.find(c_idx) == set_children_indexes.end()) {
    set_children_indexes.insert(c_idx);
  }
}

/**
 * @brief: add a generic parent node
 */
void Node::AddParent(Node *p) {
  int p_idx = p->GetNodeIndex();
  if (set_parent_indexes.find(p_idx) == set_parent_indexes.end()) {  // If p is not in the parent set.
    if (p->is_discrete) {
      AddDiscreteParent(p);
    } else {
      AddContinuousParent(p);
    }
  }
}

/**
 * @brief: add a parent to the node
 */
void Node::AddDiscreteParent(Node *p) {
  int p_idx = p->GetNodeIndex();//get the id of the parent
  if (set_parent_indexes.find(p_idx) == set_parent_indexes.end()) {  // If p is not in the parent set.
    set_parent_indexes.insert(p_idx);
    auto dp = (DiscreteNode *) p;//dp stands for "discrete parent"
    vec_disc_parent_indexes.push_back(p_idx);
    map_disc_parents_domain_size[p_idx] = dp->GetDomainSize();

    // Update possible parent configurations
    set<DiscreteConfig> new_par_combs;
    for (const auto &val : dp->vec_potential_vals) {
      DiscVarVal vv(p_idx, val);
      for (auto old_par_comb : set_discrete_parents_combinations) {
        old_par_comb.insert(vv);//insert the new parent with a potential value to the old configuration
        new_par_combs.insert(old_par_comb);
      }
    }
    this->set_discrete_parents_combinations = new_par_combs;
  }
  //if the parent is already in the set_parent_indexes, nothing needs to be done.
}

/**
 * @brief: similar to AddDiscreteParent
 */
void Node::AddContinuousParent(Node *p) {
  int p_idx = p->GetNodeIndex();
  if (set_parent_indexes.find(p_idx) == set_parent_indexes.end()) {  // If p is not in the parent set.
    set_parent_indexes.insert(p_idx);

    auto cont_this = (ContinuousNode*) this;
    cont_this->contin_par_indexes.push_back(p_idx);
  }
}

/**
 * @brief: remove a child node; this function is used for network structure learning.
 */
void Node::RemoveChild(Node *c) {
  /** Important: need to use with RemoveParent **/
  int c_idx = c->GetNodeIndex();
  if (set_children_indexes.find(c_idx) == set_children_indexes.end()) {
    fprintf(stderr, "Node #%d does not have parent node #%d!", this->GetNodeIndex(), c_idx);
    return;
  }
  set_children_indexes.erase(c_idx);
}


void Node::RemoveParent(Node *p) {
  /** Important: need to use with RemoveChild **/
  int p_idx = p->GetNodeIndex();
  if (set_parent_indexes.find(p_idx)==set_parent_indexes.end()) {
    fprintf(stderr, "Node #%d does not have parent node #%d!", this->GetNodeIndex(), p_idx);
    return;
  }
  set_parent_indexes.erase(p_idx);
  if (p->is_discrete) {
    vec_disc_parent_indexes.erase(std::find(vec_disc_parent_indexes.begin(), vec_disc_parent_indexes.end(), p_idx));
    map_disc_parents_domain_size.erase(p_idx);

    // Update possible parent configurations
    auto dp = (DiscreteNode*) p;
    set<DiscreteConfig> new_par_combs;
    //TODO: potential bug here; may need to switch the two loops
    for (const auto &val : dp->vec_potential_vals) {
      DiscVarVal vv(p_idx, val);
      for (auto old_par_comb : set_discrete_parents_combinations) {
        old_par_comb.erase(vv);
        new_par_combs.insert(old_par_comb);
      }
    }
    this->set_discrete_parents_combinations = new_par_combs;
  }
}

/**
 * @brief: generate all the possible parent configurations
 * @param: set_parent_ptrs: the set of parents of the current node.
 */
void Node::GenDiscParCombs(set<Node*> set_parent_ptrs) {
  set_discrete_parents_combinations.clear();

  // If the node has no parent, then it should have ONE empty parent configuration.
  if (set_parent_ptrs.empty()) {
    DiscreteConfig empty_config;
    set_discrete_parents_combinations.insert(empty_config);
    return;
  }

  // Preprocess. Construct set of sets.
  set<DiscreteConfig> all_config_of_each_parent;

  for (auto par_ptr : set_parent_ptrs) {
    if (!par_ptr->is_discrete) { continue; }//not parent configuration for continuous nodes
    DiscreteConfig all_config_of_a_parent;
    pair<int, int> varId_val;
    DiscreteNode *d_par_ptr = (DiscreteNode*)(par_ptr);//convert a generic node to a discrete node

    for (int i=0; i<d_par_ptr->GetDomainSize(); ++i) {
      varId_val.first = par_ptr->node_index;
      varId_val.second = ((DiscreteNode*)par_ptr)->vec_potential_vals.at(i);
      all_config_of_a_parent.insert(varId_val);
    }
    all_config_of_each_parent.insert(all_config_of_a_parent);
  }

  // Generate
  set_discrete_parents_combinations = GenAllCombinationsFromSets(&all_config_of_each_parent);
}

/**
 * @brief: possibly only used in structure learning
 */
void Node::ClearParents() {
  //TODO: double check whether need to set "num_parents_config"
  set_discrete_parents_combinations.clear();

  vec_disc_parent_indexes.clear();
  set_parent_indexes.clear();
  map_disc_parents_domain_size.clear();
}

void Node::ClearChildren() {
  set_children_indexes.clear();
}