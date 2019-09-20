//
// Created by LinjianLi on 2019/2/9.
//

#ifndef BAYESIANNETWORK_DISCRETENODE_H
#define BAYESIANNETWORK_DISCRETENODE_H


#include <set>
#include <map>
#include <utility>
#include <string>
#include "Node.h"
#include "gadget.h"

using namespace std;

class DiscreteNode : public Node {
 public:
  map<int, map<DiscreteConfig, double> >  map_cond_prob_table;
  map<int, double>  map_marg_prob_table;

  DiscreteNode();
  explicit DiscreteNode(int index);
  DiscreteNode(int index, string name);
  void SetDomain(vector<string> str_domain);
  void SetDomain(vector<int> int_domain);
  void AddParent(Node *node_ptr) override;
  void ClearParams() override;
  void PrintProbabilityTable();
  int SampleNodeGivenParents(DiscreteConfig evidence);
};


#endif //BAYESIANNETWORK_DISCRETENODE_H
