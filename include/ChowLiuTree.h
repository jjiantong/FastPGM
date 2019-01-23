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


class ChowLiuTree {
public:
	double ComputeMutualInformation(Node *, Node *, const Trainer *);
	void StructLearnChowLiuTreeCompData(Trainer *, Network *);
};


#endif //BAYESIANNETWORK_CHOWLIUTREE_H
