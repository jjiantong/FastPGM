//
// Created by Linjian Li on 2018/11/29.
//

#include "Network.h"

Network::Network() {
	// do something
}

Node* Network::givenIndexToFindNodePointer(int index) {
	if (index<0 || index>=numOfNodes) {
		cout << "=======================================================================" << '\n'
			 << "Node* Network::givenIndexToFindNodePointer(int index)\n"
			 << "Invalid index!!!" << endl;
		exit(1);
	}
	Node* nodePtr = nullptr;
	for (set<Node*>::iterator it=nodesContainer.begin(); it!=nodesContainer.end(); it++) {
		if ((*it)->nodeName==nodesOrder[index] && nodePtr==nullptr) {
			nodePtr = *it;
			return nodePtr;
		}
	}
}

int Network::givenNodePointerToFindIndex(Node* nodePtr) {
	int index = -1;
	for (int j=0; j<numOfNodes; j++) {
		if (nodesOrder[j]==nodePtr->nodeName && index==-1) {
			index = j;
			return index;
		}
	}
}

int Network::givenNodeNameToFindIndex(string nodeName) {
	int index = -1;
	for (int i=0; i<numOfNodes; i++) {
		if (nodesOrder[i]==nodeName && index==-1) {
			index = i;
			return index;
		}
	}
}

double Network::computeMutualInformation(Node* Xi, Node* Xj, const Trainer* trainer) {
	// Find the indexes of these two features in training set.
	int xi=-1, xj=-1;
	for (int k=0; k<numOfNodes; k++) {
		if (trainer->featuresNames[k]==Xi->nodeName && xi==-1) {
			xi = k;
		} else if (trainer->featuresNames[k]==Xj->nodeName && xj==-1) {
			xj = k;
		}
	}

	// Initialize the table.
	int m = trainer->numOfTrainingSamples, ri = Xi->numOfPotentialValues, rj = Xj->numOfPotentialValues;
	double** Pij = new double* [ri];
	for (int i=0; i<ri; i++) {
		Pij[i] = new double[rj]();		// The parentheses at end will initialize the array to be all zeros.
	}
	double* Pi = new double[ri]();
	double* Pj = new double[rj]();
	int a, b, s;

	// Update Pij.
	for (a=0; a<ri; a++) {
		for (b=0; b<rj; b++) {
			for (s=0; s<m; s++) {
				if (trainer->trainingSet[s][xi]==Xi->potentialValues[a] && trainer->trainingSet[s][xj]==Xj->potentialValues[b]) {
					Pij[a][b] += 1;
				}
			}
			Pij[a][b] /= m;
		}
	}

	// Update Pi.
	for (a=0; a<ri; a++) {
		for (s=0; s<m; s++) {
			if (trainer->trainingSet[s][xi]==Xi->potentialValues[a]) {
				Pi[a] += 1;
			}
		}
		Pi[a] /= m;
	}

	// Update Pj.
	for (b=0; b<rj; b++) {
		for (s=0; s<m; s++) {
			if (trainer->trainingSet[s][xj]==Xj->potentialValues[b]) {
				Pj[b] += 1;
			}
		}
		Pj[b] /= m;
	}

	// Calculate mutual information.
	double mutualInformation = 0;
	for (a=0; a<ri; a++) {
		for (b = 0; b < rj; b++) {
			double temp = Pij[a][b] * log( Pij[a][b] / (Pi[a]*Pj[b]) );
			if (!isnan(temp)) {		// There may be runtime error of divided by zero.
				mutualInformation += temp;
			}
		}
	}

	// Return
	return mutualInformation;
}


void Network::setParentChild(Node* p, Node* c) {
	p->addChild(c);
	c->addParent(p);
}

