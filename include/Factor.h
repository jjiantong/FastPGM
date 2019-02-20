//
// Created by Linjian Li on 2018/11/29.
//

#ifndef BAYESIANNETWORK_FACTOR_H
#define BAYESIANNETWORK_FACTOR_H

#include <set>
#include <map>
#include <utility>
#include <string>
#include <algorithm>

#include "gadget.h"
#include "Node.h"

using namespace std;

typedef set< pair<int, int> > Combination;

class Factor {
 public:
  set<int> related_variables;
  set<Combination> set_combinations;
  map<Combination, double> map_potentials;

  Factor();
  void ConstructFactor(Node *);
  Factor MultiplyWithFactor(Factor);
  Factor SumProductOverVariable(Node *);
  void Normalize();
};


#endif //BAYESIANNETWORK_FACTOR_H
