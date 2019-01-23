//
// Created by Linjian Li on 2018/11/29.
//

#include "Network.h"

Network::Network() {}

Node* Network::GivenIndexToFindNodePointer(int index) {
	if (index<0 || index>n_nodes) {
		cout << "=======================================================================" << '\n'
		     << "Node* Network::GivenIndexToFindNodePointer(int index) \n"
		     << "Invalid index!!! \n"
		     << "Index starts at 0 and the 0-th index is the root node's. " << endl;
		exit(1);
	}
	Node* node_ptr = nullptr;
	for (auto n_ptr : set_node_ptr_container) {
		if (n_ptr->GetNodeIndex()==index) {
			node_ptr = n_ptr;
			break;
		}
	}
	return node_ptr;
}

double Network::ComputeMutualInformation(Node *Xi, Node *Xj, const Trainer *trainer) {
	// Find the indexes of these two features in training set.
	int xi=Xi->GetNodeIndex(), xj=Xj->GetNodeIndex();

	// Initialize the table.
	int m = trainer->n_train_instance, ri = Xi->num_of_potential_values, rj = Xj->num_of_potential_values;
	double **Pij = new double* [ri];
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
				if (trainer->train_set_y_X[s][xi]==Xi->potential_values[a] && trainer->train_set_y_X[s][xj]==Xj->potential_values[b]) {
					Pij[a][b] += 1;
				}
			}
			Pij[a][b] /= m;
		}
	}

	// Update Pi.
	for (a=0; a<ri; a++) {
		for (s=0; s<m; s++) {
			if (trainer->train_set_y_X[s][xi]==Xi->potential_values[a]) {
				Pi[a] += 1;
			}
		}
		Pi[a] /= m;
	}

	// Update Pj.
	for (b=0; b<rj; b++) {
		for (s=0; s<m; s++) {
			if (trainer->train_set_y_X[s][xj]==Xj->potential_values[b]) {
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

void Network::SetParentChild(int p_index, int c_index) {
	Node *p = GivenIndexToFindNodePointer(p_index), *c = GivenIndexToFindNodePointer(c_index);
	SetParentChild(p,c);
}

void Network::SetParentChild(Node *p, Node *c) {
	p->AddChild(c);
	c->AddParent(p);
}

void Network::StructLearnChowLiuTreeCompData(Trainer *trainer) {
	cout << "=======================================================================" << '\n'
	     << "Begin structural learning. \nConstructing Chow-Liu tree with complete data......" << endl;
	n_nodes = trainer->n_feature+1;  // "+1" is for the label node.
	for (int i=0; i<n_nodes; ++i) {
		Node *node_ptr = new Node();
		node_ptr->SetNodeIndex(i);
		node_ptr->is_discrete = trainer->is_features_discrete[i];

		if (i==0) {
			node_ptr->num_of_potential_values = trainer->num_of_possible_values_of_label;
		} else {
			node_ptr->num_of_potential_values = trainer->num_of_possible_values_of_features[i];
		}

		node_ptr->potential_values = new int[node_ptr->num_of_potential_values];
		int j=0;
		if (i==0) {	// The 0-th node_ptr denotes the label node.
			for (auto v : trainer->set_label_possible_values) {
				node_ptr->potential_values[j++] = v;
			}
		} else {	// The other nodes are features.
			for (auto v : trainer->map_feature_possible_values[i]) {
				node_ptr->potential_values[j++] = v;
			}
		}

		set_node_ptr_container.insert(node_ptr);
	}

	cout << "=======================================================================" << '\n'
	     << "Constructing mutual information table......" << endl;

	int n = n_nodes;
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
				for (auto it=set_node_ptr_container.begin(); it!=set_node_ptr_container.end(); it++) {
					if ((*it)->GetNodeIndex()==i && Xi==nullptr) {
						Xi = *it;
					} else if ((*it)->GetNodeIndex()==j && Xj==nullptr) {
						Xj = *it;
					}
				}
				mutualInfoTab[i][j] = mutualInfoTab[j][i] = ComputeMutualInformation(Xi, Xj, trainer);	// Mutual information table is symmetric.
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
		graphAdjacencyMatrix[maxI][maxJ] = graphAdjacencyMatrix[maxJ][maxI] = 1;
	}
	// Add arrows in tree, set parents and childrens
	int* topologicalSortedPermutation = widthFirstTraversalWithAdjacencyMatrix(graphAdjacencyMatrix, n, 0);


	// !!! See the comments for "tree_default_elim_ord" in the "Network.h" file.
	tree_default_elim_ord = new int[n-1];
	for (int i=1; i<n; i++) {
		tree_default_elim_ord[i-1] = topologicalSortedPermutation[n-i];
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

				Node* Xi = GivenIndexToFindNodePointer(i);
				Node* Xj = GivenIndexToFindNodePointer(j);
				if (topoIndexI<topoIndexJ) {
					SetParentChild(Xi, Xj);
				} else {
					SetParentChild(Xj, Xi);
				}
			}
		}
	}

	cout << "=======================================================================" << '\n'
	     << "Generating parents combinations for each node......" << endl;
	for (auto n_ptr : set_node_ptr_container) {
		n_ptr->GenerateParentsCombinations();
	}

	cout << "=======================================================================" << '\n'
	     << "Finish structural learning." << endl;


	// The following code are just to print the result.

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

	cout << "=======================================================================" << '\n'
	     << "Topological sorted permutation generated using width-first-traversal: " << endl;
	for (int m = 0; m < n; ++m) {
		cout << topologicalSortedPermutation[m] << '\t';
	}
	cout << endl;

	cout << "=======================================================================" << '\n'
	     << "Each node's parents: " << endl;
	for (auto ptr_this_node : set_node_ptr_container) {
		cout << ptr_this_node->GetNodeIndex() << ":\t";
		for (auto ptr_par_node : ptr_this_node->set_parents_pointers) {
			cout << ptr_par_node->GetNodeIndex() << '\t';
		}
		cout << endl;
	}

	cout << "=======================================================================" << '\n'
	     << "Each node's children: " << endl;
	for (auto ptr_this_node : set_node_ptr_container) {
		cout << ptr_this_node->GetNodeIndex() << ":\t";
		for (auto ptr_child_node : ptr_this_node->set_children_pointers) {
			cout << ptr_child_node->GetNodeIndex() << '\t';
		}
		cout << endl;
	}

	cout << "=======================================================================" << '\n'
	     << "Each node's parents' combination: " << endl;
	for (auto ptr_this_node : set_node_ptr_container) {
		cout << ptr_this_node->GetNodeIndex() << ":\t";
		if (ptr_this_node->set_parents_combinations.empty()) continue;
		cout << "Num of par comb: " << ptr_this_node->set_parents_combinations.size() << endl;
		for (auto &comb : ptr_this_node->set_parents_combinations) {
			for (auto &p : comb) {
				cout << "(\"" << p.first << "\"=" << p.second << ")\t";
			}
			cout << "." << endl;
		}
		cout << endl;
	}

}

