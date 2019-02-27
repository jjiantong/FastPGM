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

  JunctionTree() = default;
  JunctionTree(Network*);

  int** ConvertDAGNetworkToAdjacencyMatrix(Network*);
  void Moralize(int**, int&);
  vector<int> MinNeighbourElimOrd(int**, int&);
  void Triangulate(Network*, int**, int&, vector<int>, set<Clique*>&);
  void ElimRedundantCliques();
  void FormJunctionTree(set<Clique*>&);
  void AssignPotentials();
  void LoadEvidence(Combination);
  void MessagePassingUpdateJT();

  void PrintAllCliquesPotentials();
  void PrintAllSeparatorsPotentials();

  Factor InferenceForVarIndexsReturnPossib(set<int>&);

};


#endif //BAYESIANNETWORK_JUNCTIONTREE_H
