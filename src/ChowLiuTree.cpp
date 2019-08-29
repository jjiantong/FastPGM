//
// Created by LinjianLi on 2019/1/23.
//

#include "ChowLiuTree.h"

double ChowLiuTree::ComputeMutualInformation(Node *Xi, Node *Xj, const Trainer *trainer) {
  // Find the indexes of these two features in training set.
  int xi=Xi->GetNodeIndex(), xj=Xj->GetNodeIndex();

  // Initialize the table.
  int m = trainer->num_train_instance, ri = Xi->num_potential_vals, rj = Xj->num_potential_vals;
  double **Pij = new double* [ri];
  for (int i=0; i<ri; ++i) {
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

  delete[] Pi;
  delete[] Pj;
  for (int i=0; i<ri; ++i) {
    delete[] Pij[i];
  }
  delete[] Pij;

  // Return
  return mutualInformation;
}


void ChowLiuTree::StructLearnCompData(Trainer *trainer) {
  struct timeval start, end;
  double diff;
  gettimeofday(&start,NULL);

  StructLearnChowLiuTreeCompData(trainer);

  gettimeofday(&end,NULL);
  diff = (end.tv_sec-start.tv_sec) + ((double)(end.tv_usec-start.tv_usec))/1.0E6;
  setlocale(LC_NUMERIC, "");
  cout << "==================================================" << '\n'
       << "The time spent to construct Chow-Liu tree is " << diff << " seconds" << endl;
}


void ChowLiuTree::StructLearnChowLiuTreeCompData(Trainer *trainer) {
  cout << "==================================================" << '\n'
       << "Begin structural learning. \nConstructing Chow-Liu tree with complete data......" << endl;

  num_nodes = trainer->num_vars;
  // Assign an index for each node.
  #pragma omp parallel for
  for (int i=0; i<num_nodes; ++i) {
    Node *node_ptr = new DiscreteNode();
    node_ptr->SetNodeIndex(i);
    node_ptr->is_discrete = true;  // trainer->is_features_discrete[i];

    if (i == 0) {   // The 0-th node_ptr denotes the label node.
      node_ptr->num_potential_vals = trainer->num_of_possible_values_of_label;
    } else {
      // Number of features is one less than number of nodes.
      node_ptr->num_potential_vals = trainer->num_of_possible_values_of_features[i-1];
    }

    node_ptr->potential_vals = new int[node_ptr->num_potential_vals];
    if (i == 0) {  // The 0-th node_ptr denotes the label node.
      int j = 0;
      for (auto v : trainer->set_label_possible_values) {
        node_ptr->potential_vals[j++] = v;
      }
    } else {  // The other nodes are features.
      int j = 0;
      for (auto v : trainer->map_feature_possible_values[i]) {
        node_ptr->potential_vals[j++] = v;
      }
    }
    #pragma omp critical
    {
      set_node_ptr_container.insert(node_ptr);
    }
  }

  cout << "==================================================" << '\n'
       << "Constructing mutual information table......" << endl;

  int n = num_nodes;
  double** mutualInfoTab = new double* [n];
  for (int i=0; i<n; ++i) {
    // The parentheses at end will initialize the array to be all zeros.
    mutualInfoTab[i] = new double[n]();
  }

  // Update the mutual information table.
  #pragma omp parallel for
  for (int i=0; i<n; ++i) {
    for (int j=0; j<i; ++j) {
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
          if (Xi!=nullptr && Xj!=nullptr) { break; }
        }
        // Mutual information table is symmetric.
        mutualInfoTab[i][j] = ComputeMutualInformation(Xi, Xj, trainer);
        mutualInfoTab[j][i] = ComputeMutualInformation(Xi, Xj, trainer);
      }
    }
  }

  cout << "==================================================" << '\n'
       << "Constructing maximum spanning tree using mutual information table and Prim's algorithm......" << endl;

  // Use Prim's algorithm to generate a spanning tree.
  int** graphAdjacencyMatrix = new int* [n];
  for (int i=0; i<n; ++i) {
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
      for (int j=0; j<n; ++j) {
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
  // Add arrows in tree, set parents and children
  int* topologicalSortedPermutation = WidthFirstTraversalWithAdjacencyMatrix(graphAdjacencyMatrix, n, 0);


  // !!! See the comments for "default_elim_ord" in the "ChowLiuTree.h" file.
  default_elim_ord = new int[n-1];
  for (int i=1; i<n; ++i) {
    default_elim_ord[i-1] = topologicalSortedPermutation[n-i];
  }
  for (int i=0; i<n; ++i) {
    topo_ord.push_back(topologicalSortedPermutation[i]);
  }


  cout << "==================================================" << '\n'
       << "Setting children and parents......" << endl;
  #pragma omp parallel for
  for (int i=0; i<n; ++i) {
    for (int j=0; j<i; ++j) {  // graphAdjacencyMatrix is symmetric, so loop while j<i instead of j<n
      if (i==j) continue;
      if (graphAdjacencyMatrix[i][j]==1){

        // Determine the topological position of i and j.
        int topoIndexI=-1, topoIndexJ=-1;
        for (int k=0; k<n; ++k) {
          if (topologicalSortedPermutation[k]==i && topoIndexI==-1) {
            topoIndexI = k;
          } else if (topologicalSortedPermutation[k]==j && topoIndexJ==-1) {
            topoIndexJ = k;
          }
          if (topoIndexI!=-1 && topoIndexJ!=-1) { break; }
        }

        if (topoIndexI<topoIndexJ) {
          SetParentChild(i, j);
        } else {
          SetParentChild(j, i);
        }
      }
    }
  }

  cout << "==================================================" << '\n'
       << "Generating parents combinations for each node......" << endl;

  // Store the pointers in an array to make use of OpenMP.
  Node** arr_node_ptr_container = new Node*[num_nodes];
  auto iter_n_ptr = set_node_ptr_container.begin();
  for (int i=0; i<num_nodes; ++i) {
    arr_node_ptr_container[i] = *(iter_n_ptr++);
  }
  #pragma omp parallel for
  for (int i=0; i<num_nodes; ++i) {
    arr_node_ptr_container[i]->GenDiscParCombs();
  }
  delete[] arr_node_ptr_container;


  cout << "==================================================" << '\n'
       << "Finish structural learning." << endl;


  // The following code are just to print the result.

//  cout << "==================================================" << '\n'
//       << "The Chow-Liu Tree has the following edges (adjacency matrix): " << endl;
//  for (int l = 0; l < n; ++l) {
//    for (int j = 0; j < n; ++j) {
//      if (graphAdjacencyMatrix[l][j]==1) {
//        cout << '<' << l << ',' << j << '>' << '\t';
//      }
//    }
//    cout << endl;
//  }

  cout << "==================================================" << '\n'
       << "Topological sorted permutation generated using width-first-traversal: " << endl;
  for (int m = 0; m < n; ++m) {
    cout << topologicalSortedPermutation[m] << '\t';
  }
  cout << endl;

  cout << "==================================================" << '\n'
       << "Each node's parents: " << endl;
  this->PrintEachNodeParents();

//  cout << "==================================================" << '\n'
//       << "Each node's children: " << endl;
//  this->PrintEachNodeChildren();
//

  for (int i=0; i<n; ++i) {
    delete[] mutualInfoTab[i];
  }
  delete[] mutualInfoTab;
  for (int i=0; i<n; ++i) {
    delete[] graphAdjacencyMatrix[i];
  }
  delete[] graphAdjacencyMatrix;
  delete[] topologicalSortedPermutation;
}


void ChowLiuTree::LearnParmsKnowStructCompData(const Trainer *trainer){
  cout << "==================================================" << '\n'
       << "Begin learning parameters with known structure and complete data." << endl;

  struct timeval start, end;
  double diff;
  gettimeofday(&start,NULL);

  // The 0-th node is the root which is the label node.
  Node *label_node = FindNodePtrByIndex(0);
  map<int, double> *MPT = &(label_node->map_marg_prob_table);
  int denominator = 0;
  for (int s = 0; s < trainer->num_train_instance; ++s) {
    denominator += 1;
    int query = trainer->train_set_y[s];
    (*MPT)[query] += 1;
  }
  for (int i = 0; i < label_node->num_potential_vals; ++i) {
    int query = label_node->potential_vals[i];
    (*MPT)[query] /= denominator;
  }

  int num_cores = omp_get_num_procs();
  omp_set_num_threads(num_cores);
  int max_work_per_thread = (trainer->num_vars-1+num_cores-1)/num_cores;
  #pragma omp parallel
  {
    // For every feature node.
    for (int i =  max_work_per_thread*omp_get_thread_num()+1;  // Because feature index start at 1 using "train_set_y_X".
         i < max_work_per_thread*(omp_get_thread_num()+1)+1 && i < trainer->num_vars;
         ++i) {
//    for (int i=1; i<trainer->num_vars; ++i) {

      Node *this_node = FindNodePtrByIndex(i);

      map<int, map<Combination, double> > *CPT = &(this_node->map_cond_prob_table);
      set<Combination> *ptr_set_par_combs = &(this_node->set_discrete_parents_combinations);
      for (auto &par_comb : *ptr_set_par_combs) {    // For each column in CPT. Because the sum over column of CPT must be 1.
        int denominator = 0;
        for (int s = 0; s < trainer->num_train_instance; ++s) {
          int compatibility = 1;  // We assume compatibility is 1,
          // and set it to 0 if we find that (*it_par_comb) is not compatible with (trainer->train_set[s]).
          // If we support learning with incomplete data,
          // the compatibility can be between 0 and 1.

          for (const auto &pair_this_node : par_comb) {
            // pair.first is the index and pair.second is the value
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
        for (int j = 0; j < this_node->num_potential_vals; ++j) {
          int query = this_node->potential_vals[j];
          (*CPT)[query][par_comb] /= denominator;
        }
      }
    }
  }   // end of: #pragma omp parallel
  cout << "==================================================" << '\n'
       << "Finish training with known structure and complete data." << endl;



  // The following code are just to print the result.
  cout << "==================================================" << '\n'
       << "Each node's conditional probability table: " << endl;
  for (const auto thisNode : set_node_ptr_container) {  // For each node
    cout << thisNode->GetNodeIndex() << ":\t";


    if (thisNode->set_parents_ptrs.empty()) {    // If this node has no parents
      for(int i=0; i<thisNode->num_potential_vals; ++i) {    // For each row of MPT
        int query = thisNode->potential_vals[i];
        cout << "P(" << query << ")=" << thisNode->map_marg_prob_table[query] << '\t';
      }
      cout << endl;
      continue;
    }


    for(int i=0; i<thisNode->num_potential_vals; ++i) {    // For each row of CPT
      int query = thisNode->potential_vals[i];
      for (auto itParCom = thisNode->set_discrete_parents_combinations.begin(); itParCom != thisNode->set_discrete_parents_combinations.end(); ++itParCom) {  // For each column of CPT
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

  gettimeofday(&end,NULL);
  diff = (end.tv_sec-start.tv_sec) + ((double)(end.tv_usec-start.tv_usec))/1.0E6;
  setlocale(LC_NUMERIC, "");
  cout << "==================================================" << '\n'
       << "The time spent to learn the parameters is " << diff << " seconds" << endl;
}


pair<int*, int> ChowLiuTree::SimplifyDefaultElimOrd(Combination evidence) {
  return SimplifyTreeDefaultElimOrd(evidence);
}


pair<int*, int> ChowLiuTree::SimplifyTreeDefaultElimOrd(Combination evidence) {

  // Remove all the barren nodes
  set<int> to_be_removed;
  for (int i=0; i<num_nodes-1; ++i) {
    Node *ptr_curr_node = FindNodePtrByIndex(default_elim_ord[i]);
    bool observed = false, need_to_be_removed = true;
    for (auto p : evidence) {
      if (p.first == ptr_curr_node->GetNodeIndex()) {    // If it is observed.
        observed = true;
        break;
      }
    }
    if (observed) continue;

    // If it is not a leaf.
    if (!ptr_curr_node->set_children_ptrs.empty()) {
      for (auto ptr_child : ptr_curr_node->set_children_ptrs) {
        // And if its children are not all removed.
        if (to_be_removed.find(ptr_child->GetNodeIndex()) == to_be_removed.end()) {
          need_to_be_removed = false;
          break;
        }
      }
    }
    if (need_to_be_removed) { to_be_removed.insert(ptr_curr_node->GetNodeIndex()); }
  }

  // Remove all m-separated nodes.
  set<int> visited;
  DepthFirstTraversalUntillMeetObserved(evidence, 0,visited,to_be_removed);  // Start at root.

  // Record all the remaining nodes in array "simplified_order".
  int num_of_remain = num_nodes-1-to_be_removed.size();    // The 0-th node is root and do not need to be eliminated.
  int* simplified_order = new int[num_of_remain];
  for (int i=0, j=1; i<num_of_remain; ++i) {    // j=1 because the 0-th node is root.
    while (to_be_removed.find(j)!=to_be_removed.end()) { ++j; }
    simplified_order[i] = j++;  // "j++" is to move to the next j, or else it will stuck at the first j that is not in "to_be_removed".
  }


  pair<int*, int> simplified_order_and_nodes_number = make_pair(simplified_order, num_of_remain);
//  delete[] simplified_order;
  return simplified_order_and_nodes_number;
}


void ChowLiuTree::DepthFirstTraversalUntillMeetObserved(Combination evidence, int start, set<int>& visited, set<int>& to_be_removed) {

  // Base case
  if (visited.find(start)!=visited.end()) return;
  visited.insert(start);

  bool observed=false;
  for (auto p : evidence) {
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
  Node* ptr_curr_node = FindNodePtrByIndex(start);
  for (auto ptr_child : ptr_curr_node->set_children_ptrs) {
    int child_index = ptr_child->GetNodeIndex();
    DepthFirstTraversalUntillMeetObserved(evidence, child_index, visited, to_be_removed);
  }

}


void ChowLiuTree::DepthFirstTraversalToRemoveMSeparatedNodes(int start, set<int>& visited, set<int>& to_be_removed) {
  visited.insert(start);
  Node* ptr_curr_node = FindNodePtrByIndex(start);
  for (auto it_ptr_child=ptr_curr_node->set_children_ptrs.begin();
       it_ptr_child!=ptr_curr_node->set_children_ptrs.end() && visited.find((*it_ptr_child)->GetNodeIndex())==visited.end();
       ++it_ptr_child) {
    to_be_removed.insert((*it_ptr_child)->GetNodeIndex());
    DepthFirstTraversalToRemoveMSeparatedNodes((*it_ptr_child)->GetNodeIndex(), visited, to_be_removed);
  }

}
