//
// Created by LinjianLi on 2019/2/16.
//

#ifndef BAYESIANNETWORK_CLIQUE_H
#define BAYESIANNETWORK_CLIQUE_H

#include <set>
#include <map>
#include <utility>
#include <string>
#include "gadget.h"
#include "Node.h"
#include "Factor.h"

typedef set< pair<int, int> > Combination;

class Clique {
 public:

  bool is_separator = false;

  int clique_size;
  set<int> related_variables;
  set<Combination> set_combinations;
  map<Combination, double> map_potentials;
  set<Clique*> set_neighbours_ptr;

  // In junction tree algorithm,
  // the "Collect" force messages to flow from downstream to upstream,
  // and the "Distribute" force messages flow from upstream to downstream.
  // So, we need a member to record the upstream of this clique (node).
  Clique *ptr_upstream_clique;

  Clique(set<Node*>);
  void InitializeClique(set<Node*>);
  Factor Collect();
  void Distribute();
  void Distribute(Factor&);
  void SumOutExternalVars(Factor&);
  void MultiplyWithFactorSumOverExternalVars(Factor&);
  virtual void UpdateUseMessage(Factor&);
  virtual Factor ConstructMessage();
  void PrintPotentials();

 protected:
  Clique() = default;
};


#endif //BAYESIANNETWORK_CLIQUE_H
