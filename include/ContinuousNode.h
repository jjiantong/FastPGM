//
// Created by LinjianLi on 2019/2/9.
//

#ifndef BAYESIANNETWORK_CONTINUOUSNODE_H
#define BAYESIANNETWORK_CONTINUOUSNODE_H


#include <set>
#include <map>
#include <vector>
#include <utility>
#include <string>
#include "Node.h"
#include "gadget.h"

using namespace std;

typedef set< pair<int, int> > Combination;

class ContinuousNode : public Node {
 public:
  bool is_discrete = false;

                               // Linear Gaussian Bayesian network
                               //   * All variables are continuous
                               //   * All CPDs are linear Gaussian
  double         mu;           // Unconditional mean.
  vector<int>    par_indexes;  // Parents should have an order, because coefficients are corresponding.
  vector<double> b;            // Linear coefficients for parents.
  double         v;            // Unconditional variance.
};


#endif //BAYESIANNETWORK_CONTINUOUSNODE_H
