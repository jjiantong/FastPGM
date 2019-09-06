//
// Created by LinjianLi on 2019/2/16.
//

#ifndef BAYESIANNETWORK_JUNCTIONTREE_H
#define BAYESIANNETWORK_JUNCTIONTREE_H

#include <set>
#include <vector>
#include "Clique.h"
#include "Separator.h"
#include "Network.h"


class JunctionTree {
 public:
  Network *network;
  set<Clique*> set_clique_ptr_container;
  set<Separator*> set_separator_ptr_container;

  vector<int> elimination_ordering;
  map<int, Clique*> map_elim_var_to_clique;

  map<Clique*,Clique> map_cliques_backup;
  map<Separator*,Separator> map_separators_backup;

  JunctionTree() = default;
  explicit JunctionTree(Network *net);
  JunctionTree(Network *net, bool elim_redundant_cliques);
  JunctionTree(Network *net, string elim_ord_strategy, bool elim_redundant_cliques);
  explicit JunctionTree(JunctionTree*);
  virtual ~JunctionTree() = default;

  static int** ConvertDAGNetworkToAdjacencyMatrix(Network*);
  static void Moralize(int**, int&);
  static vector<int> MinNeighbourElimOrd(int**, int&);
  void Triangulate(Network*, int**, int&, vector<int>, set<Clique*>&);
  void ElimRedundantCliques();
  void FormJunctionTree(set<Clique*>&);
  void NumberTheCliquesAndSeparators();
  void AssignPotentials();
  void BackUpJunctionTree();
  void ResetJunctionTree();
  virtual void LoadEvidence(const Combination&);
  void MessagePassingUpdateJT();

  void PrintAllCliquesPotentials() const;
  void PrintAllSeparatorsPotentials() const;

  Factor BeliefPropagationReturnPossib(set<int>&);
  int InferenceUsingBeliefPropagation(set<int>&);

  double TestNetReturnAccuracy(int,Trainer*);

};


#endif //BAYESIANNETWORK_JUNCTIONTREE_H
