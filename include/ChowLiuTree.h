//
// Created by LinjianLi on 2019/1/23.
//

#ifndef BAYESIANNETWORK_CHOWLIUTREE_H
#define BAYESIANNETWORK_CHOWLIUTREE_H

#include "Trainer.h"
#include "Network.h"
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


class ChowLiuTree : public Network {
 public:

  /* About (tree)"default_elim_ord":
   *   This attribute is not supposed to exist.
   *   But for now, I have just implemented the part of ChowLiu tree.
   *   And I have not implemented the part of generating an elimination order automatically.
   *   So, I just add this attribute to store a relatively "fixed" order.
   *   The order is fixed for one tree, but different for different trees.
   *   It is just the reverse order of topological sorting using width-first-traversal start at the root node.
   */
  int *default_elim_ord;
  double ComputeMutualInformation(Node *, Node *, const Trainer *);
  void StructLearnCompData(Trainer *);
  void StructLearnChowLiuTreeCompData(Trainer *);

  pair<int*, int> SimplifyDefaultElimOrd(Combination) override;
  pair<int*, int> SimplifyTreeDefaultElimOrd(Combination);
  void DepthFirstTraversalUntillMeetObserved(Combination, int, set<int> &, set<int> &);
  void DepthFirstTraversalToRemoveMSeparatedNodes(int, set<int> &, set<int> &);

};


#endif //BAYESIANNETWORK_CHOWLIUTREE_H
