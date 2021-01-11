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

typedef set<pair<int, int> > DiscreteConfig;//to keep track of the discrete parent configuration of this node

//TODO: learn more about continuous variables..
class ContinuousNode : public Node {//the parent nodes of ContinuousNode can be both discrete and continuous variables.
 public:

  // In conditional Gaussian Bayesian network,
  // discrete nodes will not have continuous parents

  //TODO: Understand "The ordering of continuous parents is important."
  vector<int> contin_par_indexes;

  // Conditional mean given discrete parents.
  map<DiscreteConfig, double> map_mu;//each configuration of discrete parents corresponds to a mu.
  double marginal_mu;//TODO: double check how to compute; possibly the average of all the mu of all the configurations

    // Conditional linear coefficients for continuous parents given discrete parents.
  /**
   * each configuration of discrete parents corresponds to a coefficient of a continuous variable.
   * the size of vector<double> is the number of continuous variables
   */
  map<DiscreteConfig, vector<double>> map_coefficients;
  vector<double> marginal_coefficients;//TODO: double check how to compute marginal_coefficient.

  // Conditional variance given discrete parents.
  map<DiscreteConfig, double> map_variance;//similar to "map_mu"
  double marginal_variance;//similar to marginal_variance

  ContinuousNode();
  explicit ContinuousNode(int index);
  ContinuousNode(int index, string name);
  ContinuousNode(const ContinuousNode &n) = default;
  ContinuousNode(ContinuousNode &n) = default;
  void AddChild(Node *node_ptr) override;
  void RemoveParent(Node *node_ptr) override;
  int GetNumParams() override;
  void ClearParams() override;

 private:
  void IdentifyContPar();
};


#endif //BAYESIANNETWORK_CONTINUOUSNODE_H
