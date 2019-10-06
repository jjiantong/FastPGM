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

typedef set< pair<int, int> > DiscreteConfig;

class ContinuousNode : public Node {
 public:

  // In conditional Gaussian Bayesian network,
  // discrete nodes will not have continuous parents

  // The ordering of continuous parents is important.
  vector<int> contin_par_indexes;

  // Conditional mean given discrete parents.
  map<DiscreteConfig, double> map_mu;
  double marginal_mu;

  // Conditional linear coefficients for continuous parents given discrete parents.
  map<DiscreteConfig, vector<double>> map_coefficients;
  vector<double> marginal_coefficients;

  // Conditional variance given discrete parents.
  map<DiscreteConfig, double> map_variance;
  double marginal_variance;

  ContinuousNode();
  explicit ContinuousNode(int index);
  ContinuousNode(int index, string name);
  void AddChild(Node *node_ptr) override;
  void AddParent(Node *node_ptr) override;
  void RemoveParent(Node *node_ptr) override;
  int GetNumParams() const override;
  void ClearParams() override;

 private:
  void IdentifyContPar();
};


#endif //BAYESIANNETWORK_CONTINUOUSNODE_H
