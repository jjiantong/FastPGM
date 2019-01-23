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


class ChowLiuTree : Network {
public:
	int *tree_default_elim_ord;
	double ComputeMutualInformation(Node *, Node *, const Trainer *);
	void StructLearnChowLiuTreeCompData(Trainer *);

	pair<int*, int> SimplifyTreeDefaultElimOrd(Network *);
	void DepthFirstTraversalUntillMeetObserved(int, set<int> &, set<int> &);
	void DepthFirstTraversalToRemoveMSeparatedNodes(int, set<int> &, set<int> &);

};


#endif //BAYESIANNETWORK_CHOWLIUTREE_H