void Network::LearnParmsKnowStructCompData(const Trainer *trainer){
	cout << "=======================================================================" << '\n'
	     << "Begin learning parameters with known structure and complete data." << endl;

	// The 0-th node is the root which is the label node.
	Node *label_node = GivenIndexToFindNodePointer(0);
	map<int, double> *MPT = &(label_node->map_marg_prob_table);
	int denominator = 0;
	for (int s = 0; s < trainer->n_train_instance; ++s) {
		denominator += 1;
		int query = trainer->train_set_y[s];
		(*MPT)[query] += 1;
	}
	for (int i = 0; i < label_node->num_of_potential_values; ++i) {
		int query = label_node->potential_values[i];
		(*MPT)[query] /= denominator;
	}

	// For every feature node.
	for (int i=1; i<trainer->n_feature+1; ++i) { // Because feature index start at 1.
	                                                   // Using "train_set_y_X".
		Node *this_node = GivenIndexToFindNodePointer(i);

		map<int, map<Combination, double> >* CPT = &(this_node->map_cond_prob_table);
		set<Combination>* ptr_set_par_combs = &(this_node->set_parents_combinations);
		for (auto &par_comb : *ptr_set_par_combs) {		// For each column in CPT. Because the sum over column of CPT must be 1.
			int denominator = 0;
			for (int s=0; s<trainer->n_train_instance; ++s) {
				int compatibility = 1;	// We assume compatibility is 1,
				                       // and set it to 0 if we find that (*it_par_comb) is not compatible with (trainer->train_set[s]).
				                       // If we support learning with incomplete data,
				                       // the compatibility can be between 0 and 1.

				for (auto &pair_this_node : par_comb) {
					// int this_node_index = pair_this_node.first;
					// int this_node_value = pair_this_node.second;
					if (trainer->train_set_y_X[s][pair_this_node.first] != pair_this_node.second) {
						compatibility = 0;
						break;
					}
				}
				denominator += compatibility;
				int query = trainer->train_set_y_X[s][i];
				(*CPT)[query][par_comb] += compatibility;
			}
			// Normalize so that the sum is 1.
			for (int j=0; j<this_node->num_of_potential_values; ++j) {
				int query = this_node->potential_values[j];
				(*CPT)[query][par_comb] /= denominator;
			}
		}
	}
	cout << "=======================================================================" << '\n'
	     << "Finish training with known structure and complete data." << endl;

	// The following code are just to print the result.
	cout << "=======================================================================" << '\n'
	     << "Each node's conditional probability table: " << endl;
	for (auto thisNode : set_node_ptr_container) {	// For each node
		cout << thisNode->GetNodeIndex() << ":\t";


		if (thisNode->set_parents_pointers.empty()) {    // If this node has no parents
			for(int i=0; i<thisNode->num_of_potential_values; i++) {		// For each row of MPT
				int query = thisNode->potential_values[i];
				cout << "P(" << query << ")=" << thisNode->map_marg_prob_table[query] << '\t';
			}
			cout << endl;
			continue;
		}


		for(int i=0; i<thisNode->num_of_potential_values; ++i) {		// For each row of CPT
			int query = thisNode->potential_values[i];
			for (auto itParCom = thisNode->set_parents_combinations.begin(); itParCom != thisNode->set_parents_combinations.end(); ++itParCom) {	// For each column of CPT
				Combination comb = (*itParCom);
				string condition;
				for (auto &p : comb) {
					condition += ("\"" + to_string(p.first) + "\"=" + to_string(p.second));
				}
				cout << "P(" << query << '|' << condition << ")=" << thisNode->map_cond_prob_table[query][comb] << '\t';
			}
		}
		cout << endl;

	}
}

