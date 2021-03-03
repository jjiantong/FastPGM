//
// Created by Linjian Li on 2018/11/29.
//

#include "Factor.h"
#include "Network.h"


/**
 * @brief: construct a factor given a node;
 * the factor consists of the node and all the existing related_variables, i.e., all its parents.
 */
Factor::Factor(DiscreteNode *disc_node, Network *net) {
  int node_index = disc_node->GetNodeIndex();

  related_variables.insert(node_index);//related_variables is empty initially, because this is a constructor.

  //construct the configuration of this node
  set<pair<int,int>> set_pair_temp;
  for (int i = 0; i < disc_node->GetDomainSize(); ++i) {
    pair<int, int> pair_temp;
    pair_temp.first = node_index;
    pair_temp.second = disc_node->vec_potential_vals.at(i);
    set_pair_temp.insert(pair_temp);
  }

  // If this disc_node has no parents.
  if (!disc_node->HasParents()) {
    for (auto &p : set_pair_temp) {
      DiscreteConfig c, empty_par_config;
      c.insert(p);
      set_disc_config.insert(c);
      map_potentials[c] = disc_node->GetProbability(p.second, empty_par_config);
    }
  }
  else {// If this disc_node has parents, the outer loop is for the disc_node, and the inner loop is for the parents.
    related_variables.insert(disc_node->set_parent_indexes.begin(), disc_node->set_parent_indexes.end());
    for (auto &p : set_pair_temp) { // for each possible value of this node
      for (const auto & comb : disc_node->set_discrete_parents_combinations) { // for each parent configuration
        DiscreteConfig c = comb;
        auto c_old = c;//an existing (parent) configuration
        c.insert(p);//a new configuration with the value of current node
        set_disc_config.insert(c);
//        DiscreteConfig par_config = disc_node->GetDiscParConfigGivenAllVarValue(c_old);
        map_potentials[c] = disc_node->GetProbability(p.second, c_old);//(AB, 00)
      }
    }
  }//end has parents
}

/**
 * @brief: constructor
 */
Factor::Factor(set<int> &rv,
               set<DiscreteConfig> &sc,
               map<DiscreteConfig, double> &mp) {
  this->related_variables = rv;
  this->set_disc_config = sc;
  this->map_potentials = mp;
}

/**
 * @brief: cartesian product on two factors (product of factors)
 * if two factors have shared variables, the conflict ones (i.e. one variable has more than one value) in the results need to be removed.
 * if "related_variables" of one of the factors is empty, then directly return the other factor without multiplication
 * because the case means that this factor is a constant; since we re-normalize at the end, the constant will not affect the result
 */
Factor Factor::MultiplyWithFactor(Factor second_factor) {
  Factor newFactor;

  if (this->related_variables.empty()) {
    return second_factor;
  }
  if (second_factor.related_variables.empty()) {
    return *this;
  }

  newFactor.related_variables.insert(this->related_variables.begin(),this->related_variables.end());
  newFactor.related_variables.insert(second_factor.related_variables.begin(),second_factor.related_variables.end());

  for (auto first: set_disc_config) {
    for (auto second : second_factor.set_disc_config) {
      // If two combinations have different values on common variables,
      // which means that they conflict, then these two combinations can not form a legal entry.
      if (Conflict(&first, &second)) {
        continue;
      }
      DiscreteConfig new_comb;
      new_comb.insert(first.begin(),first.end());
      new_comb.insert(second.begin(),second.end());
      newFactor.set_disc_config.insert(new_comb);
      newFactor.map_potentials[new_comb] = this->map_potentials[first] * second_factor.map_potentials[second];
    }
  }
  return newFactor;
}

/**
 * @brief: factor out a node by id; i.e., factor marginalization
 * eliminate variable "id" by summation of the factor over "id"
 */
Factor Factor::SumOverVar(int index) {
  Factor newFactor;

  this->related_variables.erase(index);
  newFactor.related_variables = this->related_variables;//new set of related variables

  for (auto config : set_disc_config) { // check each of the configurations
    //check if this configuration needs to be sum over
    pair<int, int> pair_to_be_erased;
    for (auto p : config) {
      if (p.first == index) { //this configuration contains the value of the variable which needs to be sum over.
        pair_to_be_erased = p;
        break;
      }
    }

    // probability/potential of this config, which will be added in order to marginalize a variable.
    double temp = this->map_potentials[config];
    config.erase(pair_to_be_erased);

    // update potential for new factor; similar to marginalise a variable.
    // this (result) config is existed: add the new probability/potential
    if (newFactor.set_disc_config.find(config) != newFactor.set_disc_config.end()) {
      newFactor.map_potentials[config] += temp;
    }
    // cannot find this config: insert and initialize
    else {
      newFactor.set_disc_config.insert(config);
      newFactor.map_potentials[config] = temp;
    }
  }
  return newFactor;
}

/**
 * @brief: factor out a node by node; i.e., factor marginalization
 * eliminate variable "node" by summation of the factor over "node"
 */
Factor Factor::SumOverVar(DiscreteNode *node) {
  return SumOverVar(node->GetNodeIndex());
}

/*!
 * @brief: factor reduction given evidence
 * @example:    a0 b0 c0    0.3             a0 b0 c0    0.3
 *              a0 b0 c1    0.7             a0 b0 c1    0.7
 *              a0 b1 c0    0.4     -->     a0 b1 c0    0.4
 *              a0 b1 c1    0.6             a0 b1 c1    0.6
 *              a1 b0 c0    0.1
 *              a1 b0 c1    0.9         (if we get the evidence that a = 0,
 *              a1 b1 c0    0.2         the line that conflict with this evidence will be removed)
 *              a1 b1 c1    0.8
 * in the example, the scope of the reduced factor actually becomes to be {b, c},
 */
Factor Factor::FactorReduction(DiscreteConfig evidence) { //set< pair<int, int> >
  Factor newFactor(related_variables, set_disc_config, map_potentials);

  for (auto &e: evidence) { // for each observation of variable
    // if this factor is related to the observation
    if (newFactor.related_variables.find(e.first) != newFactor.related_variables.end()) {
      newFactor.related_variables.erase(e.first); //TODO
      for (auto &comb: newFactor.set_disc_config) { // for each discrete config of this factor
        // if this config and the evidence have different values on common variables,
        // which means that they conflict, then this config will be removed
        if (comb.find(e) == comb.end()) {
          // TODO: double-check: set to 0 or remove?
//          newFactor.map_potentials[comb] = 0; //TODO
          newFactor.set_disc_config.erase(comb); //TODO
          newFactor.map_potentials.erase(comb); //TODO
        }
      }
    }
  }
  return newFactor;
}


/**
 * @brief: convert weight/potential into probability
 */
void Factor::Normalize() {
  double denominator = 0;

  // compute the denominator for each of the configurations
  for (auto &comb : set_disc_config) {
    denominator += map_potentials[comb];
  }

  // normalize for each of the configurations
  for (auto &comb : set_disc_config) {
    map_potentials[comb] /= denominator;
  }
}


void Factor::PrintPotentials() const {
  for (auto &potentials_key_value : map_potentials) {
    for (auto &vars_index_value : potentials_key_value.first) {
      cout << '(' << vars_index_value.first << ',' << vars_index_value.second << ") ";
    }
    cout << "\t: " << potentials_key_value.second << endl;
  }
  cout << "----------" << endl;
}