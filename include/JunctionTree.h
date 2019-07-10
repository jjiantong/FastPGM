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

  map<Clique*,Clique> map_cliques_backup;
  map<Separator*,Separator> map_separators_backup;

  JunctionTree() = default;
  JunctionTree(Network*);

  int** ConvertDAGNetworkToAdjacencyMatrix(Network*);
  void Moralize(int**, int&);
  vector<int> MinNeighbourElimOrd(int**, int&);
  void Triangulate(Network*, int**, int&, vector<int>, set<Clique*>&);
  void ElimRedundantCliques();
  void FormJunctionTree(set<Clique*>&);
  void AssignPotentials();
  void BackUpJunctionTree();
  void ResetJunctionTree();
  void LoadEvidence(Combination);
  void MessagePassingUpdateJT();

  void PrintAllCliquesPotentials() const;
  void PrintAllSeparatorsPotentials() const;

  Factor BeliefPropagationReturnPossib(set<int>&);
  int InferenceUsingBeliefPropagation(set<int>&);

  double TestNetReturnAccuracy(int,Trainer*);

};


#endif //BAYESIANNETWORK_JUNCTIONTREE_H