Combination Network::ConstructEvidence(int *nodes_indexes, int *observations, int num_of_observations) {
	Combination result;
	pair<int, int> p;
	for (int i=0; i<num_of_observations; i++) {
		p.first = nodes_indexes[i];
		p.second = observations[i];
		result.insert(p);
	}
	return result;
}

vector<Factor> Network::ConstructFactors(int *Z, int nz, Node *Y) {
	vector<Factor> factors_list;
	Factor factor;
	factor.ConstructFactor(Y);
	factors_list.push_back(factor);
	for (int i=0; i<nz; i++) {
		Node* n = GivenIndexToFindNodePointer(Z[i]);
		Factor factor;
		factor.ConstructFactor(n);
		factors_list.push_back(factor);
	}
	return factors_list;
}

void Network::LoadEvidence(vector<Factor> *factors_list, Combination E) {
	for (auto &f : *factors_list) {	// For each factor
		for (auto &p : E) {	// For each node's observation in E
			if (f.related_variables.find(p.first)!=f.related_variables.end()) {	// If this factor is related to this node
				for (auto &comb : f.set_combinations) {	// Update each row of map_potentials
					// If (*itCL) is not compatible with (*itE), set map_potentials[*itCL]=0.
					if (comb.find(p)==comb.end()) {
						f.map_potentials[comb] = 0;
					}
				}
			}
		}
	}
}