void Network::structLearn_ChowLiu_CompData(const Trainer *trainer) {
	cout << "=======================================================================" << '\n'
		 << "Begin structural learning. \nConstructing Chow-Liu tree with complete data......" << endl;
	numOfNodes = trainer->numOfFeatures;
	nodesOrder = trainer->featuresNames;
	for (int i=0; i<numOfNodes; i++) {
		Node* node = new Node();
		node->nodeName = trainer->featuresNames[i];
		node->isDiscrete = trainer->isFeaturesDiscrete[i];
		node->numOfPotentialValues = trainer->numOfPossibleValuesOfFeatures[i];

		/* todo:
		 * 	For now:
		 * 		This "potentialValues" is set to be {-1,0,1} for this prototype.
		 * 		Because libSVM  data set "a1a" only contains these values.
		 * 		The class variable is in {-1,1} and the other features is in {0,1}.
		 * 	Future work:
		 * 		Let it adept to the more general cases.
		 * */
		node->potentialValues = new int[node->numOfPotentialValues];
		if (i==0) {	// The first node denotes the class variable.
			node->potentialValues[0] = -1;
			node->potentialValues[1] = 1;
		} else {	// The other nodes are features.
			node->potentialValues[0] = 0;
			node->potentialValues[1] = 1;
		}
		nodesContainer.insert(node);
	}

	cout << "=======================================================================" << '\n'
		 << "Constructing mutual information table......" << endl;

	int n = numOfNodes;
	double** mutualInfoTab = new double* [n];
	for (int i=0; i<n; i++) {
		mutualInfoTab[i] = new double[n]();		// The parentheses at end will initialize the array to be all zeros.
	}

	// Update the mutual information table.
	for (int i=0; i<n; i++) {
		for (int j=0; j<i; j++) {
			if (i==j) {
				mutualInfoTab[i][j] = -1;
			} else {
				// To calculate the mutual information, we need to find the nodes which correspond to the indexes i and j.
				Node* Xi = nullptr;
				Node* Xj = nullptr;
				for (set<Node*>::iterator it=nodesContainer.begin(); it!=nodesContainer.end(); it++) {
					if ((*it)->nodeName==nodesOrder[i] && Xi==nullptr) {
						Xi = *it;
					} else if ((*it)->nodeName==nodesOrder[j] && Xj==nullptr) {
						Xj = *it;
					}
				}
				mutualInfoTab[i][j] = mutualInfoTab[j][i] = computeMutualInformation(Xi, Xj, trainer);	// Mutual information table is symmetric.
			}
		}
	}

	cout << "=======================================================================" << '\n'
		 << "Constructing maximum spanning tree using mutual information table and Prim's algorithm......" << endl;

	// Use Prim's algorithm to generate a spanning tree.
	int** graphAdjacencyMatrix = new int* [n];
	for (int i=0; i<n; i++) {
		graphAdjacencyMatrix[i] = new int[n]();
	}
	set<int> markSet;
	double maxMutualInfo;
	int maxI, maxJ;
	set<int>::iterator it;
	markSet.insert(0);	// The node of index 0 is the root node, which is the class variable.
	while (markSet.size()<n) {
		maxMutualInfo = -1;
		maxI = maxJ = -1;
		for (it=markSet.begin(); it!=markSet.end(); it++) {
			int i = *it;
			for (int j=0; j<n; j++) {
				if (markSet.find(j)==markSet.end() && mutualInfoTab[i][j]>maxMutualInfo) {
					maxMutualInfo = mutualInfoTab[i][j];
					maxI = i;
					maxJ = j;
				}
			}
		}
		markSet.insert(maxJ);
		// todo add edge(maxI,maxJ) to edgesContainer.
		graphAdjacencyMatrix[maxI][maxJ] = graphAdjacencyMatrix[maxJ][maxI] = 1;
	}
	// Add arrows in tree, set parents and childrens
	int* topologicalSortedPermutation = widthFirstTraversalWithAdjacencyMatirx(graphAdjacencyMatrix, n, 0);


	// !!! See the comments for "treeDefaultEliminationOrder" in the "Network.h" file.
	treeDefaultEliminationOrder = new int[n-1];
	for (int i=1; i<n; i++) {
		treeDefaultEliminationOrder[i-1] = topologicalSortedPermutation[n-i];
	}


	cout << "=======================================================================" << '\n'
		 << "Setting children and parents......" << endl;
	for (int i=0; i<n; i++) {
		for (int j=0; j<i; j++) {	// graphAdjacencyMatrix is symmetric, so loop while j<i instead of j<n
			if (i==j) continue;
			if (graphAdjacencyMatrix[i][j]==1){

				// Determine the topological position of i and j.
				int topoIndexI=-1, topoIndexJ=-1;
				for (int k=0; k<n; k++) {
					if (topologicalSortedPermutation[k]==i && topoIndexI==-1) {
						topoIndexI = k;
					} else if (topologicalSortedPermutation[k]==j && topoIndexJ==-1) {
						topoIndexJ = k;
					}
				}

				Node* Xi = givenIndexToFindNodePointer(i);
				Node* Xj = givenIndexToFindNodePointer(j);
				if (topoIndexI<topoIndexJ) {
					setParentChild(Xi, Xj);
				} else {
					setParentChild(Xj, Xi);
				}
			}
		}
	}

	cout << "=======================================================================" << '\n'
		 << "Generating parents combinations for each node......" << endl;
	for (set<Node*>::iterator it=nodesContainer.begin(); it!=nodesContainer.end(); it++) {
		(*it)->generateParentsCombinations();
	}

	cout << "=======================================================================" << '\n'
		 << "Finish structural learning." << endl;


	// The following code are just to print the result.

	// todo delete
	cout << "=======================================================================" << '\n'
		 << "The Chow-Liu Tree has the following edges (adjacency matrix): " << endl;
	for (int l = 0; l < n; ++l) {
		for (int j = 0; j < n; ++j) {
			if (graphAdjacencyMatrix[l][j]==1) {
				cout << '<' << l << ',' << j << '>' << '\t';
			}
		}
		cout << endl;
	}

	// todo delete
	cout << "=======================================================================" << '\n'
		 << "Topological sorted permutation generated using width-first-traversal: " << endl;
	for (int m = 0; m < n; ++m) {
		cout << topologicalSortedPermutation[m] << '\t';
	}
	cout << endl;

	cout << "=======================================================================" << '\n'
		 << "Each node's parents: " << endl;
	for (set<Node*>::iterator itNC=nodesContainer.begin(); itNC!=nodesContainer.end(); itNC++) {
		Node* thisNode = (*itNC);
		cout << thisNode->nodeName << ":\t";
		for (set<Node*>::iterator itPP=thisNode->parentsPointers.begin(); itPP!=thisNode->parentsPointers.end(); itPP++) {
			Node* parentNode = (*itPP);
			cout << parentNode->nodeName << '\t';
		}
		cout << endl;
	}

	cout << "=======================================================================" << '\n'
		 << "Each node's children: " << endl;
	for (set<Node*>::iterator itNC=nodesContainer.begin(); itNC!=nodesContainer.end(); itNC++) {
		Node* thisNode = (*itNC);
		cout << thisNode->nodeName << ":\t";
		for (set<Node*>::iterator itCP=thisNode->childrenPointers.begin(); itCP!=thisNode->childrenPointers.end(); itCP++) {
			Node* childNode = (*itCP);
			cout << childNode->nodeName << '\t';
		}
		cout << endl;
	}

	cout << "=======================================================================" << '\n'
		 << "Each node's parents' combination: " << endl;
	for (set<Node*>::iterator itNC=nodesContainer.begin(); itNC!=nodesContainer.end(); itNC++) {
		Node* thisNode = (*itNC);
		cout << thisNode->nodeName << ":\t";
		if (thisNode->parentsCombinations.size()<1) continue;
		cout << "Num of par comb: " << thisNode->parentsCombinations.size() << endl;
		for (set<Combination>::iterator itParCom = thisNode->parentsCombinations.begin(); itParCom != thisNode->parentsCombinations.end(); itParCom++) {
			Combination comb = (*itParCom);
			for (Combination::iterator itComb=comb.begin(); itComb!=comb.end(); itComb++) {
				pair<string, int> thisPair = (*itComb);
				cout << "(\"" << thisPair.first << "\"=" << thisPair.second << ")\t";
			}
			cout << "." << endl;
		}
		cout << endl;
	}

}

