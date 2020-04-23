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

/**
 * Compute mutual information between two variables on provided dataset.
 */
double ChowLiuTree::ComputeMutualInformation(Node *Xi, Node *Xj, const Dataset *dts) {
  // Find the indexes of these two features in training set.
  int xi=Xi->GetNodeIndex(), xj=Xj->GetNodeIndex();

  auto dXi = dynamic_cast<DiscreteNode*>(Xi);
  auto dXj = dynamic_cast<DiscreteNode*>(Xj);

  // Initialize the table.
  int m = dts->num_instance, ri = dXi->GetDomainSize(), rj = dXj->GetDomainSize();
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
        if (dts->dataset_all_vars[s][xi] == dXi->vec_potential_vals.at(a) && dts->dataset_all_vars[s][xj] == dXj->vec_potential_vals.at(b)) {
          Pij[a][b] += 1;
        }
      }
      Pij[a][b] /= m;
    }
  }

  // Update Pi.
  for (a=0; a<ri; a++) {
    for (s=0; s<m; s++) {
      if (dts->dataset_all_vars[s][xi] == dXi->vec_potential_vals.at(a)) {
        Pi[a] += 1;
      }
    }
    Pi[a] /= m;
  }

  // Update Pj.
  for (b=0; b<rj; b++) {
    for (s=0; s<m; s++) {
      if (dts->dataset_all_vars[s][xj] == dXj->vec_potential_vals.at(b)) {
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

/**
 * Just an API. Call another function.
 */
void ChowLiuTree::StructLearnCompData(Dataset *dts, bool print_struct, string algo, string topo_ord_constraint, int max_num_parents) {
  // record time
  struct timeval start, end;
  double diff;
  gettimeofday(&start,NULL);

  fprintf(stdout, "In function [%s]:\nChow-Liu tree will not be restricted by the argument \"topo_ord_constraint\".", __FUNCTION__);

  StructLearnChowLiuTreeCompData(dts, print_struct);

  // print time
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
  for (int i = 0; i < num_nodes; ++i) {

    DiscreteNode *node_ptr = new DiscreteNode(i);  // For now, only support discrete node.

    node_ptr->SetDomainSize(dts->num_of_possible_values_of_disc_vars[i]);

    for (auto v : dts->map_disc_vars_possible_values[i]) {
      node_ptr->vec_potential_vals.push_back(v);
    }

    #pragma omp critical
    {
      map_idx_node_ptr[i] = node_ptr;
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
        for (auto id_node_ptr : map_idx_node_ptr) {
          auto node_ptr = id_node_ptr.second;
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
  int* topologicalSortedPermutation = BreadthFirstTraversalWithAdjacencyMatrix(graphAdjacencyMatrix, n, root_node_index);


  // !!! See the comments for "default_elim_ord" in the "ChowLiuTree.h" file.
  vec_default_elim_ord.reserve(n-1);
  for (int i=1; i<n; ++i) {
    vec_default_elim_ord.push_back(topologicalSortedPermutation[n-i]);
  }
  topo_ord = vector<int> (topologicalSortedPermutation, topologicalSortedPermutation+n);


  cout << "==================================================" << '\n'
       << "Setting children and parents......" << endl;
  #pragma omp parallel for
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < i; ++j) {  // graphAdjacencyMatrix is symmetric, so loop while j<i instead of j<n
      if (i == j) continue;
      if (graphAdjacencyMatrix[i][j] == 1){

        // Determine the topological position of i and j.
        int topoIndexI = -1, topoIndexJ = -1;
        for (int k=0; k<n; ++k) {
          if (topologicalSortedPermutation[k] == i && topoIndexI == -1) {
            topoIndexI = k;
          } else if (topologicalSortedPermutation[k] == j && topoIndexJ == -1) {
            topoIndexJ = k;
          }
          if (topoIndexI != -1 && topoIndexJ != -1) { break; }
        }

        if (topoIndexI < topoIndexJ) {
          SetParentChild(i, j);
        } else {
          SetParentChild(j, i);
        }
      }
    }
  }

  cout << "==================================================" << '\n'
       << "Generating parents combinations for each node......" << endl;

  GenDiscParCombsForAllNodes();

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

/**
 * Just an API. Call another function.
 */
vector<int> ChowLiuTree::SimplifyDefaultElimOrd(DiscreteConfig evidence) {
  return SimplifyTreeDefaultElimOrd(evidence);
}

/**
 * Two key words: barren, m-separated.
 */
vector<int> ChowLiuTree::SimplifyTreeDefaultElimOrd(DiscreteConfig evidence) {

  // todo: delete the next line
//  fprintf(stderr, "Start [%s]\n", __FUNCTION__);

  // Remove all the barren nodes
  set<int> to_be_removed;
  for (int i = 0; i < num_nodes-1; ++i) {

    int vec_size = vec_default_elim_ord.size();
    int vec_capacity = vec_default_elim_ord.capacity();
    if (vec_size != vec_capacity) {
      fprintf(stderr, "Function [%s]: vec_size != vec_capacity\n", __FUNCTION__);
    }

    int default_elim_ord_i = vec_default_elim_ord.at(i);
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
    if (!ptr_curr_node->set_children_indexes.empty()) {
      for (auto ptr_child : GetChildrenPtrsOfNode(ptr_curr_node->GetNodeIndex())) {
        // And if its children are not all removed.
        if (to_be_removed.find(ptr_child->GetNodeIndex()) == to_be_removed.end()) {
          need_to_be_removed = false;
          break;
        }
      }
    }
    if (need_to_be_removed) { to_be_removed.insert(ptr_curr_node->GetNodeIndex()); }
  }

  // todo: delete the next line
//  fprintf(stderr, "Middle_1 [%s]\n", __FUNCTION__);

  // Remove all m-separated nodes.
  set<int> visited;
  DepthFirstTraversalUntillMeetObserved(evidence, root_node_index, visited, to_be_removed);  // Start at root.

  // todo: delete the next line
//  fprintf(stderr, "Middle_2 [%s]\n", __FUNCTION__);

  // Record all the remaining nodes in array "simplified_order".
  int num_of_remain = num_nodes-1-to_be_removed.size();    // The one of the nodes is class variable node and does not need to be eliminated.
  vector<int> vec_simplified_order;
  vec_simplified_order.reserve(num_of_remain);
  for (int i = 0; i < num_nodes-1; ++i) {
    int ord = vec_default_elim_ord.at(i);
    if (to_be_removed.find(ord) == to_be_removed.end()) {
      vec_simplified_order.push_back(ord);
    }
  }
  if (vec_simplified_order.size() != num_of_remain) {
    fprintf(stderr, "Error in function [%s], simplified order size not equal to number of remaining nodes!\n", __FUNCTION__);
    exit(1);
  }

  // todo: delete the next line
//  fprintf(stderr, "End [%s]\n", __FUNCTION__);

  return vec_simplified_order;
}

/**
 * Recursive.
 * Help to simplify the elimination order.
 */
void ChowLiuTree::DepthFirstTraversalUntillMeetObserved(DiscreteConfig evidence, int start, set<int>& visited, set<int>& to_be_removed) {

  // todo: delete the next line
//  fprintf(stderr, "Start [%s]\n", __FUNCTION__);

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
  for (auto ptr_child : GetChildrenPtrsOfNode(ptr_curr_node->GetNodeIndex())) {
    int child_index = ptr_child->GetNodeIndex();
    DepthFirstTraversalUntillMeetObserved(evidence, child_index, visited, to_be_removed);
  }

}

/**
 * Recursive.
 * Help to simplify the elimination order.
 */
void ChowLiuTree::DepthFirstTraversalToRemoveMSeparatedNodes(int start, set<int>& visited, set<int>& to_be_removed) {
  visited.insert(start);
  Node* ptr_curr_node = FindNodePtrByIndex(start);
  for (auto ptr_child : GetChildrenPtrsOfNode(ptr_curr_node->GetNodeIndex())) {
    int child_index = ptr_child->GetNodeIndex();
    if (visited.find(child_index) == visited.end()) { break; }
    to_be_removed.insert(child_index);
    DepthFirstTraversalToRemoveMSeparatedNodes(child_index, visited, to_be_removed);
  }

}