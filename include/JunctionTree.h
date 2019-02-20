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
  set<Clique*> set_clique_ptr_container;
  set<Separator*> set_separator_ptr_container;

  JunctionTree(Network*);

  int** ConvertDAGNetworkToAdjacencyMatrix(Network*);
  void Moralize(int**, int);
  void Triangulate(Network*, int**, const int&, vector<int>, set<Clique*>&);
  void FormJunctionTree(set<Clique*>);

};


#endif //BAYESIANNETWORK_JUNCTIONTREE_H