void Network::trainNetwork_KnowStruct_CompData(const Trainer* trainer){
	cout << "=======================================================================" << '\n'
		 << "Begin training with known structure and complete data." << endl;
	for (int i=0; i<trainer->numOfFeatures; i++) {		// For every node.

		Node* thisNode = givenIndexToFindNodePointer(i);

		if (thisNode->parentsPointers.empty()) {    // If this node has no parents
			map<int, double> *MPT = &(thisNode->margProbTable);
			int denominator = 0;
			for (int s = 0; s < trainer->numOfTrainingSamples; s++) {
				denominator += 1;
				int query = trainer->trainingSet[s][i];
				(*MPT)[query] += 1;
			}
			for (int i = 0; i < thisNode->numOfPotentialValues; i++) {
				int query = thisNode->potentialValues[i];
				(*MPT)[query] /= denominator;
			}
			continue; // for (int i=0; i<trainer->numOfFeatures; i++) {		// For every node.
		}


		map<int, map<Combination, double> >* CPT = &(thisNode->condProbTable);
		set<Combination>* parComb = &(thisNode->parentsCombinations);
		for (set<Combination>::iterator itParComb=parComb->begin(); itParComb!=parComb->end(); itParComb++) {		// For each column in CPT. Because the sum over column of CPT must be 1.
			int denominator = 0;
			for (int s=0; s<trainer->numOfTrainingSamples; s++) {
				// todo: calculate compatibility between (*itParComb) and (trainer->trainingSet[s])
				int compatibility = 1;		// We assume compatibility is 1, and set it to 0 if we find that (*itParComb) is not compatible with (trainer->trainingSet[s])
				Combination comb = (*itParComb);
				for (Combination::iterator itC = comb.begin(); itC!=comb.end(); itC++) {
					pair<string, int> p = (*itC);
					string thisNodeName = p.first;
					int thisNodeValue = p.second;
					int index = givenNodeNameToFindIndex(thisNodeName);
					if (trainer->trainingSet[s][index]!=thisNodeValue) {
						compatibility = 0;
					}
				}
				denominator += compatibility;
				int query = trainer->trainingSet[s][i];
				(*CPT)[query][*itParComb] += compatibility;
			}
			for (int i=0; i<thisNode->numOfPotentialValues; i++) {
				int query = thisNode->potentialValues[i];
				(*CPT)[query][*itParComb] /= denominator;
			}
		}
	}
	cout << "=======================================================================" << '\n'
		 << "Finish training with known structure and complete data." << endl;

	// The following code are just to print the result.
	cout << "=======================================================================" << '\n'
		 << "Each node's conditional probability table: " << endl;
	for (set<Node*>::iterator itNC=nodesContainer.begin(); itNC!=nodesContainer.end(); itNC++) {	// For each node
		Node* thisNode = (*itNC);
		cout << thisNode->nodeName << ":\t";


		if (thisNode->parentsPointers.empty()) {    // If this node has no parents
			for(int i=0; i<thisNode->numOfPotentialValues; i++) {		// For each row of MPT
				int query = thisNode->potentialValues[i];
				cout << "P(" << query << ")=" << thisNode->margProbTable[query] << '\t';
			}
			cout << endl;
			continue;	// for (set<Node*>::iterator itNC=nodesContainer.begin(); itNC!=nodesContainer.end(); itNC++) {	// For each node
		}


		for(int i=0; i<thisNode->numOfPotentialValues; i++) {		// For each row of CPT
			int query = thisNode->potentialValues[i];
			for (set<Combination>::iterator itParCom = thisNode->parentsCombinations.begin(); itParCom != thisNode->parentsCombinations.end(); itParCom++) {	// For each column of CPT
				Combination comb = (*itParCom);
				string condition = "";
				for (Combination::iterator itCom = comb.begin(); itCom!=comb.end(); itCom++) {
					condition = condition + "\"" + (*itCom).first + "\"=" + to_string((*itCom).second);
				}
				cout << "P(" << query << '|' << condition << ")=" << thisNode->condProbTable[query][comb] << '\t';
			}
		}
		cout << endl;

	}
}

