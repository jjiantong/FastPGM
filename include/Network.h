//
// Created by Linjian Li on 2018/11/29.
//

#ifndef BAYESIANNETWORK_NETWORK_H
#define BAYESIANNETWORK_NETWORK_H

#include "Dataset.h"
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
#include <algorithm>
#include "omp.h"


using namespace std;

// Forward Declaration
class ScoreFunction;


class Network {

 public:
  string network_name;
  int num_nodes = 0;
  bool pure_discrete;
  vector<int> vec_default_elim_ord;

  map<int, Node*> map_idx_node_ptr;  // Key: node index. Value: node pointer.
  vector<int> SparseInstanceFillZeroToCompleteInstance(DiscreteConfig &sparse_instance);

  Network();
  explicit Network(bool pure_disc);
  Network(Network &net);
  virtual ~Network() = default;

  void PrintEachNodeParents();
  void PrintEachNodeChildren();

  Node* FindNodePtrByIndex(const int &index) const;

  Node* FindNodePtrByName(const string &name) const;

  void ConstructNaiveBayesNetwork(Dataset *dts);

  virtual void StructLearnCompData(Dataset *, bool print_struct=true, string topo_ord_constraint="dataset-ord");

  void LearnParamsKnowStructCompData(const Dataset *dts, int alpha=1, bool print_params=true);

  int GetNumParams() const;
  void ClearStructure();
  void ClearParams();

  void AddNode(Node *node_ptr);
  void RemoveNode(int node_index);

  void SetParentChild(int, int);
  void SetParentChild(Node *par, Node *chi);

  void RemoveParentChild(int, int);
  void RemoveParentChild(Node *par, Node *chi);

  set<Node*> GetParentPtrsOfNode(int node_index);
  set<Node*> GetChildrenPtrsOfNode(int node_index);

  void GenDiscParCombsForAllNodes();

  vector<int> GetTopoOrd();
  vector<int> GetReverseTopoOrd();

  int** ConvertDAGNetworkToAdjacencyMatrix();


  virtual vector<int> SimplifyDefaultElimOrd(DiscreteConfig);

  DiscreteConfig ConstructEvidence(int *nodes_indexes, int *observations, int num_of_observations);

  vector<Factor> ConstructFactors(vector<int> Z, Node *Y);
  void LoadEvidenceIntoFactors(vector<Factor> *factors_list, DiscreteConfig E, set<int> all_related_vars);

  Factor SumProductVarElim(vector<Factor> factors_list, vector<int> Z);
  Factor VarElimInferReturnPossib(vector<int> elim_ord, DiscreteConfig evidence, Node *target);
  Factor VarElimInferReturnPossib(DiscreteConfig evidence, Node *target);

  map<int, double> DistributionOfValueIndexGivenCompleteInstanceValueIndex(int target_var_index, DiscreteConfig evidence);
  int PredictUseSimpleBruteForce(DiscreteConfig E, int Y_index);

  int PredictUseVarElimInfer(vector<int> Z, DiscreteConfig E, int Y_index);
  int PredictUseVarElimInfer(DiscreteConfig E, int Y_index);

  double TestNetReturnAccuracy(Dataset *dts);
  double TestNetReturnAccuracyGivenAllCompleteInstances(Dataset *dts);
  double TestNetByApproxInferReturnAccuracy(Dataset *dts, int num_samp);
  double TestAccuracyByLikelihoodWeighting(Dataset *dts, int num_samp);


  // Probabilistic logic sampling is a method
  // proposed by Max Henrion at 1988.
  DiscreteConfig ProbLogicSampleNetwork();


  vector<pair<DiscreteConfig, double>> DrawSamplesByLikelihoodWeighting(const DiscreteConfig &evidence, int num_samp);
  Factor CalcuMargWithLikelihoodWeightingSamples(const vector<pair<DiscreteConfig, double>> &samples, const int &node_index);
  int ApproxinferByLikelihoodWeighting(DiscreteConfig e, const int &node_index, const int &num_samp);


  int SampleNodeGivenMarkovBlanketReturnValIndex(Node *node_ptr, DiscreteConfig markov_blanket);

  vector<DiscreteConfig> DrawSamplesByProbLogiSamp(int num_samp);
  vector<DiscreteConfig> DrawSamplesByGibbsSamp(int num_samp, int num_burn_in);

  int ApproxInferByProbLogiRejectSamp(DiscreteConfig e, Node *node, vector<DiscreteConfig> &samples);
  int ApproxInferByProbLogiRejectSamp(DiscreteConfig e, int node_index, vector<DiscreteConfig> &samples);

 protected:
  vector<int> topo_ord;

  vector<int> GenTopoOrd();
  pair<DiscreteConfig, double> DrawOneLikelihoodWeightingSample(const DiscreteConfig &evidence);
  set<int> GetMarkovBlanketIndexesOfNode(Node *node_ptr);

  // ==================================================
  // Functions for structure learning.
  // Based on the work of Ott et al. (2003) FINDING OPTIMAL MODELS FOR SMALL GENE NETWORKS
  pair<double, set<Node*>> F(Node *node, set<Node*> &candidate_parents, Dataset *dts, map<Node*, map<set<Node*>, double>> &dynamic_program);
  pair<double, vector<pair<Node*, set<Node*>>>>
   Q(set<Node*> &set_nodes,
     vector<int> topo_ord,
     Dataset *dts,
     map<Node*,   map<set<Node*>, double>> &dynamic_program_for_F,
     map<pair<set<Node*>, vector<int>>,   pair<double, vector<pair<Node*, set<Node*>>>>> dynamic_program_for_Q);
  vector<int> M(set<Node*> &set_nodes,
                Dataset *dts,
                map<Node*, map<set<Node*>, double>> &dynamic_program_for_F,
                map<pair<set<Node*>, vector<int>>,   pair<double, vector<pair<Node*, set<Node*>>>>> dynamic_program_for_Q,
                map<set<Node*>, vector<int>> dynamic_program_for_M);
  void StructLearnByOtt(Dataset *dts, vector<int> topo_ord_constraint={});
  // ==================================================





};


#endif //BAYESIANNETWORK_NETWORK_H
