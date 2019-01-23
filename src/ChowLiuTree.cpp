//
// Created by LinjianLi on 2019/1/23.
//

#include "ChowLiuTree.h"

double ChowLiuTree::ComputeMutualInformation(Node *Xi, Node *Xj, const Trainer *trainer) {
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

void ChowLiuTree::StructLearnChowLiuTreeCompData(Trainer *trainer, Network *net) {
	cout << "=======================================================================" << '\n'
	     << "Begin structural learning. \nConstructing Chow-Liu tree with complete data......" << endl;
	net->n_nodes = trainer->n_feature+1;  // "+1" is for the label node.
	for (int i=0; i<net->n_nodes; ++i) {
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

		net->set_node_ptr_container.insert(node_ptr);
	}

	cout << "=======================================================================" << '\n'
	     << "Constructing mutual information table......" << endl;

	int n = net->n_nodes;
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
				for (auto it=net->set_node_ptr_container.begin(); it!=net->set_node_ptr_container.end(); it++) {
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
	net->tree_default_elim_ord = new int[n-1];
	for (int i=1; i<n; i++) {
		net->tree_default_elim_ord[i-1] = topologicalSortedPermutation[n-i];
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

				Node* Xi = net->GivenIndexToFindNodePointer(i);
				Node* Xj = net->GivenIndexToFindNodePointer(j);
				if (topoIndexI<topoIndexJ) {
					net->SetParentChild(Xi, Xj);
				} else {
					net->SetParentChild(Xj, Xi);
				}
			}
		}
	}

	cout << "=======================================================================" << '\n'
	     << "Generating parents combinations for each node......" << endl;
	for (auto n_ptr : net->set_node_ptr_container) {
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
	for (auto ptr_this_node : net->set_node_ptr_container) {
		cout << ptr_this_node->GetNodeIndex() << ":\t";
		for (auto ptr_par_node : ptr_this_node->set_parents_pointers) {
			cout << ptr_par_node->GetNodeIndex() << '\t';
		}
		cout << endl;
	}

	cout << "=======================================================================" << '\n'
	     << "Each node's children: " << endl;
	for (auto ptr_this_node : net->set_node_ptr_container) {
		cout << ptr_this_node->GetNodeIndex() << ":\t";
		for (auto ptr_child_node : ptr_this_node->set_children_pointers) {
			cout << ptr_child_node->GetNodeIndex() << '\t';
		}
		cout << endl;
	}

	cout << "=======================================================================" << '\n'
	     << "Each node's parents' combination: " << endl;
	for (auto ptr_this_node : net->set_node_ptr_container) {
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