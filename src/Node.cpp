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

/**
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
 * This function is similar to a filter - filters out the variables and values which are not the parents of the current node.
 */
DiscreteConfig Node::GetDiscParConfigGivenAllVarValue(DiscreteConfig &all_var_val) {
  DiscreteConfig par_var_val;

  if (!HasParents()) {
    return par_var_val;  // Return an empty config.
  }

  assert(all_var_val.size() >= GetNumDiscParents());

  for (auto const var_val : all_var_val) {
    auto node_id = var_val.first; // get the index
    // inefficiency: search (from parents) once for each node received by the argument "all_var_val"
    if (std::find(vec_disc_parent_indexes.begin(), vec_disc_parent_indexes.end(), node_id) != vec_disc_parent_indexes.end()) {
      // this node is one of the parent of the current node
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

  for (auto const par : vec_disc_parent_indexes) { // for all parents(index)
    int parent_value = all_var_val.at(par); // get the value according to the index
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
 * @brief: add a discrete child node c
 * update:  "set_children_indexes"
 */
void Node::AddChild(Node *c) {
  //TODO: Reduce redundancy: combine this function with "AddChild" in ContinuousNode
  int c_idx = c->GetNodeIndex();
  if (set_children_indexes.find(c_idx) == set_children_indexes.end()) {
    // c is not a child of the current node
    set_children_indexes.insert(c_idx);
  }
}

///**
// * @brief: add child; can be merged with AddChild in the base class.
// */
//void ContinuousNode::AddChild(Node *c) {
//    if (c->is_discrete) {
//        fprintf(stderr, "Error in function %s! \n"
//                        "Continuous node must not have discrete child!", __FUNCTION__);
//        exit(1);
//    }
//    int c_idx = c->GetNodeIndex();
//    if (set_children_indexes.find(c_idx) == set_children_indexes.end()) {
//        set_children_indexes.insert(c_idx);
//    } else {
//        fprintf(stdout, "Node #%d is already child of Node #%d", c_idx, this->GetNodeIndex());
//    }
//}

/**
 * @brief: add a generic parent node p
 */
void Node::AddParent(Node *p) {
  int p_idx = p->GetNodeIndex();
  if (set_parent_indexes.find(p_idx) == set_parent_indexes.end()) {
    // p is not in the parent set.
    if (p->is_discrete) {
      AddDiscreteParent(p);
    } else {
      AddContinuousParent(p);
    }
  }
}

/**
 * @brief: add a parent p to the node
 * update:  "set_parent_indexes",
 *          "vec_disc_parent_indexes",
 *          "map_disc_parents_domain_size",
 *          "set_discrete_parents_combinations"
 */
void Node::AddDiscreteParent(Node *p) {
  int p_idx = p->GetNodeIndex();//get the id of the parent p
  if (set_parent_indexes.find(p_idx) == set_parent_indexes.end()) {
    // If p is not in the parent set.
    // update "set_parent_indexes", "vec_disc_parent_indexes", "map_disc_parents_domain_size"
    set_parent_indexes.insert(p_idx);
    auto dp = (DiscreteNode *) p;//dp stands for "discrete parent"
    vec_disc_parent_indexes.push_back(p_idx);
    map_disc_parents_domain_size[p_idx] = dp->GetDomainSize();

    // Update possible parent configurations
    set<DiscreteConfig> new_par_combs;
    for (const auto &val : dp->vec_potential_vals) {
      DiscVarVal vv(p_idx, val); // get [idx of p, each value of p]
      for (auto old_par_comb : set_discrete_parents_combinations) {
        // set_discrete_parents_combinations: set< set< pair<int, int> > >
        // new_par_combs: set< set< pair<int, int> > >
        // old_par_comb: set< pair<int, int> >
        // vv: pair<int, int>
        // add each vv(corresponding to each potential value of p) to each old_par_comb
        old_par_comb.insert(vv);//insert the new parent with a potential value to the old configuration
        new_par_combs.insert(old_par_comb);
      } // finish adding one vv to all old_par_comb
    } // finish adding all potential vv to all old_par_comb
    this->set_discrete_parents_combinations = new_par_combs;
  }
  //if the parent is already in the set_parent_indexes, nothing needs to be done.
}

/**
 * @brief: similar to AddDiscreteParent
 * update:  "set_parent_indexes",
 *          "contin_par_indexes"
 */
void Node::AddContinuousParent(Node *p) {
  int p_idx = p->GetNodeIndex();
  if (set_parent_indexes.find(p_idx) == set_parent_indexes.end()) {  // If p is not in the parent set.
    // update "set_parent_indexes", "contin_par_indexes"
    set_parent_indexes.insert(p_idx);
    // TODO: this
    auto cont_this = (ContinuousNode*) this;
    cont_this->contin_par_indexes.push_back(p_idx);
  }
}

bool Node::IsChildOfThisNode(Node *node_ptr) {
    int node_idx = node_ptr->GetNodeIndex();
    if (set_children_indexes.find(node_idx) == set_children_indexes.end()) {
        return false;
    } else {
        return true;
    }
}

bool Node::IsParentOfThisNode(Node *node_ptr) {
    int node_idx = node_ptr->GetNodeIndex();
    if (set_parent_indexes.find(node_idx) == set_parent_indexes.end()) {
        return false;
    } else {
        return true;
    }
}

/**
 * @brief: remove a child node c; this function is used for network structure learning.
 * @attention: this function needs to use with RemoveParent
 * update:  "set_children_indexes"
 */
void Node::RemoveChild(Node *c) {
  if (!IsChildOfThisNode(c)) {
    fprintf(stderr, "Node #%d does not have parent node #%d!", this->GetNodeIndex(), c->GetNodeIndex());
    return;
  }
  int c_idx = c->GetNodeIndex();
  set_children_indexes.erase(c_idx);
}

/**
 * @brief: remove a parent node p
 * @attention: this function needs to use with RemoveChild
 * update:  "set_parent_indexes",
 *          "vec_disc_parent_indexes",
 *          "map_disc_parents_domain_size",
 *          "set_discrete_parents_combinations"
 *          "contin_par_indexes"
 */
void Node::RemoveParent(Node *p) {
    //TODO: combine with "RemoveParent" in ContinuousNode.cpp??
  if (!IsParentOfThisNode(p)) {
    fprintf(stderr, "Node #%d does not have parent node #%d!", this->GetNodeIndex(), p->GetNodeIndex());
    return;
  }

  int p_idx = p->GetNodeIndex();
  set_parent_indexes.erase(p_idx);

  if (p->is_discrete) {
    vec_disc_parent_indexes.erase(std::find(vec_disc_parent_indexes.begin(), vec_disc_parent_indexes.end(), p_idx));
    map_disc_parents_domain_size.erase(p_idx);

    // Update possible parent configurations
    auto dp = (DiscreteNode*) p;
    set<DiscreteConfig> new_par_combs;
    //TODO: potential bug here; may need to switch the two loops
    //TODO: erase each vv(corresponding to each potential value of p) to each old_par_comb is wrong!
    //TODO: because each old_par_comb contains only one vv
    //TODO: and this will finally insert # of potential values times more combs to new_par_combs
    //TODO: just erase one config corresponding to p_idx for each old_par_comb will also insert more combs
    //TODO: maybe choose combs from set_discrete_parents_combinations that have one specific [p_idx, one of the value]
    //TODO: then erase [p_idx, one of the value] and insert to new_par_combs
    for (const auto &val : dp->vec_potential_vals) {
      DiscVarVal vv(p_idx, val); // get [idx of p, each value of p]
      for (auto old_par_comb : set_discrete_parents_combinations) {
        old_par_comb.erase(vv);
        new_par_combs.insert(old_par_comb);
      }
    }
    this->set_discrete_parents_combinations = new_par_combs;

//    // comparison: add parent:
//    // Update possible parent configurations
//    set<DiscreteConfig> new_par_combs;
//    for (const auto &val : dp->vec_potential_vals) {
//      DiscVarVal vv(p_idx, val); // get [idx of p, each value of p]
//      for (auto old_par_comb : set_discrete_parents_combinations) {
//        // set_discrete_parents_combinations: set< set< pair<int, int> > >
//        // new_par_combs: set< set< pair<int, int> > >
//        // old_par_comb: set< pair<int, int> >
//        // vv: pair<int, int>
//        // add each vv(corresponding to each potential value of p) to each old_par_comb
//        old_par_comb.insert(vv);//insert the new parent with a potential value to the old configuration
//        new_par_combs.insert(old_par_comb);
//      } // finish adding one vv to all old_par_comb
//    } // finish adding all potential vv to all old_par_comb
//    this->set_discrete_parents_combinations = new_par_combs;
  }
}

///**
// * @brief: remove parent; possibly only used in structure learning
// */
//void ContinuousNode::RemoveParent(Node *p) {
//    Node::RemoveParent(p);
//
//    if (!p->is_discrete) {
//        //remove the parent from the vector based on parent id
//        auto it = contin_par_indexes.begin();
//        while (*it!=p->GetNodeIndex()) { ++it; }
//        contin_par_indexes.erase(it);
//    }
//}

/**
 * @brief: generate all the possible parent configurations ("set_discrete_parents_combinations")
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
    if (!par_ptr->is_discrete) { //no parent configuration for continuous nodes
      continue;
    }

    DiscreteConfig all_config_of_a_parent;
    pair<int, int> varId_val;
    DiscreteNode *d_par_ptr = (DiscreteNode*)(par_ptr);//convert a generic node to a discrete node

    for (int i = 0; i < d_par_ptr->GetDomainSize(); ++i) {
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
 * clear:   "set_parent_indexes",
 *          "vec_disc_parent_indexes",
 *          "map_disc_parents_domain_size",
 *          "set_discrete_parents_combinations"
 */
void Node::ClearParents() {
  //TODO: double check whether need to set "num_parents_config"
  //TODO: this function clear discrete parents but not continuous parents
  set_discrete_parents_combinations.clear();

  vec_disc_parent_indexes.clear();
  set_parent_indexes.clear();
  map_disc_parents_domain_size.clear();
}

void Node::ClearChildren() {
  set_children_indexes.clear();
}