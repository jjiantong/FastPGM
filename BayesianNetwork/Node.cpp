//
// Created by Linjian Li on 2018/11/29.
//

#include "Node.h"


Node::Node() {
	// todo do something
}

void Node::addParent(Node* p) {
	parentsPointers.insert(p);
}

void Node::addChild(Node* c) {
	childrenPointers.insert(c);
}

void Node::generateParentsCombinations() {
	// Preprocess
	set<Combination> setOfSets;
	if (parentsPointers.size()<1) return;
	for (auto par : parentsPointers) {
		Combination cb;
		pair<string, int> ele;
		for (int i=0; i<par->numOfPotentialValues; i++) {
			ele.first = par->nodeName;
			ele.second = par->potentialValues[i];
			cb.insert(ele);
		}
		setOfSets.insert(cb);
	}

	// Generate
	parentsCombinations = generate_all_possible_combinations_from_several_sets(&setOfSets);

}