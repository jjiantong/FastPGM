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

typedef set< pair<string, int> > Combination;

class Network {
public:
	int numOfNodes;
	string* nodesOrder;
	set<Node*> nodesContainer;
	//set<Edge*> edgesContainer;

	/* About "treeDefaultEliminationOrder":
	 * 		This attribute is not supposed to exist.
	 * 		But for now, I have just implemented the part of ChowLiu tree.
	 * 		And I have not implemented the part of generating an elimination order automatically.
	 * 		So, I just add this attribute to store a relatively "fixed" order.
	 * 		The order is fixed for one tree, but different for different trees.
	 * 		It is just the reverse order of width-first-traversal start at the root node.
	 * */
	int* treeDefaultEliminationOrder;

	Network();

	Node* givenIndexToFindNodePointer(int);
	int givenNodePointerToFindIndex(Node*);
	int givenNodeNameToFindIndex(string);
	double computeMutualInformation(Node*, Node*, const Trainer*);
	void structLearn_ChowLiu_CompData(const Trainer *trainer);
	void trainNetwork_KnowStruct_CompData(const Trainer*);
	void setParentChild(Node*, Node*);

	Combination constructEvidence(int*, int*, int);
	vector<Factor> constructFactors(int*, int, Node*);
	void loadEvidence(vector<Factor>*, Combination);
	Factor sumProductVariableElimination(vector<Factor>, int*, int);
	Factor variableEliminationInferenceReturningPossibilities(int*, int, Combination, Node*);
	Factor variableEliminationInferenceReturningPossibilities(Combination, Node*);

	double testingNetworkReturnAccuracy(Trainer*);
};


#endif //BAYESIANNETWORK_NETWORK_H
