//
// Created by Linjian Li on 2018/11/29.
//

#ifndef BAYESIANNETWORK_NETWORK_H
#define BAYESIANNETWORK_NETWORK_H

#include "Trainer.h"
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


class Network {
public:
	int n_nodes;
	set<Node*> set_node_ptr_container;
	//set<Edge*> edges_container;

	/* About "tree_default_elim_ord":
	 * 		This attribute is not supposed to exist.
	 * 		But for now, I have just implemented the part of ChowLiu tree.
	 * 		And I have not implemented the part of generating an elimination order automatically.
	 * 		So, I just add this attribute to store a relatively "fixed" order.
	 * 		The order is fixed for one tree, but different for different trees.
	 * 		It is just the reverse order of topological sorting using width-first-traversal start at the root node.
	 * */
	int* tree_default_elim_ord;
	Combination network_evidence;

	Network();

	Node* GivenIndexToFindNodePointer(int);

	double ComputeMutualInformation(Node *, Node *, const Trainer *);
	void StructLearnChowLiuTreeCompData(Trainer *);

	void LearnParmsKnowStructCompData(const Trainer *);

	void SetParentChild(int, int);
	void SetParentChild(Node *, Node *);

	Combination ConstructEvidence(int *, int *, int);

	vector<Factor> ConstructFactors(int *, int, Node *);
	void LoadEvidence(vector<Factor> *, Combination);

	pair<int*, int> SimplifyTreeDefaultElimOrd();
	void DepthFirstTraversalUntillMeetObserved(int, set<int>&, set<int>&);
	void DepthFirstTraversalToRemoveMSeparatedNodes(int, set<int>&, set<int>&);

	Factor SumProductVarElim(vector<Factor>, int *, int);
	Factor VarElimInferReturnPossib(int *, int, Combination, Node *);
	Factor VarElimInferReturnPossib(Combination, Node *);

	double TestNetReturnAccuracy(Trainer *);
};


#endif //BAYESIANNETWORK_NETWORK_H
