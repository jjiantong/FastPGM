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

//typedef set< pair<int, int> > DiscreteConfig; // set of [variable id, variable value]

class Network;  // Forward declaration.

/**
 * @brief: this class contains the weights/potentials of each discrete config;
 * the discrete config does not have parent-child relationships.
 */
class Factor {//this is used only for discrete nodes;
 public:
  set<int> related_variables;//the variables involved in this factor
  set<DiscreteConfig> set_disc_configs;//all the configurations of the related variables
  map<DiscreteConfig, double> map_potentials;//the weight/potential of each discrete config

  Factor() = default;
  Factor(DiscreteNode *disc_node, Network *net);
  Factor(set<int> &rv, set<DiscreteConfig> &sc, map<DiscreteConfig, double> &mp);

  Factor MultiplyWithFactor(Factor second_factor);
  Factor SumOverVar(DiscreteNode *);
  Factor SumOverVar(int);
  void FactorReduction(DiscreteConfig evidence);
  void Normalize();

  void PrintPotentials() const;
};


#endif //BAYESIANNETWORK_FACTOR_H
