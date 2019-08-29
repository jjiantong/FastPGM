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

  // In conditional Gaussian Bayesian network,
  // discrete nodes will not have continuous parents

  // The ordering of continuous parents is important.
  vector<int> contin_par_indexes;

  // Conditional mean given discrete parents.
  map<Combination, double> mu;

  // Conditional linear coefficients for continuous parents given discrete parents.
  map<Combination, vector<double>> b;

  // Conditional variance given discrete parents.
  map<Combination, double> v;

  void AddChild(Node *node_ptr) override;
  void AddParent(Node *node_ptr) override;
};


#endif //BAYESIANNETWORK_CONTINUOUSNODE_H
