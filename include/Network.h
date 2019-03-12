//
// Created by Linjian Li on 2018/11/29.
//

#ifndef BAYESIANNETWORK_NETWORK_H
#define BAYESIANNETWORK_NETWORK_H

#include "Trainer.h"
#include "Node.h"
#include "Edge.h"
#include "Factor.h"
#include "gadget.h"
#include <set>
#include <queue>
#include <vector>
#include <cmath>
#include <iostream>

using namespace std;

typedef set< pair<int, int> > Combination;


class Network {
 public:
  int num_nodes;
  set<Node*> set_node_ptr_container;
  //set<Edge*> edges_container;
  int *default_elim_ord;
  vector<int> topo_ord;

  Network();

  Node* GivenIndexToFindNodePointer(int);

  virtual void StructLearnCompData(Trainer *) = 0;

  void LearnParmsKnowStructCompData(const Trainer *);

  void SetParentChild(int, int);
  void SetParentChild(Node *, Node *);

  void RemoveParentChild(int, int);
  void RemoveParentChild(Node *, Node *);

  virtual pair<int*, int> SimplifyDefaultElimOrd(Combination) = 0;

  Combination ConstructEvidence(int *, int *, int);

  vector<Factor> ConstructFactors(int *, int, Node *);
  void LoadEvidence(vector<Factor> *, Combination);

  Factor SumProductVarElim(vector<Factor>, int *, int);
  Factor VarElimInferReturnPossib(int *, int, Combination, Node *);
  Factor VarElimInferReturnPossib(Combination, Node *);

  int PredictUseVarElimInfer(int *, int, Combination, int);
  int PredictUseVarElimInfer(Combination, int);

  double TestNetReturnAccuracy(Trainer *);

  vector<int> TopoSort();
};


#endif //BAYESIANNETWORK_NETWORK_H
