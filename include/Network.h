//
// Created by Linjian Li on 2018/11/29.
//

#ifndef BAYESIANNETWORK_NETWORK_H
#define BAYESIANNETWORK_NETWORK_H

#include "Dataset.h"
#include "Node.h"
#include "Factor.h"
#include "gadget.h"
#include <string>
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
#include <bits/stdc++.h>
#include "omp.h"


using namespace std;

// Forward Declaration
class ScoreFunction;

//TODO: devide into 3 or more classes (e.g. network, learning and inference)
class Network {//this class is used by both the customized networks and networks learning from data.
 public:
  string network_name;//a name for each bayesian network (usually an xml file contains a name for the network).
  int num_nodes = 0;
  bool pure_discrete;
  /** the default value to infer is the first (i.e. root) variable **/
  vector<int> vec_default_elim_ord;//the elimination order is obtained by reverse topological sort.

  map<int, Node*> map_idx_node_ptr;  // Key: node index. Value: node pointer. This map is a helper for FindNodePtrByIndex.

  Network();
  explicit Network(bool pure_disc);
  Network(Network &net);
  virtual ~Network() = default;

  void PrintEachNodeParents();
  void PrintEachNodeChildren();

  Node* FindNodePtrByIndex(const int &index) const;
  Node* FindNodePtrByName(const string &name) const;

  void ConstructNaiveBayesNetwork(Dataset *dts);
  virtual void StructLearnCompData(Dataset *dts, bool print_struct=true,
                                   string algo="k2-weka", string topo_ord_constraint="dataset-ord",
                                   int max_num_parents=INT_MAX);

  // TODO: understanding: I think this function just gets each node's conditional probability table
  void LearnParamsKnowStructCompData(const Dataset *dts, int alpha=1, bool print_params=true);

  int GetNumParams() const;
  void ClearStructure();
  void ClearParams();

  void AddNode(Node *node_ptr);
  void RemoveNode(int node_index);

  bool AddArc(int p_index, int c_index);
  void DeleteArc(int p_index, int c_index);
  bool ReverseArc(int p_index, int c_index);

  double CalcuExtraScoreWithModifiedArc(int p_index, int c_index, Dataset *dts,
                                        string modification, string score_metric);

  void SetParentChild(int p_index, int c_index);
  void SetParentChild(Node *par, Node *chi); // checked

  void RemoveParentChild(int p_index, int c_index);
  void RemoveParentChild(Node *par, Node *chi);

  set<Node*> GetParentPtrsOfNode(int node_index);
  set<Node*> GetChildrenPtrsOfNode(int node_index);

  void GenDiscParCombsForAllNodes();

  vector<int> GetTopoOrd();
  vector<int> GetReverseTopoOrd();

  int** ConvertDAGNetworkToAdjacencyMatrix();
  bool ContainCircle();


  virtual vector<int> SimplifyDefaultElimOrd(DiscreteConfig evidence);
  virtual vector<int> SimplifyDefaultElimOrd2(DiscreteConfig evidence, vector<int> left_nodes);

  vector<Factor> ConstructFactors(vector<int> Z, Node *Y);
  void LoadEvidenceIntoFactors(vector<Factor> *factors_list, DiscreteConfig E, set<int> all_related_vars);

  Factor SumProductVarElim(vector<Factor> factors_list, vector<int> elim_order);
  Factor VarElimInferReturnPossib(DiscreteConfig evid, Node *target_node, vector<int> elim_order=vector<int>{});

  Factor GetMarginalProbabilitiesUseBruteForce(int target_var_index, DiscreteConfig evidence);

  int PredictUseBruteForce(DiscreteConfig evid, int target_node_idx);
  vector<int> PredictUseBruteForce(vector<DiscreteConfig> evidences, int target_node_idx);

  DiscreteConfig GenerateInstanceByProbLogicSampleNetwork();

  int SampleNodeGivenMarkovBlanketReturnValIndex(Node *node_ptr, DiscreteConfig markov_blanket);

  vector<DiscreteConfig> DrawSamplesByGibbsSamp(int num_samp, int num_burn_in);


 protected:
  vector<int> topo_ord;

  vector<int> GenTopoOrd();
  set<int> GetMarkovBlanketIndexesOfNode(Node *node_ptr);

  /**
   * Functions for structure learning.
   * This learning process has 4 functions, i.e., F, Q, M and StructLearnByOtt
   */
  pair<double, set<Node*>> F(Node *node,
                             set<Node*> &candidate_parents,
                             Dataset *dts,
                             map<Node*, map<set<Node*>, double>> &dynamic_program);
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
  void StructLearnByOtt(Dataset *dts, vector<int> topo_ord_constraint={}); // TODO: understanding of Ott's algorithm


  /**
   * Function for structure learning like K2 of Weka
   */
  void StructLearnLikeK2Weka(Dataset *dts, vector<int> topo_ord_constraint={}, int max_num_parents=INT_MAX);
};


#endif //BAYESIANNETWORK_NETWORK_H

