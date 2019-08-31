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

typedef set< pair<int, int> > Combination;

class DiscreteNode : public Node {
 public:
  DiscreteNode();
  explicit DiscreteNode(int index);
  void AddParent(Node *node_ptr) override;

};


#endif //BAYESIANNETWORK_DISCRETENODE_H
