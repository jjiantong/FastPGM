#pragma clang diagnostic push
#pragma ide diagnostic ignored "openmp-use-default-none"
//
// Created by LinjianLi on 2019/1/23.
//

#include "ChowLiuTree.h"


/**
 * @brief: Compute mutual information between two variables (i.e., Xi and Xj) on the provided data set.
 * This function is purely based on the formula of mutual information.
 * I(Xi,Xj) = ∑_{xi,xj} P(xi,xj)*log{P(xi,xj)/[P(xi)*P(xj)]}
 */
//TODO: may be moved to other classes
double ChowLiuTree::ComputeMutualInformation(Node *Xi, Node *Xj, const Dataset *dts) {
  // Find the indexes of these two features in training set.
  int xi = Xi->GetNodeIndex();
  int xj = Xj->GetNodeIndex();

  assert(Xi->is_discrete == true && Xj->is_discrete == true);
  auto dXi = dynamic_cast<DiscreteNode*>(Xi);
  auto dXj = dynamic_cast<DiscreteNode*>(Xj);

  // Initialize the table.
  int m = dts->num_instance;
  // ri, rj are the possible values of variables Xi and Xj
  int ri = dXi->GetDomainSize();
  int rj = dXj->GetDomainSize();
  // Pij[ri][rj] is joint distribution P(Xi, Xj)
  double **Pij = new double* [ri];
  for (int i = 0; i < ri; ++i) {
    Pij[i] = new double[rj](); // The parentheses at end will initialize the array to be all zeros.
  }
  // Pi[ri], Pj[rj] are the marginal distributions P(Xi), P(Xj)
  double* Pi = new double[ri]();
  double* Pj = new double[rj]();

  int a, b, s;

  // TODO: computation-expensive... for each possible configuration, we need to traverse all data set...
  // TODO: can we traverse only once and update the related P(Xi,Xj)?
  // Update Pij.
  for (a=0; a<ri; a++) { // for each possible value of Xi
    for (b=0; b<rj; b++) { // for each possible value of Xj
      for (s=0; s<m; s++) { // for each instance
        if (dts->dataset_all_vars[s][xi] == dXi->vec_potential_vals.at(a) && dts->dataset_all_vars[s][xj] == dXj->vec_potential_vals.at(b)) {
          Pij[a][b] += 1;
        }
      }
      Pij[a][b] /= m; // normalize
    }
  }

  // TODO: we can use Pi[a] = Pij[a][0] + Pij[a][1] + ... + Pij[a][rj-1]
  // TODO: then complexity is ri*rj rather than ri*m, and normalization is not required...
  // Update Pi.
  for (a=0; a<ri; a++) {
    for (s=0; s<m; s++) {
      if (dts->dataset_all_vars[s][xi] == dXi->vec_potential_vals.at(a)) {
        Pi[a] += 1;
      }
    }
    Pi[a] /= m;
  }

  // TODO: the same as Pi...
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
  // I(Xi,Xj) = ∑_{xi,xj} P(xi,xj)*log{P(xi,xj)/[P(xi)*P(xj)]}
  double mutualInformation = 0;
  for (a = 0; a < ri; a++) {
    for (b = 0; b < rj; b++) {
      double temp = Pij[a][b] * log( Pij[a][b] / (Pi[a]*Pj[b]) );
      // may have error: call of overloaded ‘isnan(double&)’ is ambiguous
      // qualify "isnan" explicitly, by calling either "::isnan" or "std::isnan"
      if (!::isnan(temp)) {    // There may be runtime error of divided by zero.
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
 * default params: print_struct = true, topo_ord_constraint = "", max_num_parents = 1
 * (they are no use...)
 */
void ChowLiuTree::StructLearnCompData(Dataset *dts, bool print_struct, string topo_ord_constraint, int max_num_parents) {
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

/**
 * @brief: construct a Bayesian network which is a tree, based on the data set
 */
void ChowLiuTree::StructLearnChowLiuTreeCompData(Dataset *dts, bool print_struct) {
  cout << "==================================================" << '\n'
       << "Begin structural learning. \nConstructing Chow-Liu tree with complete data......" << endl;
  assert(network->pure_discrete == true);//It seems Chow Liu Tree only supports discrete networks.

  network->num_nodes = dts->num_vars;
  root_node_index = dts->class_var_index;

  // Assign an index for each node.
  #pragma omp parallel for
  for (int i = 0; i < network->num_nodes; ++i) { // for each node in the network
    // construct a node in the network
    DiscreteNode *node_ptr = new DiscreteNode(i);  // For now, only support discrete node.
    node_ptr->SetDomainSize(dts->num_of_possible_values_of_disc_vars[i]);
    for (auto v : dts->map_disc_vars_possible_values[i]) {
      node_ptr->vec_potential_vals.push_back(v);
    }

    #pragma omp critical
    { network->map_idx_node_ptr[i] = node_ptr; }
  }

  cout << "==================================================" << '\n'
       << "Constructing mutual information table......" << endl;

  // mutualInfoTab[n][n]
  int n = network->num_nodes;
  double** mutualInfoTab = new double* [n];
  for (int i=0; i<n; ++i) {
    mutualInfoTab[i] = new double[n](); // The parentheses at end will initialize the array to be all zeros.
  }

  // Initialize the mutual information table.
  #pragma omp parallel for
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < i; ++j) {
      if (i == j) {
        mutualInfoTab[i][j] = -1;
      }
      else {

        /** To calculate the mutual information, we need to find the nodes which correspond to the indexes i and j.
         * */
        Node* Xi = network->FindNodePtrByIndex(i);
        Node* Xj = network->FindNodePtrByIndex(j);

        // Mutual information table is symmetric.
        mutualInfoTab[i][j] = ComputeMutualInformation(Xi, Xj, dts);
        mutualInfoTab[j][i] = ComputeMutualInformation(Xi, Xj, dts);
      }
    }
  }

  cout << "==================================================" << '\n'
       << "Constructing maximum spanning tree using mutual information table and Prim's algorithm......" << endl;

  /** Use Prim's algorithm (easy to find on Google) to generate a spanning tree.
   * Two nodes with the maximum mutual information are connected.
   */
  // graphAdjacencyMatrix[n][n]
  int** graphAdjacencyMatrix = new int* [n];
  for (int i = 0; i < n; ++i) {
    graphAdjacencyMatrix[i] = new int[n]();
  }

  set<int> markSet;
  double maxMutualInfo;
  int maxI, maxJ;
  set<int>::iterator it;

  markSet.insert(0);  // The node of index 0 is the root node, which is the class variable.
  while (markSet.size() < n) { // for each round of selecting one node and one arc
    maxMutualInfo = -1;
    maxI = maxJ = -1;
    for (it = markSet.begin(); it != markSet.end(); it++) { // for each node having been selected
      int i = *it;
      for (int j = 0; j < n; ++j) { // for each node
        // if j has not been selected, and the mutual information between i and j is larger than the current maximum
        // it can be ensure that this case cannot form a circle
        if (markSet.find(j) == markSet.end() && mutualInfoTab[i][j] > maxMutualInfo) {
          maxMutualInfo = mutualInfoTab[i][j]; // mark the current maximum
          maxI = i; // mark the current i
          maxJ = j; // mark the current j
        }
      }
    }
    markSet.insert(maxJ); // insert the node j into markSet

    // update the adjacency matrix; i and j form an undirected arc
    graphAdjacencyMatrix[maxI][maxJ] = graphAdjacencyMatrix[maxJ][maxI] = 1;
  }

  // obtain a topological order for constructing parent-child relationships, and generating an elimination order.
  int* topologicalSortedPermutation = BreadthFirstTraversalWithAdjacencyMatrix(graphAdjacencyMatrix, n, root_node_index);


  //TODO: see the comments for "default_elim_ord" in the "ChowLiuTree.h" file;
  // may need to remove either "default_elim_ord" or "vec_default_elim_ord".
  // elimination ordering is the reverse order of the topological ordering
  // (remove the first element of the topological ordering)
  network->vec_default_elim_ord.reserve(n-1);
  for (int i = 1; i < n; ++i) {
      network->vec_default_elim_ord.push_back(topologicalSortedPermutation[n-i]);
  }
  network->topo_ord = vector<int> (topologicalSortedPermutation, topologicalSortedPermutation + n);


  cout << "==================================================" << '\n'
       << "Setting children and parents......" << endl;
  // Add arrows in tree to form an directed edge in the network; set parents and children
  #pragma omp parallel for
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < i; ++j) {// graphAdjacencyMatrix is symmetric, so loop while j < i instead of j < n
      if (i == j)
        continue;
      if (graphAdjacencyMatrix[i][j] == 1){

        // Determine the topological position of i and j; determine which one should appear first (e.g., i > j or j > i).
        if (OccurInCorrectOrder(i, j, network->topo_ord)) { // if i occurs before j in the topological ordering
            network->SetParentChild(i, j); // i->j
        }
        else { // if j occurs before i in the topological ordering
            network->SetParentChild(j, i); // j->i
        }
      }
    }
  }

  cout << "==================================================" << '\n'
       << "Generating parents combinations for each node......" << endl;

  network->GenDiscParCombsForAllNodes();

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
    network->PrintEachNodeParents();

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

///**
// * Just an API. Call another function.
// */
//vector<int> ChowLiuTree::SimplifyDefaultElimOrd(DiscreteConfig evidence) {
//  return SimplifyTreeDefaultElimOrd(evidence);
//}
//
///**
// * Just an API. Call another function.
// */
//vector<int> ChowLiuTree::SimplifyDefaultElimOrd2(DiscreteConfig evidence, vector<int> left_nodes) {
//  return SimplifyTreeDefaultElimOrd2(evidence, left_nodes);
//}

///**
// * @brief: given an evidence, the default elimination order can be simplified by removing two types of nodes (i.e., barren and m-separated).
// * suppose Y is the set of variables observed; X is the set of variables of interest
// * barren node: a leaf and not in X∪Y
// * moral graph: obtained by adding an edge between each pair of parents and dropping all directions
// * m-separated node: this node and X are separated by the set Y in the moral graph
// * The implementation is based on "A simple approach to Bayesian network computations" by Zhang and Poole, 1994.
// */
//vector<int> ChowLiuTree::SimplifyTreeDefaultElimOrd(DiscreteConfig evidence) {
//  // Remove all the barren nodes
//  // barren node: a leaf and not in X∪Y
//  set<int> to_be_removed;
//
////  for (int i = 0; i < num_nodes - 1; ++i) { // for each node except for the target
////    // "vec_default_elim_ord" is obtained by reverse topological sort
////    // so "vec_default_elim_ord.at(num_nodes - 1)" is the root, i.e., the target node
////    int vec_size = vec_default_elim_ord.size();
////    int vec_capacity = vec_default_elim_ord.capacity();
////    if (vec_size != vec_capacity) {
////      fprintf(stderr, "Function [%s]: vec_size != vec_capacity\n", __FUNCTION__);
////    }
////
////    int default_elim_ord_i = vec_default_elim_ord.at(i);
////    Node *ptr_curr_node = FindNodePtrByIndex(default_elim_ord_i);
////    bool observed = false;
////    bool need_to_be_removed = true;
////
////    for (auto p : evidence) {
////      if (p.first == ptr_curr_node->GetNodeIndex()) { // if it is observed.
////        observed = true;
////        break;
////      }
////    }
////    if (observed) { // the observed variables (in Y)
////      continue;
////    }
////
////    // if it is not observed
////    // if it is not a leaf; otherwise, it is a leaf
////    if (!ptr_curr_node->set_children_indexes.empty()) {
////      for (auto ptr_child : GetChildrenPtrsOfNode(ptr_curr_node->GetNodeIndex())) { // for each of its child
////        // And if its children are not all removed;
////        // otherwise, all its children are removed, then it becomes a new leaf
////        if (to_be_removed.find(ptr_child->GetNodeIndex()) == to_be_removed.end()) {
////          need_to_be_removed = false;
////          break;
////        }
////      }
////    }
////    if (need_to_be_removed) {
////      to_be_removed.insert(ptr_curr_node->GetNodeIndex());
////    }
////  }
////
////  // Remove all m-separated nodes.
////  // m-separated node: this node and X are separated by the set Y in the moral graph
////  set<int> visited;
////  DepthFirstTraversalUntillMeetObserved(evidence, root_node_index, visited, to_be_removed);  // Start at root.
//
////  cout << endl << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << '\n'
////       << "The removed barren nodes and m-separated node: " << endl;
////  for (auto r = to_be_removed.begin(); r != to_be_removed.end(); r++) {
////      cout << (*r) << ", ";
////  }
////  cout << endl;
//
//  // Record all the remaining nodes in array "simplified_order".
//  int num_of_remain = network->num_nodes - 1 - to_be_removed.size(); // The one of the nodes is class variable node and does not need to be eliminated.
//  vector<int> vec_simplified_order;
//  vec_simplified_order.reserve(num_of_remain);
//  for (int i = 0; i < network->num_nodes - 1; ++i) {
//    int ord = network->vec_default_elim_ord.at(i);
//    if (to_be_removed.find(ord) == to_be_removed.end()) { // if it is not removed
//      vec_simplified_order.push_back(ord);
//    }
//  }
//  if (vec_simplified_order.size() != num_of_remain) {
//    fprintf(stderr, "Error in function [%s], simplified order size not equal to number of remaining nodes!\n", __FUNCTION__);
//    exit(1);
//  }
//
//  return vec_simplified_order;
//}
//
///**
// * @brief: given an evidence and the left nodes,
// * the order can be simplified by removing the evidence and the target nodes from the left nodes
// * @param left_nodes is the left node set by removing barren nodes and m-separated nodes
// * The implementation is based on "A simple approach to Bayesian network computations" by Zhang and Poole, 1994.
// */
//vector<int> ChowLiuTree::SimplifyTreeDefaultElimOrd2(DiscreteConfig evidence, vector<int> left_nodes) {
//  // to_be_removed contains: irrelevant nodes & evidence
//  set<int> to_be_removed;
//  // add the evidence
//  for (auto p: evidence) { // for each index-value pair in the evidence
//    to_be_removed.insert(p.first);
//  }
//  // add the irrelevant nodes
//  for (int i = 0; i < network->num_nodes; ++i) { // for each node in the network
//    if (find(left_nodes.begin(), left_nodes.end(), i) == left_nodes.end()) { // if i is not in "left_nodes"
//      to_be_removed.insert(i); // add into "to_be_removed"
//    }
//  }
//
//  // Record all the remaining nodes in array "simplified_order".
////  int num_of_remain = num_nodes - 1 - to_be_removed.size(); // The one of the nodes is class variable node and does not need to be eliminated.
//  vector<int> vec_simplified_order;
////  vec_simplified_order.reserve(num_of_remain);
//  for (int i = 0; i < network->num_nodes - 1; ++i) {
//    int ord = network->vec_default_elim_ord.at(i);
//    if (to_be_removed.find(ord) == to_be_removed.end()) { // if it is not removed
//      vec_simplified_order.push_back(ord);
//    }
//  }
////  // this case may happen if the test set contains more features than the training set
////  if (vec_simplified_order.size() != num_of_remain) {
////    fprintf(stderr, "Error in function [%s], simplified order size not equal to number of remaining nodes!\n", __FUNCTION__);
////    exit(1);
////  }
//
//  return vec_simplified_order;
//}


/**
 * Recursive.
 * remove the m-separated nodes; help to simplify the elimination order.
 * moral graph: obtained by adding an edge between each pair of parents and dropping all directions
 * m-separated node: this node and X are separated by the set Y in the moral graph
 */
void ChowLiuTree::DepthFirstTraversalUntillMeetObserved(DiscreteConfig evidence, int start, set<int>& visited, set<int>& to_be_removed) {
//TODO: compare with the algorithm in the paper

  // Base case
  if (visited.find(start) != visited.end()) { // find the node in visited
    return;
  }
  visited.insert(start); // visit this node

  bool observed = false;
  for (auto p : evidence) {
    if (p.first == start) {  // If it is observed.
      observed = true;
      break;
    }
  }

  if (observed) {
    DepthFirstTraversalToRemoveMSeparatedNodes(start, visited, to_be_removed); // Cut down all the descendant.
    // we can cut down all the descendant because the structure is tree-structure, each node at most has one parent
    return;
  }

  // If not observed, then check whether its children are observed
  // we can just check its children because the structure is tree-structure and we start at the root
  // while originally the moral graph is undirected and both parents and children should be considered
  // Recursive case
  // TODO: replace the 2 lines below with "for (auto ptr_child : GetChildrenPtrsOfNode(start)) {"
  Node* ptr_curr_node = network->FindNodePtrByIndex(start);
  for (auto ptr_child : network->GetChildrenPtrsOfNode(ptr_curr_node->GetNodeIndex())) { // each child of this node
    int child_index = ptr_child->GetNodeIndex();
    DepthFirstTraversalUntillMeetObserved(evidence, child_index, visited, to_be_removed);
  }
}

/**
 * Recursive.
 * remove the m-separated nodes; help to simplify the elimination order.
 * moral graph: obtained by adding an edge between each pair of parents and dropping all directions
 * m-separated node: this node and X are separated by the set Y in the moral graph
 */
void ChowLiuTree::DepthFirstTraversalToRemoveMSeparatedNodes(int start, set<int>& visited, set<int>& to_be_removed) {
  visited.insert(start);
  // TODO: replace the 2 lines below with "for (auto ptr_child : GetChildrenPtrsOfNode(start)) {"
  Node* ptr_curr_node = network->FindNodePtrByIndex(start);
  for (auto ptr_child : network->GetChildrenPtrsOfNode(ptr_curr_node->GetNodeIndex())) { // each child of this node
    int child_index = ptr_child->GetNodeIndex();
//    if (visited.find(child_index) == visited.end()) { // if this child has not been visited TODO:?
//      break;
//    }
    to_be_removed.insert(child_index);

    // recursively remove the children of "child_index"
    DepthFirstTraversalToRemoveMSeparatedNodes(child_index, visited, to_be_removed);
  }

}