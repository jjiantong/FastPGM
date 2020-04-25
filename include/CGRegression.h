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

typedef set< pair<int, int> > DiscreteConfig;

/**
 * This class can enable Bayesian network for regression problems.
 * This class is similar to continuous node (e.g., has mu and coefficient) and Factor where the content of CGRegression is gradually simplified.
 * The implementation is based "Local Propagation in Conditional Gaussian Bayesian Networks" by Cowell in 2005.
 * http://jmlr.csail.mit.edu/papers/volume6/cowell05a/cowell05a.pdf
 */
class CGRegression {
  // todo: test the whole class
  // Data structures needed for elimination tree
  // (like junction tree) of gaussian Bayesian network.
  // Proposed in [].
  // This class is used as the element of the lp-potential and postbag.

 public:

  // todo: deal with the case that there are no discrete tail

  /**
   * The conditioned variable is called head, and the conditioning variable is called tail.
   * For example, in L(A|B,C), A is the head while B and C are tails.
   */
  int head_var_index;
  set<int> set_all_tail_index;
  set<DiscreteConfig> set_discrete_tails_combinations;

  // The ordering of continuous tails is important.
  vector<int> vec_contin_tail_indexes;//

  // Conditional mean given discrete tails.
  map<DiscreteConfig, double> map_mu;
  double marginal_mu;

  // Conditional linear coefficients for continuous tails given discrete tails.
  map<DiscreteConfig, vector<double>> map_coefficients;
  vector<double> marginal_coefficients;

  //TODO: double check whether "variance" is standard deviation.
  // Conditional variance given discrete tails.
  map<DiscreteConfig, double> map_variance;
  double marginal_variance;

  CGRegression(Node *node_ptr, set<Node*> set_parent_ptrs);

  void Substitute(pair<int, double> var_value);
  static void Exchange(CGRegression &Z, CGRegression &Y);
  string GetExpression();

 protected:
  static void PrepareForExchange(CGRegression &Z, CGRegression &Y);
  static void ActuallyExchange(CGRegression &Z, CGRegression &Y);
};


#endif //BAYESIANNETWORK_CGREGRESSION_H
