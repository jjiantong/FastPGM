//
// Created by LinjianLi on 2019/9/1.
//

#ifndef BAYESIANNETWORK_ELIMINATIONTREE_H
#define BAYESIANNETWORK_ELIMINATIONTREE_H

#include "JunctionTree.h"

class EliminationTree : public JunctionTree {
  // Data structures needed for elimination tree
  // (like junction tree) of gaussian Bayesian network.
  // Proposed in [Local Propagation in Conditional Gaussian Bayesian Networks (Cowell, 2005)].

 public:
  Clique *strong_root;

  // The set of discrete clusters that are neighbours to the
  // continuous part will be called the boundary.
  set<Clique*> boundary;

  map<int, int> map_to_root;

  EliminationTree() = default;
  EliminationTree(Network *net);
  void CalcuEachToRoot();

};


#endif //BAYESIANNETWORK_ELIMINATIONTREE_H
