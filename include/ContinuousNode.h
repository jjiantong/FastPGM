//
// Created by LinjianLi on 2019/2/9.
//

#ifndef BAYESIANNETWORK_CONTINUOUSNODE_H
#define BAYESIANNETWORK_CONTINUOUSNODE_H


#include <set>
#include <map>
#include <utility>
#include <string>
#include "Node.h"
#include "gadget.h"

using namespace std;

typedef set< pair<int, int> > Combination;

class ContinuousNode : public Node {
 public:
  bool is_discrete = false;
  double mu;  // Unconditional mean.
  double *b;  // Linear coefficients.
  double v;   // Conditional variance.
};


#endif //BAYESIANNETWORK_CONTINUOUSNODE_H
