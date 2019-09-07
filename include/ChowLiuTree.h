//
// Created by LinjianLi on 2019/1/23.
//

#ifndef BAYESIANNETWORK_CHOWLIUTREE_H
#define BAYESIANNETWORK_CHOWLIUTREE_H

#include "Trainer.h"
#include "Network.h"
#include "Node.h"
#include "DiscreteNode.h"
#include "Factor.h"
#include "gadget.h"
#include <set>
#include <queue>
#include <vector>
#include <cmath>
#include <iostream>
#include <sys/time.h>
#include <locale.h>
#include "omp.h"

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

  ChowLiuTree();
  explicit ChowLiuTree(bool pure_disc);

  double ComputeMutualInformation(Node *Xi, Node *Xj, const Trainer *trainer);
  void StructLearnCompData(Trainer *trainer, bool print_struct=true) override;
  void StructLearnChowLiuTreeCompData(Trainer *trainer, bool print_struct=true);

  void LearnParamsKnowStructCompData(const Trainer *trainer, bool print_params=true) override;


  pair<int*, int> SimplifyDefaultElimOrd(Combination evidence) override;
  pair<int*, int> SimplifyTreeDefaultElimOrd(Combination evidence);

 protected:
  void DepthFirstTraversalUntillMeetObserved(Combination evidence, int start, set<int>& visited, set<int>& to_be_removed);
  void DepthFirstTraversalToRemoveMSeparatedNodes(int start, set<int>& visited, set<int>& to_be_removed);

};


#endif //BAYESIANNETWORK_CHOWLIUTREE_H