pair<int*, int> Network::SimplifyTreeDefaultElimOrd() {

	// Remove all the barren nodes
	set<int> to_be_removed;
	for (int i=0; i<n_nodes-1; ++i) {    // The 0-th node is root.
		bool observed = false, need_to_be_removed = true;
		Node *ptr_curr_node = GivenIndexToFindNodePointer(i);
		for (auto p : network_evidence) {
			if (p.first == tree_default_elim_ord[i]) {    // If it is observed.
				observed = true;
				break;
			}
		}
		if (observed) continue;

		if (!ptr_curr_node->set_children_pointers.empty()) {        // If it is not a leaf.
			for (auto ptr_child : ptr_curr_node->set_children_pointers) {
				if (to_be_removed.find(ptr_child->GetNodeIndex()) != to_be_removed.end()) {
					need_to_be_removed = false;        // And if its children are not all removed.
					break;
				}
			}
		}
		if (need_to_be_removed) to_be_removed.insert(i);
	}

	// Remove all m-separated nodes.
	set<int> visited;
	DepthFirstTraversalUntillMeetObserved(0,visited,to_be_removed);	// Start at root.

	// Record all the remaining nodes in array "simplified_order".
	int num_of_remain = n_nodes-1-to_be_removed.size();		// The 0-th node is root and do not need to be eliminated.
	int* simplified_order = new int[num_of_remain];
	for (int i=0, j=1; i<num_of_remain; ++i) {		// j=1 because the 0-th node is root.
		while (to_be_removed.find(j)!=to_be_removed.end()) {++j;}
		simplified_order[i] = j++;	// "j++" is to move to the next j, or else it will stuck at the first j that is not in "to_be_removed".
	}


	pair<int*, int> simplified_order_and_nodes_number = make_pair(simplified_order, num_of_remain);
	return simplified_order_and_nodes_number;
}

void Network::DepthFirstTraversalUntillMeetObserved(int start, set<int>& visited, set<int>& to_be_removed) {

	// Base case
	if (visited.find(start)!=visited.end()) return;
	visited.insert(start);

	bool observed=false;
	auto att = network_evidence.begin();
	for (auto p : network_evidence) {
		if (p.first == start) {	// If it is observed.
			observed = true;
			break;
		}
	}
	if (observed) {
		DepthFirstTraversalToRemoveMSeparatedNodes(start, visited, to_be_removed); // Cut down all the descendent.
		return;
	}

	// If not observed
	// Recursive case
	Node* ptr_curr_node = GivenIndexToFindNodePointer(start);
	for (auto ptr_child : ptr_curr_node->set_children_pointers) {
		int child_index = ptr_child->GetNodeIndex();
		DepthFirstTraversalUntillMeetObserved(child_index, visited, to_be_removed);
	}

}

void Network::DepthFirstTraversalToRemoveMSeparatedNodes(int start, set<int>& visited, set<int>& to_be_removed) {
	visited.insert(start);
	Node* ptr_curr_node = GivenIndexToFindNodePointer(start);
	for (auto it_ptr_child=ptr_curr_node->set_children_pointers.begin();
	     it_ptr_child!=ptr_curr_node->set_children_pointers.end() && visited.find((*it_ptr_child)->GetNodeIndex())==visited.end();
	     ++it_ptr_child) {
		to_be_removed.insert((*it_ptr_child)->GetNodeIndex());
		DepthFirstTraversalToRemoveMSeparatedNodes((*it_ptr_child)->GetNodeIndex(), visited, to_be_removed);
	}

}

