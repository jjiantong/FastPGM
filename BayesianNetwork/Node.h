//
// Created by Linjian Li on 2018/11/29.
//

#ifndef BAYESIANNETWORK_NODE_H
#define BAYESIANNETWORK_NODE_H

#include <set>
#include <map>
#include <utility>
#include <string>
#include "gadget.h"

using namespace std;

typedef set< pair<string, int> > Combination;

class Node {
public:
	string nodeName;
	bool isDiscrete;
	int numOfPotentialValues;
	int* potentialValues;
	set<Node*> parentsPointers;
	set<Node*> childrenPointers;
	set<Combination> parentsCombinations;
	map<int, map<Combination, double> >  condProbTable;
	map<int, double>  margProbTable;

	Node();
	void addChild(Node*);
	void addParent(Node*);
	void generateParentsCombinations();
};


#endif //BAYESIANNETWORK_NODE_H
