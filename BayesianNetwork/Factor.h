//
// Created by Linjian Li on 2018/11/29.
//

#ifndef BAYESIANNETWORK_FACTOR_H
#define BAYESIANNETWORK_FACTOR_H

#include <set>
#include <map>
#include <utility>
#include <string>
#include <algorithm>

#include "gadget.h"
#include "Node.h"

using namespace std;

typedef set< pair<string, int> > Combination;

class Factor {
public:

	set<string> relatedVariables;
	set<Combination> combList;
	map<Combination, double> potentialsList;

	Factor();
	void constructFactor(Node*);
	Factor multiplyWithFactor(Factor);
	Factor sumProductOverVariable(Node*);
	void normalize();
};


#endif //BAYESIANNETWORK_FACTOR_H
