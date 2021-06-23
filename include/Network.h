//
// Created by Linjian Li on 2018/11/29.
//

#ifndef BAYESIANNETWORK_NETWORK_H
#define BAYESIANNETWORK_NETWORK_H

#include "Dataset.h"
#include "Node.h"
#include "Edge.h"
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
  int num_edges = 0;
  bool pure_discrete;
  vector<int> topo_ord;
    /** the default value to infer is the first (i.e. root) variable **/
  vector<int> vec_default_elim_ord;//the elimination order is obtained by reverse topological sort.

  map<int, Node*> map_idx_node_ptr;  // Key: node index. Value: node pointer. This map is a helper for FindNodePtrByIndex.
  vector<Edge> vec_edges;  // edges in the network

  Network();
  explicit Network(bool pure_disc);
  Network(Network &net);
  virtual ~Network() = default;

  void PrintEachNodeParents();
  void PrintEachNodeChildren();

  Node* FindNodePtrByIndex(const int &index) const;
  Node* FindNodePtrByName(const string &name) const;

  void ConstructNaiveBayesNetwork(Dataset *dts);

  int GetNumParams() const;
  void ClearStructure();
  void ClearParams();

  void AddNode(Node *node_ptr);
  void RemoveNode(int node_index);

  bool NodeIsInNetwork(Node *node_ptr);
  bool NodeIsInNetwork(int node_idx);

  bool AddDirectedEdge(int p_index, int c_index);
  void DeleteDirectedEdge(int p_index, int c_index);
  bool ReverseDirectedEdge(int p_index, int c_index);
  void AddUndirectedEdge(int p_index, int c_index);
  void DeleteUndirectedEdge(int p_index, int c_index);

  double CalcuExtraScoreWithModifiedEdge(int p_index, int c_index, Dataset *dts, string modification, string score_metric);

  void SetParentChild(int p_index, int c_index);
  void SetParentChild(Node *par, Node *chi); // checked

  void RemoveParentChild(int p_index, int c_index);
  void RemoveParentChild(Node *par, Node *chi);

  set<Node*> GetParentPtrsOfNode(int node_index);
  set<Node*> GetChildrenPtrsOfNode(int node_index);

  void GenDiscParCombsForAllNodes();

  vector<int> GetTopoOrd();
  vector<int> GetReverseTopoOrd();

  set<int> GetMarkovBlanketIndexesOfNode(Node *node_ptr);

  int** ConvertDAGNetworkToAdjacencyMatrix();
  bool ContainCircle();

  virtual vector<int> SimplifyDefaultElimOrd(DiscreteConfig evidence);

  vector<Factor> ConstructFactors(vector<int> Z, Node *Y);
  void LoadEvidenceIntoFactors(vector<Factor> *factors_list, DiscreteConfig E, set<int> all_related_vars);

  Factor SumProductVarElim(vector<Factor> factors_list, vector<int> elim_order);
  Factor VarElimInferReturnPossib(DiscreteConfig evid, Node *target_node, vector<int> elim_order=vector<int>{});


 protected:
  vector<int> GenTopoOrd();
};


#endif //BAYESIANNETWORK_NETWORK_H