Factor Network::SumProductVarElim(vector<Factor> factors_list, int *Z, int nz) {
	for (int i=0; i<nz; i++) {
		vector<Factor> tempFactorsList;
		Node* nodePtr = GivenIndexToFindNodePointer(Z[i]);
		// Move every factor that is related to the node Z[i] from factors_list to tempFactorsList.
		/*
		 * Note: This for loop does not contain "it++" in the parentheses.
		 * 		 Because if we do so, it may cause some logic faults which, however, will not cause runtime error, so hard to debug.
		 * 		 For example:
		 * 		 	When "it" reaches the second to last element, and this element is related to the node.
		 * 		 	Then this element will be erase from factors_list, and then "it++" which will move "it" to the end.
		 * 		 	Then the for loop will end because "it" has reached the end.
		 * 		 	However, at this time, the last element has been ignored, even if it is related to the node.
		 */
		for (auto it=factors_list.begin(); it!=factors_list.end(); /* no ++it */) {
			if ((*it).related_variables.find(nodePtr->GetNodeIndex())!=(*it).related_variables.end()) {
				tempFactorsList.push_back(*it);
				factors_list.erase(it);
				continue;
			} else {
				++it;
			}
		}
		while(tempFactorsList.size()>1) {
			Factor temp1, temp2, product;
			temp1 = tempFactorsList.back();
			tempFactorsList.pop_back();
			temp2 = tempFactorsList.back();
			tempFactorsList.pop_back();
			product = temp1.MultiplyWithFactor(temp2);
			tempFactorsList.push_back(product);
		}
		Factor newFactor = tempFactorsList.back().SumProductOverVariable(nodePtr);
		factors_list.push_back(newFactor);
	}

	/*
	 * 	If we are calculating a node's posterior probability given evidence about its children,
	 * 	then when the program runs to here,
	 * 	the "factors_list" will contain several factors about the same node which is the query node Y.
	 * 	When it happens, we need to multiply these several factors.
	 */
	while (factors_list.size()>1) {
		Factor temp1, temp2, product;
		temp1 = factors_list.back();
		factors_list.pop_back();
		temp2 = factors_list.back();
		factors_list.pop_back();
		product = temp1.MultiplyWithFactor(temp2);
		factors_list.push_back(product);
	}

	return factors_list.back();	// After all the processing shown above, the only remaining factor is the factor about Y.
}

Factor Network::VarElimInferReturnPossib(int *Z, int nz, Combination E, Node *Y) {
	vector<Factor> factorsList = ConstructFactors(Z, nz, Y);
	LoadEvidence(&factorsList, E);
	Factor F = SumProductVarElim(factorsList, Z, nz);
	F.Normalize();
	return F;
}

Factor Network::VarElimInferReturnPossib(Combination E, Node *Y) {
	pair<int*, int> simplified_elimination_order = SimplifyTreeDefaultElimOrd();
	return this->VarElimInferReturnPossib(simplified_elimination_order.first, simplified_elimination_order.second, E, Y);
}

double Network::TestNetReturnAccuracy(Trainer *tester) {

	cout << "=======================================================================" << '\n'
	     << "Begin testing the trained network." << endl;

	cout << "Progress indicator: ";

	int num_of_correct=0, num_of_wrong=0, m=tester->n_train_instance, m10=m/10, percent=0;

	for (int i=0; i<m; i++) {	// For each sample in test set

		if (i%m10==0) {
			cout << (percent++)*10 << "%... " << flush;
		}

		Node *Y = GivenIndexToFindNodePointer(0);

		// For now, only support complete data.
		int e_num=n_nodes-1, *e_index=new int[e_num], *e_value=new int[e_num];
		for (int j=0; j<e_num; ++j) {
			e_index[j] = j+1;
			e_value[j] = tester->train_set_X[i][j];
		}
		Combination E = ConstructEvidence(e_index, e_value, e_num);
		this->network_evidence = E;
		Factor F = VarElimInferReturnPossib(E, Y);
		double max_prob = 0;
		Combination comb_predict;
		for (auto &comb : F.set_combinations) {
			if (F.map_potentials[comb] > max_prob) {
				max_prob = F.map_potentials[comb];
				comb_predict = comb;
			}
		}
		int label_predict = comb_predict.begin()->second;

		if (label_predict == tester->train_set_y[i]) {
			num_of_correct++;
		} else {
			num_of_wrong++;
		}

	}
	double accuracy = num_of_correct / (double)(num_of_correct+num_of_wrong);
	cout << '\n' << "Accuracy: " << accuracy << endl;
	return accuracy;
}