Combination Network::constructEvidence(int* nodesIndexes, int* observations, int numOfObservations) {
	Combination result;
	pair<string, int> p;
	for (int i=0; i<numOfObservations; i++) {
		p.first = givenIndexToFindNodePointer(nodesIndexes[i])->nodeName;
		p.second = observations[i];
		result.insert(p);
	}
	return result;
}

/*
vector<Factor> Network::constructFactorsWithEvidences(int* Z, int nz, Node* Y, Combination E) {
	vector<Factor> factorsList;
	Factor factor;
	factor.constructFactor(Y);
	for (Combination::iterator itE=E.begin(); itE!=E.end(); itE++) {	// Check each observation in E
		if ((*itE).first==Y->nodeName) {	// If this node "Y" has been observed
			for (set<Combination>::iterator itCL=factor.combList.begin(); itCL!=factor.combList.end(); itCL++) {	// Update each row of potentialsList
				// If (*itCL) is not compatible with (*itE), set potentialsList[*itCL]=0.
				if ((*itCL).find(*itE)==(*itCL).end()) {
					factor.potentialsList[*itCL] = 0;
				}
			}
			break;
		}
	}
	factorsList.push_back(factor);
	for (int i=0; i<nz; i++) {
		Node* n = givenIndexToFindNodePointer(Z[i]);
		Factor factor;
		factor.constructFactor(n);
		for (Combination::iterator itE=E.begin(); itE!=E.end(); itE++) {	// Check each observation in E
			if ((*itE).first==n->nodeName) {	// If this node "n" has been observed
				for (set<Combination>::iterator itCL=factor.combList.begin(); itCL!=factor.combList.end(); itCL++) {	// Update each row of potentialsList
					// If (*itCL) is not compatible with (*itE), set potentialsList[*itCL]=0.
					if ((*itCL).find(*itE)==(*itCL).end()) {
						factor.potentialsList[*itCL] = 0;
					}
				}
				break;
			}
		}
		factorsList.push_back(factor);
	}
	return factorsList;
}
*/

