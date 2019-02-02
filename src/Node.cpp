//
// Created by Linjian Li on 2018/11/29.
//

#include "Node.h"


Node::Node() {}


int Node::GetNodeIndex() {
	return node_index;
}


void Node::SetNodeIndex(int i) {
	if (i<0) exit(1);
	node_index = i;
}


void Node::AddParent(Node *p) {
	set_parents_pointers.insert(p);
}


void Node::AddChild(Node *c) {
	set_children_pointers.insert(c);
}


void Node::RemoveChild(Node *c) {
	set_children_pointers.erase(c);
}


void Node::RemoveParent(Node *p) {
	set_parents_pointers.erase(p);
}


void Node::GenParCombs() {
	// Preprocess
	set<Combination> set_of_sets;
	if (set_parents_pointers.empty()) return;
	for (auto par_ptr : set_parents_pointers) {
		Combination cb;
		pair<int, int> ele;
		for (int i=0; i<par_ptr->num_of_potential_values; i++) {
			ele.first = par_ptr->node_index;
			ele.second = par_ptr->potential_values[i];
			cb.insert(ele);
		}
		set_of_sets.insert(cb);
	}

	// Generate
	set_parents_combinations = GenAllCombFromSets(&set_of_sets);

}