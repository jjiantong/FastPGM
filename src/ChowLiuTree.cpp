//
// Created by LinjianLi on 2019/1/23.
//

#include "ChowLiuTree.h"


double ChowLiuTree::ComputeMutualInformation(Node *Xi, Node *Xj, const Trainer *trainer) {
  // Find the indexes of these two features in training set.
  int xi=Xi->GetNodeIndex(), xj=Xj->GetNodeIndex();

  // Initialize the table.
  int m = trainer->n_train_instance, ri = Xi->num_potential_vals, rj = Xj->num_potential_vals;
  double **Pij = new double* [ri];
  for (int i=0; i<ri; i++) {
    Pij[i] = new double[rj]();    // The parentheses at end will initialize the array to be all zeros.
  }
  double* Pi = new double[ri]();
  double* Pj = new double[rj]();
  int a, b, s;

  // Update Pij.
  for (a=0; a<ri; a++) {
    for (b=0; b<rj; b++) {
      for (s=0; s<m; s++) {
        if (trainer->train_set_y_X[s][xi]==Xi->potential_vals[a] && trainer->train_set_y_X[s][xj]==Xj->potential_vals[b]) {
          Pij[a][b] += 1;
        }
      }
      Pij[a][b] /= m;
    }
  }

  // Update Pi.
  for (a=0; a<ri; a++) {
    for (s=0; s<m; s++) {
      if (trainer->train_set_y_X[s][xi]==Xi->potential_vals[a]) {
        Pi[a] += 1;
      }
    }
    Pi[a] /= m;
  }

  // Update Pj.
  for (b=0; b<rj; b++) {
    for (s=0; s<m; s++) {
      if (trainer->train_set_y_X[s][xj]==Xj->potential_vals[b]) {
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
      if (!isnan(temp)) {    // There may be runtime error of divided by zero.
        mutualInformation += temp;
      }
    }
  }

  // Return
  return mutualInformation;
}


void ChowLiuTree::StructLearnCompData(Trainer *trainer) {
  StructLearnChowLiuTreeCompData(trainer);
}


void ChowLiuTree::StructLearnChowLiuTreeCompData(Trainer *trainer) {
  cout << "=======================================================================" << '\n'
       << "Begin structural learning. \nConstructing Chow-Liu tree with complete data......" << endl;
  n_nodes = trainer->n_feature+1;  // "+1" is for the label node.
  for (int i=0; i<n_nodes; ++i) {
    Node *node_ptr = new Node();
    node_ptr->SetNodeIndex(i);
    node_ptr->is_discrete = trainer->is_features_discrete[i];

    if (i==0) {
      node_ptr->num_potential_vals = trainer->num_of_possible_values_of_label;
    } else {
      node_ptr->num_potential_vals = trainer->num_of_possible_values_of_features[i];
    }

    node_ptr->potential_vals = new int[node_ptr->num_potential_vals];
    int j=0;
    if (i==0) {  // The 0-th node_ptr denotes the label node.
      for (auto v : trainer->set_label_possible_values) {
        node_ptr->potential_vals[j++] = v;
      }
    } else {  // The other nodes are features.
      for (auto v : trainer->map_feature_possible_values[i]) {
        node_ptr->potential_vals[j++] = v;
      }
    }

    set_node_ptr_container.insert(node_ptr);
  }

  cout << "=======================================================================" << '\n'
       << "Constructing mutual information table......" << endl;

  int n = n_nodes;
  double** mutualInfoTab = new double* [n];
  for (int i=0; i<n; i++) {
    mutualInfoTab[i] = new double[n]();    // The parentheses at end will initialize the array to be all zeros.
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
        mutualInfoTab[i][j] = mutualInfoTab[j][i] = ComputeMutualInformation(Xi, Xj, trainer);  // Mutual information table is symmetric.
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
  markSet.insert(0);  // The node of index 0 is the root node, which is the class variable.
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
  int* topologicalSortedPermutation = WidthFirstTraversalWithAdjacencyMatrix(graphAdjacencyMatrix, n, 0);


  // !!! See the comments for "default_elim_ord" in the "ChowLiuTree.h" file.
  default_elim_ord = new int[n-1];
  for (int i=1; i<n; i++) {
    default_elim_ord[i-1] = topologicalSortedPermutation[n-i];
  }


  cout << "=======================================================================" << '\n'
       << "Setting children and parents......" << endl;
  for (int i=0; i<n; i++) {
    for (int j=0; j<i; j++) {  // graphAdjacencyMatrix is symmetric, so loop while j<i instead of j<n
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

        if (topoIndexI<topoIndexJ) {
          SetParentChild(i, j);
        } else {
          SetParentChild(j, i);
        }
      }
    }
  }

  cout << "=======================================================================" << '\n'
       << "Generating parents combinations for each node......" << endl;
  for (auto n_ptr : set_node_ptr_container) {
    n_ptr->GenParCombs();
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
    for (auto ptr_par_node : ptr_this_node->set_parents_ptrs) {
      cout << ptr_par_node->GetNodeIndex() << '\t';
    }
    cout << endl;
  }

  cout << "=======================================================================" << '\n'
       << "Each node's children: " << endl;
  for (auto ptr_this_node : set_node_ptr_container) {
    cout << ptr_this_node->GetNodeIndex() << ":\t";
    for (auto ptr_child_node : ptr_this_node->set_children_ptrs) {
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


pair<int*, int> ChowLiuTree::SimplifyDefaultElimOrd() {
  return SimplifyTreeDefaultElimOrd();
}


pair<int*, int> ChowLiuTree::SimplifyTreeDefaultElimOrd() {

  // Remove all the barren nodes
  set<int> to_be_removed;
  for (int i=0; i<n_nodes-1; ++i) {    // The 0-th node is root.
    bool observed = false, need_to_be_removed = true;
    Node *ptr_curr_node = GivenIndexToFindNodePointer(i);
    for (auto p : network_evidence) {
      if (p.first == default_elim_ord[i]) {    // If it is observed.
        observed = true;
        break;
      }
    }
    if (observed) continue;

    if (!ptr_curr_node->set_children_ptrs.empty()) {        // If it is not a leaf.
      for (auto ptr_child : ptr_curr_node->set_children_ptrs) {
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
  DepthFirstTraversalUntillMeetObserved(0,visited,to_be_removed);  // Start at root.

  // Record all the remaining nodes in array "simplified_order".
  int num_of_remain = n_nodes-1-to_be_removed.size();    // The 0-th node is root and do not need to be eliminated.
  int* simplified_order = new int[num_of_remain];
  for (int i=0, j=1; i<num_of_remain; ++i) {    // j=1 because the 0-th node is root.
    while (to_be_removed.find(j)!=to_be_removed.end()) {++j;}
    simplified_order[i] = j++;  // "j++" is to move to the next j, or else it will stuck at the first j that is not in "to_be_removed".
  }


  pair<int*, int> simplified_order_and_nodes_number = make_pair(simplified_order, num_of_remain);
  return simplified_order_and_nodes_number;
}


void ChowLiuTree::DepthFirstTraversalUntillMeetObserved(int start, set<int>& visited, set<int>& to_be_removed) {

  // Base case
  if (visited.find(start)!=visited.end()) return;
  visited.insert(start);

  bool observed=false;
  auto att = network_evidence.begin();
  for (auto p : network_evidence) {
    if (p.first == start) {  // If it is observed.
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
  for (auto ptr_child : ptr_curr_node->set_children_ptrs) {
    int child_index = ptr_child->GetNodeIndex();
    DepthFirstTraversalUntillMeetObserved(child_index, visited, to_be_removed);
  }

}


void ChowLiuTree::DepthFirstTraversalToRemoveMSeparatedNodes(int start, set<int>& visited, set<int>& to_be_removed) {
  visited.insert(start);
  Node* ptr_curr_node = GivenIndexToFindNodePointer(start);
  for (auto it_ptr_child=ptr_curr_node->set_children_ptrs.begin();
       it_ptr_child!=ptr_curr_node->set_children_ptrs.end() && visited.find((*it_ptr_child)->GetNodeIndex())==visited.end();
       ++it_ptr_child) {
    to_be_removed.insert((*it_ptr_child)->GetNodeIndex());
    DepthFirstTraversalToRemoveMSeparatedNodes((*it_ptr_child)->GetNodeIndex(), visited, to_be_removed);
  }

}
