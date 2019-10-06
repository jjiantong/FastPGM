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
#include "DiscreteNode.h"

using namespace std;

class Factor {
 public:
  set<int> related_variables;
  set<DiscreteConfig> set_combinations;
  map<DiscreteConfig, double> map_potentials;

  Factor() = default;
  explicit Factor(DiscreteNode*);
  void CopyFactor(Factor F);
  void SetMembers(set<int> , set<DiscreteConfig> , map<DiscreteConfig, double> );
  void ConstructFactor(DiscreteNode*);
  Factor MultiplyWithFactor(Factor);
  Factor SumOverVar(DiscreteNode *);
  Factor SumOverVar(int);
  void Normalize();

  void PrintPotentials() const;
};


#endif //BAYESIANNETWORK_FACTOR_H
