//
// Created by Linjian Li on 2018/11/29.
//

#ifndef BAYESIANNETWORK_NETWORK_H
#define BAYESIANNETWORK_NETWORK_H

#include "Trainer.h"
#include "Node.h"
#include "Factor.h"
#include "gadget.h"
#include <set>
#include <queue>
#include <vector>
#include <cmath>
#include <iostream>
#include <random>
#include <chrono>

using namespace std;

typedef set< pair<int, int> > Combination;


class Network {
 public:
  string network_name;
  int num_nodes;
  set<Node*> set_node_ptr_container;

  int *default_elim_ord;
  vector<int> topo_ord;

  Network() = default;

  void PrintNetworkStruct();

  Node* FindNodePtrByIndex(int);

  Node* FindNodePtrByName(string);

  virtual void StructLearnCompData(Trainer *) = 0;

  void LearnParmsKnowStructCompData(const Trainer *);

  void SetParentChild(int, int);
  void SetParentChild(Node *, Node *);

  void RemoveParentChild(int, int);
  void RemoveParentChild(Node *, Node *);

  vector<int> GenTopoOrd();

  virtual pair<int*, int> SimplifyDefaultElimOrd(Combination) = 0;

  Combination ConstructEvidence(int *, int *, int);

  vector<Factor> ConstructFactors(int *, int, Node *);
  void LoadEvidence(vector<Factor> *, Combination);

  Factor SumProductVarElim(vector<Factor>, int *, int);
  Factor VarElimInferReturnPossib(int *elim_ord, int num_elim_ord, Combination evidence, Node *target);
  Factor VarElimInferReturnPossib(Combination evidence, Node *target);

  int PredictUseVarElimInfer(int *, int, Combination, int);
  int PredictUseVarElimInfer(Combination, int);

  double TestNetReturnAccuracy(Trainer *);
  double TestNetByApproxInferReturnAccuracy(Trainer *, int);


  // Probabilistic logic sampling is a method
  // proposed by Max Henrion at 1988.
  Combination ProbLogicSampleNetwork();

  set<int> GetMarkovBlanketIndexesOfNode(Node *node_ptr);
  int SampleNodeGivenMarkovBlanketReturnValIndex(Node *node_ptr, Combination markov_blanket);

  vector<Combination> DrawSamplesByProbLogiSamp(int num_samp);
  vector<Combination> DrawSamplesByGibbsSamp(int num_samp, int num_burn_in);

  int ApproxInferByProbLogiRejectSamp(Combination e, Node *node, vector<Combination> &samples);
  int ApproxInferByProbLogiRejectSamp(Combination e, int node_index, vector<Combination> &samples);
};


#endif //BAYESIANNETWORK_NETWORK_H
