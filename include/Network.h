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
#include <sys/time.h>
#include <locale.h>
#include "omp.h"


using namespace std;

typedef set< pair<int, int> > Combination;


class Network {

 public:
  string network_name;
  int num_nodes;
  bool pure_discrete;
  set<Node*> set_node_ptr_container;

  int *default_elim_ord;
  vector<int> topo_ord;

  Network();
  explicit Network(bool pure_disc);
  virtual ~Network() = default;

  void PrintEachNodeParents();
  void PrintEachNodeChildren();

  Node* FindNodePtrByIndex(const int &index) const;

  Node* FindNodePtrByName(const string &name) const;

  virtual void StructLearnCompData(Trainer *, bool print_struct=true);

  virtual void LearnParmsKnowStructCompData(const Trainer *, bool print_params=true);

  void ClearParms();

  void SetParentChild(int, int);
  void SetParentChild(Node *par, Node *chi);

  void RemoveParentChild(int, int);
  void RemoveParentChild(Node *par, Node *chi);

  vector<int> GenTopoOrd();

  virtual pair<int*, int> SimplifyDefaultElimOrd(Combination) = 0;

  Combination ConstructEvidence(int *nodes_indexes, int *observations, int num_of_observations);

  vector<Factor> ConstructFactors(int *Z, int nz, Node *Y);
  void LoadEvidence(vector<Factor> *factors_list, Combination E,  set<int> all_related_vars);

  Factor SumProductVarElim(vector<Factor> factors_list, int *Z, int nz);
  Factor VarElimInferReturnPossib(int *elim_ord, int num_elim_ord, Combination evidence, Node *target);
  Factor VarElimInferReturnPossib(Combination evidence, Node *target);

  int PredictUseVarElimInfer(int *Z, int nz, Combination E, int Y_index);
  int PredictUseVarElimInfer(Combination E, int Y_index);

  double TestNetReturnAccuracy(Trainer *tester);
  double TestNetByApproxInferReturnAccuracy(Trainer *tester, int num_samp);
  double TestAccuracyByLikelihoodWeighting(Trainer *tester, int num_samp);


  // Probabilistic logic sampling is a method
  // proposed by Max Henrion at 1988.
  Combination ProbLogicSampleNetwork();

  pair<Combination, double> DrawOneLikelihoodWeightingSample(const Combination &evidence);
  vector<pair<Combination, double>> DrawSamplesByLikelihoodWeighting(const Combination &evidence, int num_samp);
  Factor CalcuMargWithLikelihoodWeightingSamples(const vector<pair<Combination, double>> &samples, const int &node_index);
  int ApproxinferByLikelihoodWeighting(Combination e, const int &node_index, const int &num_samp);

  set<int> GetMarkovBlanketIndexesOfNode(Node *node_ptr);
  int SampleNodeGivenMarkovBlanketReturnValIndex(Node *node_ptr, Combination markov_blanket);

  vector<Combination> DrawSamplesByProbLogiSamp(int num_samp);
  vector<Combination> DrawSamplesByGibbsSamp(int num_samp, int num_burn_in);

  int ApproxInferByProbLogiRejectSamp(Combination e, Node *node, vector<Combination> &samples);
  int ApproxInferByProbLogiRejectSamp(Combination e, int node_index, vector<Combination> &samples);
};


#endif //BAYESIANNETWORK_NETWORK_H
