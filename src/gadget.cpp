//
// Created by Linjian Li on 2018/12/5.
//

#include "gadget.h"


set<Combination> GenAllCombFromSets(set<Combination> *set_of_sets) {

	// Error Case
	if (set_of_sets->empty()) {
		cout << "The size of set_of_sets is less than 1." << endl;
		exit(1);
	}

	auto its=set_of_sets->begin();
	Combination toBeAdded;
	toBeAdded =  *its;
	set<Combination> result;

	// Base Case
	if (set_of_sets->size()==1) {
		for (auto &p : toBeAdded){
			Combination c;
			c.insert(p);
			result.insert(c);
		}
		return result;
	}

	// Recursive Case (the size of set_of_sets is greater than 1)
	set_of_sets->erase(its);
	result = GenAllCombFromSets(set_of_sets);
	for (auto &p : toBeAdded){
		for (auto c : result) {
			c.insert(p);
			result.insert(c);
		}
	}
	return result;
}


bool EachFirstIsInSecond(Combination *first, Combination *second) {
	for (auto &f : *first) {
		if (second->find(f)==second->end()) return false;
	}
	return true;
}


bool FirstCompatibleSecond(Combination *first, Combination *second) {
	for (auto &f : *first) {
		for (auto &s : *second) {
			if (f.first==s.first && f.second!=s.second) return false;
		}
	}
	return true;
}


int* WidthFirstTraversalWithAdjacencyMatrix(int **graph, int numOfNodes, int start) {
	int* result = new int[numOfNodes];
	int itResult = 0;
	queue<int> que;
	set<int> markSet;
	que.push(start);
	while (!que.empty()) {
		int pos = que.front();
		result[itResult++] = pos;
		markSet.insert(pos);
		que.pop();
		for (int i=0; i<numOfNodes; i++) {
			if (graph[pos][i]!=0 && markSet.find(i)==markSet.end()) {
				que.push(i);
			}
		}
	}
	return result;
}