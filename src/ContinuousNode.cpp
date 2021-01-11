//
// Created by LinjianLi on 2019/2/9.
//

#include"ContinuousNode.h"
#include "DiscreteNode.h"

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

/**
 * @brief: include both the parameters of discrete and continuous parents
 * refer to https://stats.stackexchange.com/questions/353479/gaussian-bayesian-networks-and-covariance-calculation
 */
int ContinuousNode::GetNumParams() {
  int scale = GetNumParentsConfig();
  /** a discrete configuration determines the coefficients of all the continuous variables + mu + variance;
   *  this is a common way of modelling the continuous Gaussian Bayesian Networks**/

  int num_params_for_a_config = 0;
  num_params_for_a_config += 2;  // For mu and variance, as the continuous variable follows Gaussian distribution
  num_params_for_a_config += contin_par_indexes.size();  // For coefficients.
  return num_params_for_a_config * scale;
}


void ContinuousNode::ClearParams() {
  //TODO: incomplete implementation
  fprintf(stderr, "Function [%s] not implemented yet!", __FUNCTION__);
  exit(1);
}

/**
 * @brief: identify continuous parents
 */
void ContinuousNode::IdentifyContPar() {
  contin_par_indexes.clear();
  set<int> set_disc_par(vec_disc_parent_indexes.begin(), vec_disc_parent_indexes.end());

  //use set difference: all_parents - discrete_parents = continuous_parents
  set_difference(set_parent_indexes.begin(), set_parent_indexes.end(),
                 set_disc_par.begin(), set_disc_par.end(),
                 inserter(contin_par_indexes, contin_par_indexes.begin()));
}
