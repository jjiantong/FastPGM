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

typedef set< pair<int, int> > Combination;

class Node {
private:
	int node_index;
public:
	bool is_discrete;
	int num_of_potential_values;
	int* potential_values;
	set<Node*> set_parents_pointers;
	set<Node*> set_children_pointers;
	set<Combination> set_parents_combinations;
	map<int, map<Combination, double> >  map_cond_prob_table;
	map<int, double>  map_marg_prob_table;

	Node();
	int GetNodeIndex();
	void SetNodeIndex(int);
	void AddChild(Node *);
	void AddParent(Node *);
	void GenerateParentsCombinations();
};


#endif //BAYESIANNETWORK_NODE_H
