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
  int clique_size;
  set<int> related_variables;
  set<Combination> set_combinations;
  map<Combination, double> map_potentials;
  set<Clique*> set_neighbours_ptr;

  Clique();
  Clique(set<Node*>);
  map<Combination, double> Collect();
  map<Combination, double> Distribute();

  void MultiplyWithFactor(Factor);

};


#endif //BAYESIANNETWORK_CLIQUE_H
