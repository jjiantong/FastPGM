#pragma clang diagnostic push
#pragma ide diagnostic ignored "openmp-use-default-none"
//
// Created by LinjianLi on 2019/1/23.
//

#include "ChowLiuTree.h"

ChowLiuTree::ChowLiuTree(): ChowLiuTree(true) {}

ChowLiuTree::ChowLiuTree(bool pure_disc) {
  this->pure_discrete = pure_disc;
}

double ChowLiuTree::ComputeMutualInformation(Node *Xi, Node *Xj, const Dataset *dts) {
  // Find the indexes of these two features in training set.
  int xi=Xi->GetNodeIndex(), xj=Xj->GetNodeIndex();

  auto dXi = dynamic_cast<DiscreteNode*>(Xi);
  auto dXj = dynamic_cast<DiscreteNode*>(Xj);

  // Initialize the table.
  int m = dts->num_instance, ri = dXi->num_potential_vals, rj = dXj->num_potential_vals;
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
        if (dts->dataset_all_vars[s][xi] == dXi->potential_vals[a] && dts->dataset_all_vars[s][xj] == dXj->potential_vals[b]) {
          Pij[a][b] += 1;
        }
      }
      Pij[a][b] /= m;
    }
  }

  // Update Pi.
  for (a=0; a<ri; a++) {
    for (s=0; s<m; s++) {
      if (dts->dataset_all_vars[s][xi] == dXi->potential_vals[a]) {
        Pi[a] += 1;
      }
    }
    Pi[a] /= m;
  }

  // Update Pj.
  for (b=0; b<rj; b++) {
    for (s=0; s<m; s++) {
      if (dts->dataset_all_vars[s][xj] == dXj->potential_vals[b]) {
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

  return mutualInformation;
}


void ChowLiuTree::StructLearnCompData(Dataset *dts, bool print_struct, string topo_ord_constraint) {
  struct timeval start, end;
  double diff;
  gettimeofday(&start,NULL);

  fprintf(stdout, "In function [%s]:\nChow-Liu tree will not be restricted by the argument \"topo_ord_constraint\".", __FUNCTION__);

  StructLearnChowLiuTreeCompData(dts, print_struct);

  gettimeofday(&end,NULL);
  diff = (end.tv_sec-start.tv_sec) + ((double)(end.tv_usec-start.tv_usec))/1.0E6;
  setlocale(LC_NUMERIC, "");
  cout << "==================================================" << '\n'
       << "The time spent to construct Chow-Liu tree is " << diff << " seconds" << endl;
}


void ChowLiuTree::StructLearnChowLiuTreeCompData(Dataset *dts, bool print_struct) {
  cout << "==================================================" << '\n'
       << "Begin structural learning. \nConstructing Chow-Liu tree with complete data......" << endl;

  num_nodes = dts->num_vars;
  root_node_index = dts->class_var_index;
  // Assign an index for each node.
  #pragma omp parallel for
  for (int i=0; i<num_nodes; ++i) {

    DiscreteNode *node_ptr = new DiscreteNode(i);  // For now, only support discrete node.

    node_ptr->num_potential_vals = dts->num_of_possible_values_of_disc_vars[i];

    node_ptr->potential_vals = new int[node_ptr->num_potential_vals];

    int j = 0;
    for (auto v : dts->map_disc_vars_possible_values[i]) {
      node_ptr->potential_vals[j++] = v;
    }

    #pragma omp critical
    { set_node_ptr_container.insert(node_ptr); }
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
        for (auto node_ptr : set_node_ptr_container) {
          if (node_ptr->GetNodeIndex()==i && Xi==nullptr) {
            Xi = node_ptr;
          } else if (node_ptr->GetNodeIndex()==j && Xj==nullptr) {
            Xj = node_ptr;
          }
          if (Xi!=nullptr && Xj!=nullptr) { break; }
        }
        // Mutual information table is symmetric.
        mutualInfoTab[i][j] = ComputeMutualInformation(Xi, Xj, dts);
        mutualInfoTab[j][i] = ComputeMutualInformation(Xi, Xj, dts);
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
  int* topologicalSortedPermutation = WidthFirstTraversalWithAdjacencyMatrix(graphAdjacencyMatrix, n, root_node_index);


  // !!! See the comments for "default_elim_ord" in the "ChowLiuTree.h" file.
  default_elim_ord = new int[n-1];
  for (int i=1; i<n; ++i) {
    default_elim_ord[i-1] = topologicalSortedPermutation[n-i];
  }
  topo_ord = vector<int> (topologicalSortedPermutation, topologicalSortedPermutation+n);


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

  if (print_struct) {

    cout << "==================================================" << '\n'
         << "Topological sorted permutation generated using width-first-traversal: " << endl;
    for (int m = 0; m < n; ++m) {
      cout << topologicalSortedPermutation[m] << ", ";
    }
    cout << endl;

    cout << "==================================================" << '\n'
         << "Each node's parents: " << endl;
    this->PrintEachNodeParents();

  }

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


pair<int*, int> ChowLiuTree::SimplifyDefaultElimOrd(DiscreteConfig evidence) {
  return SimplifyTreeDefaultElimOrd(evidence);
}


pair<int*, int> ChowLiuTree::SimplifyTreeDefaultElimOrd(DiscreteConfig evidence) {

  // Remove all the barren nodes
  set<int> to_be_removed;
  for (int i=0; i<num_nodes-1; ++i) {
    int default_elim_ord_i = default_elim_ord[i];
    Node *ptr_curr_node = FindNodePtrByIndex(default_elim_ord_i);
    bool observed = false, need_to_be_removed = true;
    for (auto p : evidence) {
      if (p.first == ptr_curr_node->GetNodeIndex()) {    // If it is observed.
        observed = true;
        break;
      }
    }
    if (observed) { continue; }

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
  DepthFirstTraversalUntillMeetObserved(evidence, root_node_index, visited, to_be_removed);  // Start at root.

  // Record all the remaining nodes in array "simplified_order".
  int num_of_remain = num_nodes-1-to_be_removed.size();    // The one of the nodes is class variable node and does not need to be eliminated.
  vector<int> vec_simplified_order;
  vec_simplified_order.reserve(num_of_remain);
  for (int i=0; i<num_nodes-1; ++i) {
    int ord = default_elim_ord[i];
    if (to_be_removed.find(ord) == to_be_removed.end()) {
      vec_simplified_order.push_back(ord);
    }
  }
  if (vec_simplified_order.size() != num_of_remain) {
    fprintf(stderr, "Error in function [%s], simplified order size not equal to number of remaining nodes!\n", __FUNCTION__);
    exit(1);
  }

  int* simplified_order = new int[num_of_remain];
//  for (int i=0, j=1; i<num_of_remain; ++i) {    // j=1 because the 0-th node is root.
//    while (to_be_removed.find(j)!=to_be_removed.end()) { ++j; }
//    simplified_order[i] = j++;  // "j++" is to move to the next j, or else it will stuck at the first j that is not in "to_be_removed".
//  }
  for (int i=0; i<num_of_remain; ++i) {
    simplified_order[i] = vec_simplified_order.at(i);
  }


  pair<int*, int> simplified_order_and_nodes_number = make_pair(simplified_order, num_of_remain);
//  delete[] simplified_order;
  return simplified_order_and_nodes_number;
}


void ChowLiuTree::DepthFirstTraversalUntillMeetObserved(DiscreteConfig evidence, int start, set<int>& visited, set<int>& to_be_removed) {

  // Base case
  if (visited.find(start)!=visited.end()) { return; }
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
  for (auto ptr_child : ptr_curr_node->set_children_ptrs) {
    int child_index = ptr_child->GetNodeIndex();
    if (visited.find(child_index) == visited.end()) { break; }
    to_be_removed.insert(child_index);
    DepthFirstTraversalToRemoveMSeparatedNodes(child_index, visited, to_be_removed);
  }

}