vector<Factor> Network::constructFactors(int* Z, int nz, Node* Y) {
	vector<Factor> factorsList;
	Factor factor;
	factor.constructFactor(Y);
	factorsList.push_back(factor);
	for (int i=0; i<nz; i++) {
		Node* n = givenIndexToFindNodePointer(Z[i]);
		Factor factor;
		factor.constructFactor(n);
		factorsList.push_back(factor);
	}
	return factorsList;
}

void Network::loadEvidence(vector<Factor>* factorsList, Combination E) {
	for (vector<Factor>::iterator itFL=factorsList->begin(); itFL!=factorsList->end(); itFL++) {	// For each factor
		for (Combination::iterator itE=E.begin(); itE!=E.end(); itE++) {	// For each node's observation in E
			if ((*itFL).relatedVariables.find((*itE).first)!=(*itFL).relatedVariables.end()) {	// If this factor is related to this node
				for (set<Combination>::iterator itCL=(*itFL).combList.begin(); itCL!=(*itFL).combList.end(); itCL++) {	// Update each row of potentialsList
					// If (*itCL) is not compatible with (*itE), set potentialsList[*itCL]=0.
					if ((*itCL).find(*itE)==(*itCL).end()) {
						(*itFL).potentialsList[*itCL] = 0;
					}
				}
			}
		}
	}
}

