//
// Created by LinjianLi on 2019/2/9.
//

#ifndef BAYESIANNETWORK_DISCRETENODE_H
#define BAYESIANNETWORK_DISCRETENODE_H


#include <set>
#include <map>
#include <utility>
#include <string>
#include "gadget.h"

using namespace std;

typedef set< pair<int, int> > Combination;

class DiscreteNode : public Node {
 public:
  bool is_discrete = true;
  int num_potential_vals;
  int* potential_vals;
  set<Combination> set_parents_combinations;
  map<int, map<Combination, double> >  map_cond_prob_table;
  map<int, double>  map_marg_prob_table;

  void GenParCombs();
};


#endif //BAYESIANNETWORK_DISCRETENODE_H
