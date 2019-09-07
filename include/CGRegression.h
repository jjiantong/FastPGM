//
// Created by LinjianLi on 2019/8/31.
//

#ifndef BAYESIANNETWORK_CGREGRESSION_H
#define BAYESIANNETWORK_CGREGRESSION_H

#include <set>
#include <map>
#include <vector>
#include <utility>
#include <algorithm>
#include "Node.h"
#include "ContinuousNode.h"
#include "gadget.h"

using namespace std;

typedef set< pair<int, int> > Combination;

class CGRegression {
  // todo: test the whole class
  // Data structures needed for elimination tree
  // (like junction tree) of gaussian Bayesian network.
  // Proposed in [Local Propagation in Conditional Gaussian Bayesian Networks (Cowell, 2005)].
  // This class is used as the element of the lp-potential and postbag.

  // The conditioned variable is called head, and the conditioning variable is called tail.
  // For example, in L(A|B,C), A is the head while B and C are tails.

 public:

  // todo: deal with the case that there are no discrete tail

  int head_var_index;
  set<int> set_all_tail_index;
  set<Combination> set_discrete_tails_combinations;

  // The ordering of continuous tails is important.
  vector<int> contin_tail_indexes;

  // Conditional mean given discrete tails.
  map<Combination, double> mu;

  // Conditional linear coefficients for continuous tails given discrete tails.
  map<Combination, vector<double>> coefficients;

  // Conditional variance given discrete tails.
  map<Combination, double> variance;

  CGRegression(Node *node_ptr);

  void Substitute(pair<int, double> var_value);
  static void Exchange(CGRegression &Z, CGRegression &Y);

 protected:
  static void PrepareForExchange(CGRegression &Z, CGRegression &Y);
  static void ActuallyExchange(CGRegression &Z, CGRegression &Y);
};


#endif //BAYESIANNETWORK_CGREGRESSION_H