Factor Network::sumProductVariableElimination(vector<Factor> factorsList, int* Z, int nz) {
	for (int i=0; i<nz; i++) {
		vector<Factor> tempFactorsList;
		Node* nodePtr = givenIndexToFindNodePointer(Z[i]);
		// Move every factor that is related to the node Z[i] from factorsList to tempFactorsList.
		/*
		 * Note: This for loop does not contain "it++" in the parentheses.
		 * 		 Because if we do so, it may cause some logic faults which, however, will not cause runtime error, so hard to debug.
		 * 		 For example:
		 * 		 	When "it" reaches the second to last element, and this element is related to the node.
		 * 		 	Then this element will be erase from factorsList, and then "it++" which will move "it" to the end.
		 * 		 	Then the for loop will end because "it" has reached the end.
		 * 		 	However, at this time, the last element has been ignored, even if it is related to the node.
		 */
		for (vector<Factor>::iterator it=factorsList.begin(); it!=factorsList.end(); /* no it++ */) {
			if ((*it).relatedVariables.find(nodePtr->nodeName)!=(*it).relatedVariables.end()) {
				tempFactorsList.push_back(*it);
				factorsList.erase(it);
				continue;
			} else {
				it++;
			}
		}
		while(tempFactorsList.size()>1) {
			Factor temp1, temp2, product;
			temp1 = tempFactorsList.back();
			tempFactorsList.pop_back();
			temp2 = tempFactorsList.back();
			tempFactorsList.pop_back();
			product = temp1.multiplyWithFactor(temp2);
			tempFactorsList.push_back(product);
		}
		Factor newFactor = tempFactorsList.back().sumProductOverVariable(nodePtr);
		factorsList.push_back(newFactor);
	}

	/*
	 * 	If we are calculating a node's posterior probability given evidence about its children,
	 * 	then when the program runs to here,
	 * 	the "factorsList" will contain several factors about the same node which is the query node Y.
	 * 	When it happens, we need to multiply these several factors.
	 */
	while (factorsList.size()>1) {
		Factor temp1, temp2, product;
		temp1 = factorsList.back();
		factorsList.pop_back();
		temp2 = factorsList.back();
		factorsList.pop_back();
		product = temp1.multiplyWithFactor(temp2);
		factorsList.push_back(product);
	}

	return factorsList.back();	// After all the processing shown above, the only remaining factor is the factor about Y.
}

Factor Network::variableEliminationInferenceReturningPossibilities(int* Z, int nz, Combination E, Node* Y) {
	vector<Factor> factorsList = constructFactors(Z, nz, Y);
	loadEvidence(&factorsList,E);
	Factor F = sumProductVariableElimination(factorsList, Z, nz);
	F.normalize();
	return F;
}

Factor Network::variableEliminationInferenceReturningPossibilities(Combination E, Node* Y) {
	return this->variableEliminationInferenceReturningPossibilities(treeDefaultEliminationOrder, numOfNodes-1, E, Y);
}



double Network::testingNetworkReturnAccuracy(Trainer* tester) {

	cout << "=======================================================================" << '\n'
		 << "Begin testing the trained network." << endl;

	int numOfCorrect=0, numOfWrong=0;

	for (int i=0; i<tester->numOfTrainingSamples; i++) {	// For each row i of testing set
		Node* Y = givenIndexToFindNodePointer(0);

		/* todo
		 *	This implementation is not good.
		 *	I know the final tree structure and CPD of the ChowLiu tree trained with dataset "a1a".
		 *	So I hard-coding the elimination order and evidences in program.
		 *	It should have been generated by algorithm!!!
		 *	It should have been generated by algorithm!!!
		 *	It should have been generated by algorithm!!!
		 */

		// These 5 nodes provide the most useful information.
		int nZ=5, *Z=new int[nZ]{39,40,51,75,76};
		int eNum=5, *eIndex=new int[eNum], *eValue=new int[eNum];
		for (int j=0; j<eNum; j++) {
			eIndex[j] = Z[j];
			eValue[j] = tester->trainingSet[i][eIndex[j]];
		}
		Combination E = constructEvidence(eIndex,eValue,eNum);
		Factor F = variableEliminationInferenceReturningPossibilities(Z,nZ,E, Y);
		double maxProb = 0;
		Combination combPredict;
		for (auto comb : F.combList) {
			if (F.potentialsList[comb] > maxProb) {
				maxProb = F.potentialsList[comb];
				combPredict = comb;
			}
		}

		if ((*combPredict.begin()).second == tester->trainingSet[i][0]) {
			numOfCorrect++;
			cout << "y, ";
		} else {
			numOfWrong++;
			cout << "n, ";
		}
	}
	double accuracy = numOfCorrect / (double)(numOfCorrect+numOfWrong);
	cout << '\n' << "Accuracy: " << accuracy << endl;
	return accuracy;
}