#pragma clang diagnostic push
#pragma ide diagnostic ignored "openmp-use-default-none"
//
// Created by Linjian Li on 2018/11/29.
//

#include "Network.h"
#include "ScoreFunction.h"
#include "DiscreteNode.h"
#include "ContinuousNode.h"

Network::Network(): Network(true) {}

Network::Network(bool pure_disc) {
  this->pure_discrete = pure_disc;
}

Network::Network(Network &net) {
  //need to test correctness

  network_name = net.network_name;
  num_nodes = net.num_nodes;
  pure_discrete = net.pure_discrete;
  vec_default_elim_ord = net.vec_default_elim_ord;
  topo_ord = net.GetTopoOrd();//this sequence/order is used for variable elimination.

  //copy the map for mapping id to node ptr
  map_idx_node_ptr = net.map_idx_node_ptr;
  for (int i = 0; i < num_nodes; ++i) {
    auto old_node = map_idx_node_ptr.at(i);
    Node *n = nullptr;
    if (old_node->is_discrete) {
      auto disc_old_node = (DiscreteNode*)old_node;
      n = new DiscreteNode(*disc_old_node);
    } else {
      auto cont_old_node = (ContinuousNode*)old_node;
      n = new ContinuousNode(*cont_old_node);
    }
    map_idx_node_ptr[i] = n;
  }

}

//TODO: double-check correctness
/*!
 * @brief: print the parents of all the nodes.
 * each line corresponds to a node and its parents.
 */
void Network::PrintEachNodeParents() {
  for (const auto &id_node_ptr : map_idx_node_ptr) { // for each <int, Node*> pair
    auto node_ptr = id_node_ptr.second; // get the Node*
    cout << node_ptr->node_name << ":\t";
    for (const auto &par_node_ptr : GetParentPtrsOfNode(node_ptr->GetNodeIndex())) {
      cout << par_node_ptr->node_name << '\t';
    }
    cout << endl;
  }
}

//TODO: double-check correctness
void Network::PrintEachNodeChildren() {//print the child nodes of all the nodes
  for (const auto &id_node_ptr : map_idx_node_ptr) {
    auto node_ptr = id_node_ptr.second;
    cout << node_ptr->node_name << ":\t";
    for (const auto &chi_node_ptr : GetChildrenPtrsOfNode(node_ptr->GetNodeIndex())) {
      cout << chi_node_ptr->node_name << '\t';
    }
    cout << endl;
  }
}

/**
 * @brief: find node ptr by id
 */
Node* Network::FindNodePtrByIndex(const int &index) const {
  if (index < 0 || index >= num_nodes) {  // The node indexes are consecutive integers start at 0.
    fprintf(stderr, "Error in function %s! \nInvalid index [%d]!", __FUNCTION__, index);
    exit(1);
  }
  return map_idx_node_ptr.at(index);
}

/**
 * @brief: find node ptr by name
 */
Node* Network::FindNodePtrByName(const string &name) const {
  Node* node_ptr = nullptr;
  for (const auto i_n_ptr : map_idx_node_ptr) {
    auto n_ptr = i_n_ptr.second;
    if (n_ptr->node_name==name) {
      node_ptr = n_ptr;
      break;
    }
  }
  return node_ptr;
}

/**
 * @brief: construct a Naive Bayesian Network with the class variable as the root and all the other variables as leaves.
 * structure of Naive Bayes: label is the root node; all feature variables are the children of the root node.
 * time complexity: O( number of feature variables * number of possible labels of class variable)
 * (not consider the time spent on generating topological ordering)
 */
void Network::ConstructNaiveBayesNetwork(Dataset *dts) {

  // common part with "StructLearnCompData" TODO
  num_nodes = dts->num_vars;

  // Assign an index for each node.
#pragma omp parallel for
  for (int i = 0; i < num_nodes; ++i) {
    DiscreteNode *node_ptr = new DiscreteNode(i);  // For now, only support discrete node.

    //set the potential values for this node
    node_ptr->SetDomainSize(dts->num_of_possible_values_of_disc_vars[i]);
    for (auto v : dts->map_disc_vars_possible_values[i]) {
      node_ptr->vec_potential_vals.push_back(v);//TODO: keep name convention consistent (i.e. possible vs potential)
    }
#pragma omp critical
    {
      map_idx_node_ptr[i] = node_ptr;
    }
  }

  // Set parents and children.
  Node *class_node_ptr = FindNodePtrByIndex(dts->class_var_index);
  for (auto &i_n : map_idx_node_ptr) { // for each id-node_ptr pair i_n
    if (i_n.second == class_node_ptr) { // TODO: directly "if (xxx != xxx)"
      continue;
    }
    // all feature variables are the children of the root node (i.e., the label)
    SetParentChild(class_node_ptr, i_n.second);
  }

  // Generate parent configurations for all nodes in the network.
  // TODO: for Naive Bayesian Network, it may not need to call "GenDiscParCombsForAllNodes"
  // because "GenDiscParCombsForAllNodes" needs "GetParentPtrsOfNode" -> "GenDiscParCombs",
  // where "GenAllCombinationsFromSets" is expensive.
  // while we can directly generate configurations of the root node,
  // it is exactly the parent configurations of all nodes except for the root;
  // and the parent configurations of the root is ONE empty parent configuration (cf. "GenDiscParCombs" in class Node)
  GenDiscParCombsForAllNodes();

  // Generate topological ordering and default elimination ordering.
  vector<int> topo = GetTopoOrd();

  vec_default_elim_ord.reserve(num_nodes - 1);//-1, because the last one is the value (which must be kept) to be inferred.
  for (int i = 0; i < num_nodes-1; ++i) { // TODO: for num_nodes-1 -> 0, at(i)
    vec_default_elim_ord.push_back(topo.at(num_nodes-1-i));
  }

  //check for correctness (i.e. reserved size == number of variables).
  int vec_size = vec_default_elim_ord.size();
  int vec_capacity = vec_default_elim_ord.capacity();
  if (vec_size != vec_capacity) {
    fprintf(stderr, "Function [%s]: vec_size != vec_capacity\n", __FUNCTION__);
  }
}

/**
 * @brief: learn structure from a data set without missing values
 * @param dts:                  data set
 * @param print_struct:         whether want to print structure while learning
 * @param algo:                 specify a network structure learning algorithm
 * @param topo_ord_constraint:  a constraint to limit the learned network structure (i.e. the # of possible structures is often huge)
 * @param max_num_parents:      a node can only has max_num_parents (to limit the # of possible networks)
 */
void Network::StructLearnCompData(Dataset *dts, bool print_struct, string algo, string topo_ord_constraint, int max_num_parents) {
  fprintf(stderr, "Not be completely implemented yet!");//TODO: most of the functionalities have been implemented; "algo" hasn't been used.

  cout << "==================================================" << '\n'
       << "Begin structural learning with complete data......" << endl;

  struct timeval start, end;
  double diff;

  gettimeofday(&start,NULL);

  // common part with "ConstructNaiveBayesNetwork" TODO
  num_nodes = dts->num_vars;
  // Assign an index for each node.
#pragma omp parallel for
  for (int i = 0; i < num_nodes; ++i) {
    DiscreteNode *node_ptr = new DiscreteNode(i);  // For now, only support discrete node.

    // TODO: extra part. double-check the usage of "node_name"
    //give this node a name
    if (dts->vec_var_names.size() == num_nodes) {
      node_ptr->node_name = dts->vec_var_names.at(i);
    } else {
      node_ptr->node_name = to_string(i);//use id as name
    }

    //set the potential values for this node
    node_ptr->SetDomainSize(dts->num_of_possible_values_of_disc_vars[i]);
    for (auto v : dts->map_disc_vars_possible_values[i]) {
      node_ptr->vec_potential_vals.push_back(v); //TODO: keep name convention consistent (i.e. possible vs potential)
    }
#pragma omp critical
    {
      map_idx_node_ptr[i] = node_ptr;
    }
  }

  //assign an order of the nodes; the order will be used in structure learning
  vector<int> ord;
  ord.reserve(num_nodes);
  for (int i = 0; i < num_nodes; ++i) {
    ord.push_back(i);   // the nodes are created the same order as in the dataset.
  }

  cout << "topo_ord_constraint: " << topo_ord_constraint << endl;
  //choose an order to serve as a constraint during learning
  if (topo_ord_constraint == "dataset-ord") {
    // Do nothing.
  }
  else if (topo_ord_constraint == "random") {
    std::srand(unsigned(std::time(0)));
    std::random_shuffle(ord.begin(), ord.end());
  }
  else if (topo_ord_constraint == "best") {
    ord = vector<int> {};//no order is provided (i.e. no constraint)
  }
  else {
    fprintf(stderr, "Error in function [%s]!\nInvalid topological ordering restriction!", __FUNCTION__);
    exit(1);
  }

  //choose an algorithm for the learning
  if (algo == "ott") {
    StructLearnByOtt(dts, ord);
  }
  else if (algo == "k2-weka") {
    StructLearnLikeK2Weka(dts, ord, max_num_parents);
  }


  cout << "==================================================" << '\n'
       << "Finish structural learning." << endl;

  gettimeofday(&end,NULL);
  diff = (end.tv_sec-start.tv_sec) + ((double)(end.tv_usec-start.tv_usec))/1.0E6;
  setlocale(LC_NUMERIC, "");//formatting the output
  cout << "==================================================" << '\n'
       << "The time spent to learn the structure is " << diff << " seconds" << endl;


  if (print_struct) {

    cout << "==================================================" << '\n'
         << "Topological sorted permutation generated using width-first-traversal: " << endl;
    auto topo = GetTopoOrd();
    for (int m = 0; m < num_nodes; ++m) {
      cout << topo.at(m) << ", ";
    }
    cout << endl;

    cout << "==================================================" << '\n'
         << "Each node's parents: " << endl;
    this->PrintEachNodeParents();

  }

}

/**
 * @brief: add a node to the map
 */
void Network::AddNode(Node *node_ptr) {
  map_idx_node_ptr[node_ptr->GetNodeIndex()] = node_ptr;
  ++num_nodes;
}

/**
 * @brief: remove a node based on id; not used yet.
 */
void Network::RemoveNode(int node_index) {
  map_idx_node_ptr.erase(node_index);
  --num_nodes;
}

/**
 * @brief: add an edge/arc to the network
 * @return true if not form a circle; false if form a circle (also delete the added arc)
 */
bool Network::AddArc(int p_index, int c_index) {
  SetParentChild(p_index, c_index); // set parent and child relationship
  bool contain_circle = ContainCircle();

  // the edge/arc shouldn't be added, because it leads to loops in the network.
  if (contain_circle) {
    DeleteArc(p_index, c_index);
  }
  return !contain_circle;
}

void Network::DeleteArc(int p_index, int c_index) {
  RemoveParentChild(p_index, c_index);
}

/**
 * @brief: swap a parent and child relationship
 * @return true if not form a circle; false if form a circle (also delete the added arc)
 */
bool Network::ReverseArc(int p_index, int c_index) {
  DeleteArc(p_index, c_index);
  return AddArc(c_index, p_index);
}

/**
 * @brief: calculate delta score when modified the arcs
 * this function is based on WEKA: calcScoreWithExtraParent(iAttribute, iAttribute2);
 * @param modification: includes "add", "delete", and "reverse".
 */
double Network::CalcuExtraScoreWithModifiedArc(int p_index, int c_index,
                                               Dataset *dts,
                                               string modification,
                                               string score_metric) {
  // todo: test correctness
  Network new_net(*this); // modify on new_net rather than the original "this"

  // Convert the string to lowercase
  transform(modification.begin(), modification.end(), modification.begin(), ::tolower);

  Node *node = new_net.FindNodePtrByIndex(c_index);

  // do the modification
  if (modification == "add") {
    if (node->set_parent_indexes.find(p_index)!=node->set_parent_indexes.end()) {
      return 0; // The parent already exists.
    }
    new_net.AddArc(p_index, c_index);
  }
  else if (modification == "delete") {
    if (node->set_parent_indexes.find(p_index)==node->set_parent_indexes.end()) {
      return 0; // The parent does not exist.
    }
    new_net.DeleteArc(p_index, c_index);
  }
  else if (modification == "reverse") {
    if (node->set_parent_indexes.find(p_index)==node->set_parent_indexes.end()) {
      return 0; // The parent does not exist.
    }
    new_net.ReverseArc(p_index, c_index);
  }
  else {
    fprintf(stderr, "Fucntion [%s]: Invalid modification string \"%s\"!",
            __FUNCTION__, modification.c_str());
    exit(1);
  }

  Node *old_c_node = this->FindNodePtrByIndex(c_index),
       *new_c_node = new_net.FindNodePtrByIndex(c_index);

  new_c_node->GenDiscParCombs(new_net.GetParentPtrsOfNode(c_index));

  ScoreFunction old_sf(this, dts),
                new_sf(&new_net, dts);

  double old_score = old_sf.ScoreForNode(old_c_node, score_metric),
         new_score = new_sf.ScoreForNode(new_c_node, score_metric);

  double delta = new_score - old_score;
  return delta;
}

/**
 * @brief: set up the parent and child relationship
 * @param p_index: parent index
 * @param c_index: child index
 */
void Network::SetParentChild(int p_index, int c_index) {
  // convert the index format into node ptr format
  Node *p = FindNodePtrByIndex(p_index), *c = FindNodePtrByIndex(c_index);
  SetParentChild(p,c);
}

/**
 * @brief: set parent and child relationship.
 * @param p: parent node ptr
 * @param c: child node ptr
 * add c to p as a child, and add p to c as a parent
 */
void Network::SetParentChild(Node *p, Node *c) {
  if (map_idx_node_ptr.find(p->GetNodeIndex()) == map_idx_node_ptr.end()
      ||
      map_idx_node_ptr.find(c->GetNodeIndex())==map_idx_node_ptr.end()) {
    fprintf(stderr, "Error in function [%s].\nThe nodes [%d] and [%d] do not belong to this network!",
            __FUNCTION__, p->GetNodeIndex(), c->GetNodeIndex());
    exit(1);
  }//end checking whether the nodes belong to the network
  p->AddChild(c);
  c->AddParent(p);
}

/**
 * @brief: remove parent child relationship
 * @param p_index: parent index
 * @param c_index: child index
 * remove c to p as a child, and remove p to c as a parent
 */
void Network::RemoveParentChild(int p_index, int c_index) {
  // convert the index format into node ptr format
  Node *p = FindNodePtrByIndex(p_index), *c = FindNodePtrByIndex(c_index);
  RemoveParentChild(p,c);
}

/**
 * @brief: remove parent child relationship using pointers
 * @param p: parent node ptr
 * @param c: child node ptr
 */
void Network::RemoveParentChild(Node *p, Node *c) {
  if (map_idx_node_ptr.find(p->GetNodeIndex()) == map_idx_node_ptr.end()
      ||
      map_idx_node_ptr.find(c->GetNodeIndex())==map_idx_node_ptr.end()) {
    fprintf(stderr, "Error in function [%s].\nThe nodes [%d] and [%d] do not belong to this network!",
            __FUNCTION__, p->GetNodeIndex(), c->GetNodeIndex());
    exit(1);
  }//end checking whether the nodes belong to the network
  p->RemoveChild(c);
  c->RemoveParent(p);
}

/**
 * @brief: find parents given a node id; used to generate discrete configurations
 * @return a set of pointers to the parents of a node
 */
set<Node*> Network::GetParentPtrsOfNode(int node_index) {
  set<Node*> set_par_ptrs;
  Node *node = map_idx_node_ptr.at(node_index); // TODO: function "FindNodePtrByIndex"
  for (const auto &idx : node->set_parent_indexes) { // "set_parent_indexes" contains both discrete and continuous parents
    set_par_ptrs.insert(map_idx_node_ptr.at(idx));
  }
  return set_par_ptrs;
}

/**
 * @brief: find the children given a node id
 */
set<Node*> Network::GetChildrenPtrsOfNode(int node_index) {
  set<Node*> set_chi_ptrs;
  Node *node = map_idx_node_ptr.at(node_index); // TODO: function "FindNodePtrByIndex"
  for (const auto &idx : node->set_children_indexes) {
    set_chi_ptrs.insert(map_idx_node_ptr.at(idx));
  }
  return set_chi_ptrs;
}

/**
 * @brief: generate all the configurations of the parents for each node
 */
void Network::GenDiscParCombsForAllNodes() {
  for (auto id_np : this->map_idx_node_ptr) { // for each node (id-node_ptr pair) in the network
    auto np = id_np.second;
    np->GenDiscParCombs(GetParentPtrsOfNode(np->GetNodeIndex()));
  }
}

/**
 * @brief: obtain topological order
 * @return a vector<int>, the elements is the indexes of the nodes
 */
vector<int> Network::GetTopoOrd() {
  if (topo_ord.empty()) {
    this->GenTopoOrd();
  }
  return topo_ord;
}

/**
 * @brief: obtain reverse topological order
 */
vector<int> Network::GetReverseTopoOrd() {
  auto ord = this->GetTopoOrd();
  reverse(ord.begin(), ord.end());
  return ord;
}

/*!
 * @brief: generate the topological order
 * @return a vector<int>, the elements is the indexes of the nodes
 * generate 1. a directed adjacency matrix and 2. an in-degree array to generate the topological order
 */
// TODO: maybe not need to generate a directed adjacency matrix
// just use in-degree array and "set_children_indexes" to generate the ordering
// TODO: potential bug in "TopoSortOfDAGZeroInDegreeFirst" -> "TopoSortOfDAGZeroInDegreeFirst"
vector<int> Network::GenTopoOrd() {

  if (this->pure_discrete) {

    // convert the network to a directed adjacency matrix (n*n)
    // direct: p->c (i.e., graph[p][c] = 1)
    // TODO: use function "ConvertDAGNetworkToAdjacencyMatrix"
    // TODO: adjacency matrix or adjacency list? more memory for adjacency matrix
    int **graph = new int*[num_nodes];
#pragma omp for
    for (int i=0; i<num_nodes; ++i) {
      graph[i] = new int[num_nodes]();
    }

    // TODO: calculate the in-degrees here instead of in "TopoSortOfDAGZeroInDegreeFirst"
    for (auto &i_n_p : map_idx_node_ptr) { // for each node
      auto n_p = i_n_p.second;
      for (auto &c_p : GetChildrenPtrsOfNode(n_p->GetNodeIndex())) {
        // TODO: each time assigning 1, add 1 to the in-degree of "c_p->GetNodeIndex()"
        graph[n_p->GetNodeIndex()][c_p->GetNodeIndex()] = 1;
      }
    }

    topo_ord = TopoSortOfDAGZeroInDegreeFirst(graph, num_nodes);

    for (int i=0; i<num_nodes; ++i) {
      delete[] graph[i];
    }
    delete[] graph;

  }
  else { // TODO: double-check, not check for the continuous cases

    // If the network is not pure discrete, then it is conditional Gaussian.
    // Discrete nodes should not have continuous parents.
    // Continuous nodes can have both discrete and continuous parents.
    // In topological ordering, all discrete nodes should occur before any continuous node.
    // todo: test correctness of the case of Gaussian network

    set<Node*> set_disc_node_ptr, set_cont_node_ptr;
    for (const auto &i_n_p : map_idx_node_ptr) {
      auto n_p = i_n_p.second;
      if (n_p->is_discrete) {
        set_disc_node_ptr.insert(n_p);
      } else {
        set_cont_node_ptr.insert(n_p);
      }
    }
    int **graph_disc = new int*[set_disc_node_ptr.size()];
    int **graph_cont = new int*[set_cont_node_ptr.size()];
    for (int i=0; i<set_disc_node_ptr.size(); ++i) {
      graph_disc[i] = new int[set_disc_node_ptr.size()]();
    }
    for (int i=0; i<set_cont_node_ptr.size(); ++i) {
      graph_disc[i] = new int[set_disc_node_ptr.size()]();
      graph_cont[i] = new int[set_cont_node_ptr.size()]();
    }

    // Generate the ordering for discrete nodes.
    map<int, int> disc_order_index, disc_index_order;
    int disc_ord = 0;
    for (const auto &n_p : set_disc_node_ptr) {
      disc_order_index[disc_ord] = n_p->GetNodeIndex();
      disc_index_order[n_p->GetNodeIndex()] = disc_ord;
      ++disc_ord;
    }
    for (const auto &n_p : set_disc_node_ptr) {
      for (const auto &c_p : GetChildrenPtrsOfNode(n_p->GetNodeIndex())) {
        if (!c_p->is_discrete) { continue; }
        graph_disc[ disc_index_order[n_p->GetNodeIndex()] ]
                  [ disc_index_order[c_p->GetNodeIndex()] ] = 1;
      }
    }
    vector<int> topo_ord_disc = TopoSortOfDAGZeroInDegreeFirst(graph_disc, set_disc_node_ptr.size());

    // Generate the ordering for continuous nodes.
    map<int, int> cont_order_index, cont_index_order;
    int cont_ord = 0;
    for (const auto &n_p : set_cont_node_ptr) {
      cont_order_index[cont_ord] = n_p->GetNodeIndex();
      cont_index_order[n_p->GetNodeIndex()] = cont_ord;
      ++cont_ord;
    }
    for (const auto &n_p : set_cont_node_ptr) {
      for (const auto &c_p : GetChildrenPtrsOfNode(n_p->GetNodeIndex())) {
        graph_cont[ cont_index_order[n_p->GetNodeIndex()] ]
                  [ cont_index_order[c_p->GetNodeIndex()] ] = 1;
      }
    }
    vector<int> topo_ord_cont = TopoSortOfDAGZeroInDegreeFirst(graph_cont, set_cont_node_ptr.size());
    // Restore the index from the ordering.
    for (auto &o : topo_ord_cont) {
      o = cont_order_index[o];
    }

    // Concatinate topo_ord_disc and topo_ord_cont.
    topo_ord_disc.insert(topo_ord_disc.end(), topo_ord_cont.begin(), topo_ord_cont.end());
    this->topo_ord = topo_ord_disc;

    for (int i=0; i<set_disc_node_ptr.size(); ++i) {
      delete[] graph_disc[i];
    }
    for (int i=0; i<set_cont_node_ptr.size(); ++i) {
      delete[] graph_cont[i];
    }
    delete[] graph_disc;
    delete[] graph_cont;

  }

  return topo_ord;
}

/**
 * @brief: convert network to a dense directed adjacency matrix (n*n)
 */
int** Network::ConvertDAGNetworkToAdjacencyMatrix() {
  int **matrix = new int* [num_nodes];
  for (int i=0; i<num_nodes; ++i) {
    matrix[i] = new int[num_nodes]();
  }

  // TODO: calculate the in-degrees here
  // TODO: instead of in "TopoSortOfDAGZeroInDegreeFirst" and "DirectedGraphContainsCircleByBFS"
  // direct: node_ptr->child_ptr (i.e., graph[node_ptr][child_ptr] = 1)
  for (auto &id_node_ptr : map_idx_node_ptr) { // for each node
    auto node_ptr = id_node_ptr.second;

    for (auto &child_ptr : GetChildrenPtrsOfNode(node_ptr->GetNodeIndex())) {
      // TODO: each time assigning 1, add 1 to the in-degree of "child_ptr->GetNodeIndex()"
      matrix[node_ptr->GetNodeIndex()][child_ptr->GetNodeIndex()] = 1;
    }
  }
  return matrix;
}

/**
 * @brief: check if network has loops.
 */
bool Network::ContainCircle() {
  int **graph = ConvertDAGNetworkToAdjacencyMatrix();
  bool result = DirectedGraphContainsCircleByBFS(graph, num_nodes);

  for (int i = 0; i < num_nodes; ++i) {
    delete[] graph[i];
  }
  delete[] graph;
  return result;
}

/**
 * @brief: learn the weights or probability tables TODO:?
 * @brief: get each node's conditional probability table
 */
void Network::LearnParamsKnowStructCompData(const Dataset *dts, int alpha, bool print_params){
  cout << "==================================================" << '\n'
       << "Begin learning parameters with known structure and complete data." << '\n'
       << "Laplace smoothing param: alpha = " << alpha << endl;

  struct timeval start, end;
  double diff;// j is a node index and also an array index
  gettimeofday(&start,NULL);

  int num_cores = omp_get_num_procs();
  omp_set_num_threads(num_cores);
  int max_work_per_thread = (dts->num_vars + num_cores - 1) / num_cores;
  #pragma omp parallel
  {
    int thread_id = omp_get_thread_num();

    // a thread for one or more nodes
    for (int i = max_work_per_thread * thread_id;
         i < max_work_per_thread * (thread_id + 1) && i < dts->num_vars;
         ++i) {
//    for (int i=0; i<dts->num_vars; ++i) {
      // for each variable/node, update probability table of (node | parent configurations)
      DiscreteNode *this_node = dynamic_cast<DiscreteNode*>(FindNodePtrByIndex(i));   // todo: support continuous node
      this_node->SetLaplaceSmooth(alpha);

      for (int s = 0; s < dts->num_instance; ++s) { // for each instance
        // create the vector "values" by copying the array "dts->dataset_all_vars[s]"
        vector<int> values = vector<int>(dts->dataset_all_vars[s], dts->dataset_all_vars[s] + dts->num_vars);
        //convert an instance to discrete configuration
        DiscreteConfig instance; //set<pair<int, int> >
        for (int j = 0; j < values.size(); ++j) { // for each variable of this instance
          instance.insert(pair<int, int>(j, values.at(j)));
        }
        this_node->AddInstanceOfVarVal(instance);//an instance affects all the nodes in the network, because the instance here is dense.
      }
    }
  }   // end of: #pragma omp parallel
  cout << "==================================================" << '\n'
       << "Finish training with known structure and complete data." << endl;

  if (print_params) {
    cout << "==================================================" << '\n'
         << "Each node's conditional probability table: " << endl;
    for (const auto &id_node_ptr : map_idx_node_ptr) {  // For each node
      dynamic_cast<DiscreteNode*>(id_node_ptr.second)->PrintProbabilityTable();
    }
  }

  gettimeofday(&end,NULL);
  diff = (end.tv_sec-start.tv_sec) + ((double)(end.tv_usec-start.tv_usec))/1.0E6;
  setlocale(LC_NUMERIC, "");
  cout << "==================================================" << '\n'
       << "The time spent to learn the parameters is " << diff << " seconds" << endl;

}

/**
 * @brief: get the number of parameters of the network, based on probability tables
 *         (and other parameters in continuous variables)
 */
int Network::GetNumParams() const {
  int result = 0;
  for (const auto &i_n : map_idx_node_ptr) { // TODO: function "FindNodePtrByIndex"
    result += i_n.second->GetNumParams(); // TODO: Node::GetNumParams is a virtual function
  }
  return result;
}

/**
 * @brief: clear structure; mainly for reuse some content for fast testing
 *
 * Important: may have bugs! TODO
 */
void Network::ClearStructure() {
  for (auto &i_n_p : this->map_idx_node_ptr) {
    i_n_p.second->ClearParams();
    i_n_p.second->ClearParents();
    i_n_p.second->ClearChildren();
  }
}

/**
 * @brief: clear structure; mainly for reuse some content for fast testing
 *
 * Important: may have bugs! TODO
 */
void Network::ClearParams() {
  for (auto &i_n_p : this->map_idx_node_ptr) { // TODO: function "FindNodePtrByIndex"
    i_n_p.second->ClearParams(); // TODO: Node::ClearParams is a virtual function
  }
}

/**
 * @brief: this is a virtual function;
 * The performance of variable elimination relies heavily on the elimination ordering
 */
vector<int> Network::SimplifyDefaultElimOrd(DiscreteConfig evidence) {//TODO: use C++ pure virtual function
  fprintf(stderr, "Function [%s] not implemented yet!", __FUNCTION__);
  exit(1);
}

/**
 * @brief: Factor is a class; construct a set of factors using a node and an elimination order; used in Junction Tree
 * @param Z: a set of nodes identified by IDs; Z is the elimination order.
 * @param Y: a node
 * @return: a set of Factors, where each factor corresponds to a node; first Y (target node), then nodes ordered by Z
 */
vector<Factor> Network::ConstructFactors(vector<int> Z, Node *Y) {
  vector<Factor> factors_list;
  factors_list.push_back(Factor(dynamic_cast<DiscreteNode*>(Y), this));
  for (int i = 0; i < Z.size(); ++i) {
    Node* n = FindNodePtrByIndex(Z.at(i));
    factors_list.push_back(Factor(dynamic_cast<DiscreteNode*>(n), this));
  }
  return factors_list;
}

/**
 * @brief: update the probabilities/weights of all the factors related to the nodes between the target node and node with the evidence/observation
 * @param factors_list: a list factors related to the nodes between the target node and the node with evidence/observation
 * @param E: a new observation
 * @param all_related_vars: all the related variables between the target node and the node with the evidence/observation
 */
void Network::LoadEvidenceIntoFactors(vector<Factor> *factors_list,
                                      DiscreteConfig E, set<int> all_related_vars) {

  // TODO: check openmp
  // I do not know why this function cannot use omp to parallel.
  // If I set number of threads more than 1, the accuracy will decrease!
//  int num_cores = omp_get_num_procs();
//  omp_set_num_threads(num_cores);
//  int max_work_per_thread = (factors_list->size()+num_cores-1)/num_cores;
//  #pragma omp parallel
  {
//    for (int i = omp_get_thread_num() * max_work_per_thread;
//         i < (omp_get_thread_num()+1) * max_work_per_thread && i < factors_list->size();
//         ++i) {

    for (int i = 0; i < factors_list->size(); ++i) {
      Factor &f = factors_list->at(i);   // For each factor. "std::vector::at" returns reference.
      for (const auto &e : E) {  // For each node's observation in E
        // If this factor is related to this node with observation
        if (f.related_variables.find(e.first) != f.related_variables.end()) {
          /** For example:  X --> Y (evidence/obs) --> Z --> A (target node) --> B --> C --> E --> F (evidence/observation) --> G
           * Only the factors of Z and G are true in the above if statement, because Z and G have configurations containing evidence.
           * **/
          // Update each row of map_potentials
          for (const auto &comb : f.set_disc_config) {
            // If this entry is not compatible to the evidence -> reduction
            if (comb.find(e) == comb.end()) {
              f.map_potentials[comb] = 0;
            }
          }
        }//end if the factor is related to the node
      }

      //--------------------------------------------------------------------------------
      // Fix bug Step 2/2.
      // This block is to fix the bug occurring when the target node
      // is not the root and the variable elimination order do not start at root.
      // For example:  X--> Y (evidence/obs) --> Z --> A (target node) --> B --> C --> E --> F (evidence/observation; not important) --> G
      //
      // For example: A --> B --> C (old example)
      // When given the markov blanket of node "C", which is "{B}",
      // there is no need to calculate the other nodes, which is "{A}".
      // However, when using this function,
      // the parent of parent of this node, which is "A",
      // still appears in the constructed factor of the parent which is "B".
      // my understanding:
      // maybe some variables are not in the elimination order, but they are also existed in some related factors,
      // so what we need to do is to eliminate these variables before the main variable elimination (VE) process.
      set<int> related_vars_of_f = f.related_variables;
      for (auto &v : related_vars_of_f) { // for each related variables of the factor f
        if (all_related_vars.find(v) == all_related_vars.end()) {
          f = f.SumOverVar(v);//X and G will be sum over, i.e. eliminate X and G given the evidence/observations
        }
      }
      //--------------------------------------------------------------------------------
    }

  }   // end of: #pragma omp parallel
}

/**
 * @brief: variable elimination (VE): gradually eliminate variables until only one (i.e. the target node) left
 */
Factor Network::SumProductVarElim(vector<Factor> factors_list, vector<int> elim_order) {
  for (int i = 0; i < elim_order.size(); ++i) { // consider each node i according to the elimination order
    vector<Factor> tempFactorsList;
    Node* nodePtr = FindNodePtrByIndex(elim_order.at(i));

    // Move every factor that is related to the node elim_order[i] from factors_list to tempFactorsList.
    /*
     * Note: This for loop does not contain "++it" in the parentheses.
     *      When finding "elim_order[i]" during the traverse, the iterator "it" points to this vector.
     *      We use "erase" to delete this element from "factors_list" via iterator "it";
     *      the function "erase" returns an iterator pointing to the next element of the delete element.
     */
    for (auto it = factors_list.begin(); it != factors_list.end(); /* no ++it */) {
      // if the factor "it" is related to the node "elim_order[i]" (i.e., the node to be eliminated now)
      if ((*it).related_variables.find(nodePtr->GetNodeIndex())!=(*it).related_variables.end()) {
        tempFactorsList.push_back(*it);
        factors_list.erase(it);
        continue;
      }
      else {
        ++it;
      }
    }

    //merge all the factors in tempFactorsList into one factor
    while(tempFactorsList.size()>1) {
      // every time merge two factors into one
      Factor temp1, temp2, product;
      temp1 = tempFactorsList.back(); // get the last element
      tempFactorsList.pop_back();  // remove the last element
      temp2 = tempFactorsList.back();
      tempFactorsList.pop_back();

      product = temp1.MultiplyWithFactor(temp2);
      tempFactorsList.push_back(product);
    }

    // eliminate variable "nodePtr" by summation of the factor "tempFactorsList.back()" over "nodePtr"
    Factor newFactor = tempFactorsList.back().SumOverVar(dynamic_cast<DiscreteNode*>(nodePtr));
    factors_list.push_back(newFactor);
  } // finish eliminating variables and only one variable left

  /*
   *   If we are calculating a node's posterior probability given evidence about its children,
   *   then when the program runs to here,
   *   the "factors_list" will contain several factors
   *   about the same node which is the query node Y.
   *   When it happens, we need to multiply these several factors.
   */
  while (factors_list.size()>1) {//TODO: reuse the code of the while loop above
    Factor temp1, temp2, product;
    temp1 = factors_list.back();
    factors_list.pop_back();
    temp2 = factors_list.back();
    factors_list.pop_back();
    product = temp1.MultiplyWithFactor(temp2);
    factors_list.push_back(product);
  }

  // After all the processing shown above, the only remaining factor is the factor about Y.
  return factors_list.back();
}

/**
 * @brief: infer the marginal probability of the target node, given the evidence
 * @param evid: the given evidence
 * @param target_node: the node whose probability needs to be updated given the evidence/observation.
 * @param elim_order: elimination order
 * @return the factor which contains the marginal probability table of the target node
 */
Factor Network::VarElimInferReturnPossib(DiscreteConfig evid, Node *target_node, vector<int> elim_order) {

  if (elim_order.empty()) {
    elim_order = SimplifyDefaultElimOrd(evid);
  }

  /**
   * the factor list corresponds to all the nodes which are between the target node and the observation/evidence.
   * **/
  vector<Factor> factorsList = ConstructFactors(elim_order, target_node);

  //--------------------------------------------------------------------------------
  // // Fix bug Step 1/2.
  // This block is to fix the bug occurring when the target node
  // is not the root and the variable elimination order do not start at root.
  // For example:  X--> Y (evidence/obs) --> Z --> A (target node) --> B --> C --> E --> F (evidence/observation; not important) --> G
  //
  // For example: A --> B --> C (old example)
  // When given the markov blanket of node "C", which is "{B}",
  // there is no need to calculate the other nodes, which is "{A}".
  // However, when using this function,
  // the parent of parent of this node, which is "A",
  // still appears in the constructed factor of the parent which is "B".
  // my understanding:
  // maybe some variables are not in the elimination order, but they are also existed in some related factors,
  // so what we need to do is to eliminate these variables before the main variable elimination (VE) process.
  set<int> all_related_vars;
  all_related_vars.insert(target_node->GetNodeIndex());
  for (int i = 0; i < elim_order.size(); ++i) {
    all_related_vars.insert(elim_order.at(i));
  }
  //--------------------------------------------------------------------------------

  //load evidence function below returns a factorsList with fewer configurations.
  LoadEvidenceIntoFactors(&factorsList, evid, all_related_vars);

  //compute the probability table of the target node
  Factor target_node_factor = SumProductVarElim(factorsList, elim_order);
  target_node_factor.Normalize();

  return target_node_factor;
}

/**
 * @brief: for inference given a target variable id and an (full) evidence/observation.
 * @param evidence: full evidence/observation (i.e. a dense instance)
 * @return map: key is the possible value of the target node; value is the probability of the target node with a specific value
 */
 // TODO: why marginal? i think it is to compute joint distribution
map<int, double> Network::GetMarginalProbabilities(int target_var_index, DiscreteConfig evidence) {
  if (!this->pure_discrete) {
    fprintf(stderr, "Function [%s] only works on pure discrete networks!", __FUNCTION__);
    exit(1);
  }
  /**
   * Example: X --> Y, and X is the target node; X = {0, 1}, Y={0,1}.
   */

  map<int, double> result;

  DiscreteNode *target_node = (DiscreteNode*) FindNodePtrByIndex(target_var_index);
  auto vec_complete_instance_values = SparseInstanceFillZeroToDenseInstance(evidence); // vector<int>

  // compute the probability of each possible value of the target node
  for (int i = 0; i < target_node->GetDomainSize(); ++i) { // for each possible value of the target node (e.g. X=0)
    // add the ith value of the target node into "vec_complete_instance_values" and "evidence"
    vec_complete_instance_values.at(target_var_index) = target_node->vec_potential_vals.at(i);
    // TODO: "evidence" is not used
    evidence.insert(DiscVarVal(target_var_index, target_node->vec_potential_vals.at(i)));

    // use chain rule to get the joint distribution (multiply "num_nodes" factors)
    result[i] = 0;
    for (int j = 0; j < num_nodes; ++j) { // for each node
      // e.g. Y
      DiscreteNode *node_j = (DiscreteNode*) FindNodePtrByIndex(j);
      // e.g. X = 0. related to the target node X, then the value is the ith possible value i.e. 0.
      DiscreteConfig par_config = node_j->GetDiscParConfigGivenAllVarValue(vec_complete_instance_values);

      // TODO: merge if/else
      double temp_prob = 0;
      if (j == target_var_index) {//e.g. false
        temp_prob = target_node->GetProbability(target_node->vec_potential_vals.at(i), par_config);
      }
      else {//Y=1 given X = 0
        int observe_value = vec_complete_instance_values.at(j); // e.g. observation of Y
        temp_prob = node_j->GetProbability(observe_value, par_config); // e.g. P(Y=observation|X=0)
      }

      // note: use log!! so it is not "+=", it is in fact "*="..
      result[i] += log(temp_prob);
    }

    // remove the ith value of the target node
    // TODO: "evidence" is not used
    evidence.erase(DiscVarVal(target_var_index, target_node->vec_potential_vals.at(i)));
  }//end for each possible value of the target node

  for (int i = 0; i < target_node->GetDomainSize(); ++i) {
    result[i] = exp(result[i]); //the result[i] is computed in log scale
  }
  result = Normalize(result);
  return result;
}


/**
 * @brief: predict label given (partial or full observation) evidence
 * check "map_potentials", and the predict label is the one with maximum probability
 * @return label of the target variable
 */
int Network::PredictUseVarElimInfer(DiscreteConfig evid, int target_node_idx, vector<int> elim_order) {
  Node *Y = FindNodePtrByIndex(target_node_idx);
  // get the factor (marginal probability) of the target node, given the evidence
  Factor F = VarElimInferReturnPossib(evid, Y, elim_order);

  // find the configuration with the maximum probability
  double max_prob = 0;
  DiscreteConfig comb_predict;
  for (auto &comb : F.set_disc_config) { // for each configuration of the related variables
    if (F.map_potentials[comb] > max_prob) {
      max_prob = F.map_potentials[comb];
      comb_predict = comb;
    }
  }
  int label_predict = comb_predict.begin()->second;
  return label_predict;
}

/**
 * @brief: predict the labels given different evidences
 * it just repeats the function above multiple times, and print the progress at the meantime
 * @param elim_orders: elimination order which may be different given different evidences due to the simplification of elimination order
 */
vector<int> Network::PredictUseVarElimInfer(vector<DiscreteConfig> evidences, int target_node_idx, vector<vector<int>> elim_orders) {
  int size = evidences.size();

  cout << "Progress indicator: ";
  int every_1_of_20 = size / 20; // used to print, print 20 times in total
  int progress = 0;

  if (elim_orders.empty()) {
    // Vector of size "size". Each element is an empty vector.
    elim_orders = vector<vector<int>> (size, vector<int>{});
  }

  vector<int> results(size, 0);
#pragma omp parallel for
  for (int i = 0; i < size; ++i) {
#pragma omp critical
    { ++progress; }
//    string progress_detail = to_string(progress) + '/' + to_string(size);
//    fprintf(stdout, "%s\n", progress_detail.c_str());
//    fflush(stdout);

    if (progress % every_1_of_20 == 0) {
      string progress_percentage = to_string((double)progress/size * 100) + "%...\n";
      fprintf(stdout, "%s\n", progress_percentage.c_str());
      fflush(stdout);
    }

    DiscreteConfig evidence = evidences.at(i);
    vector<int> elim_ord = elim_orders.at(i);
    int pred = PredictUseVarElimInfer(evidence, target_node_idx, elim_ord);
    results.at(i) = pred;
  }
  return results;
}

/**
 * @brief: predict label given evidence E and target variable id
 * @return label of the target variable
 */
int Network::PredictLabelBruteForce(DiscreteConfig E, int Y_index) {
  // get map "distribution"; key: possible value of Y_index; value: probability under evidence E and value of Y_index
  map<int, double> distribution = GetMarginalProbabilities(Y_index, E);
  // find the label which has the max probability
  int label_index = ArgMax(distribution);

  // convert index of the target value to label
  DiscreteNode* tempNode = ((DiscreteNode*)FindNodePtrByIndex(Y_index));
  int label_predict = tempNode->vec_potential_vals.at(label_index);
  return label_predict;
}

/**
 * @brief: predict the label of different evidences
 * it just repeats the function above multiple times, and print the progress at the meantime
 */
vector<int> Network::PredictUseSimpleBruteForce(vector<DiscreteConfig> evidences, int target_node_idx) {
  int size = evidences.size();

  cout << "Progress indicator: ";
  int every_1_of_20 = size / 20;
  int progress = 0;


  vector<int> results(size, 0);
#pragma omp parallel for
  for (int i = 0; i < size; ++i) {
#pragma omp critical
    { ++progress; }
//    string progress_detail = to_string(progress) + '/' + to_string(size);
//    fprintf(stdout, "%s\n", progress_detail.c_str());
//    fflush(stdout);

    if (progress % every_1_of_20 == 0) {
      string progress_percentage = to_string((double)progress/size * 100) + "%...\n";
      fprintf(stdout, "%s\n", progress_percentage.c_str());
      fflush(stdout);
    }

    int pred = PredictLabelBruteForce(evidences.at(i), target_node_idx);
    results.at(i) = pred;
  }
  return results;
}

/**
 * @brief: compute the accuracy for a classification problem.
 * @param dts: data set; this is a classification problem.
 */
double Network::EvaluateVarElimAccuracy(Dataset *dts) {

  cout << "==================================================" << '\n'
       << "Begin testing the trained network." << endl;

  struct timeval start, end;
  double diff;
  gettimeofday(&start,NULL);

  int m = dts->num_instance;

  int class_var_index = dts->class_var_index;

  // construct the test data set with labels
  vector<int> ground_truths;
  vector<DiscreteConfig> evidences;
  evidences.reserve(m);
  ground_truths.reserve(m);

  for (int i = 0; i < m; ++i) { // for each instance in the data set
    // construct a test data set by removing the class variable
    int e_num = num_nodes - 1;
    int *e_index = new int[e_num];
    int *e_value = new int[e_num];
    for (int j = 0; j < num_nodes; ++j) {
      if (j == class_var_index) {
        continue; // skip the class variable
      }
      e_index[j < class_var_index ? j : j - 1] = j;
      e_value[j < class_var_index ? j : j - 1] = dts->dataset_all_vars[i][j];
    }
    // convert to DiscreteConfig and construct the test set
    DiscreteConfig E = ArrayToDiscreteConfig(e_index, e_value, e_num);
    evidences.push_back(E);

    // construct the ground truth
    int g = dts->dataset_all_vars[i][class_var_index];
    ground_truths.push_back(g);

    delete[] e_index;
    delete[] e_value;
  }

  // predict the labels of the test instances
  vector<int> predictions = PredictUseVarElimInfer(evidences, class_var_index);
  double accuracy = Accuracy(ground_truths, predictions);
  cout << '\n' << "Accuracy: " << accuracy << endl;


  gettimeofday(&end,NULL);
  diff = (end.tv_sec-start.tv_sec) + ((double)(end.tv_usec-start.tv_usec))/1.0E6;
  setlocale(LC_NUMERIC, "");
  cout << "==================================================" << '\n'
       << "The time spent to test the accuracy is " << diff << " seconds" << endl;

  return accuracy;
}

//TODO: combine with the EvaluateVarElimAccuracy(Dataset *dts) function
double Network::EvaluateAccuracyGivenAllCompleteInstances(Dataset *dts) {

  cout << "==================================================" << '\n'
       << "Begin testing the trained network." << endl;

  struct timeval start, end;
  double diff;
  gettimeofday(&start,NULL);

  int m = dts->num_instance;

  int class_var_index = dts->class_var_index;

  // construct the test data set with labels
  vector<int> ground_truths;
  vector<DiscreteConfig> evidences;
  evidences.reserve(m);
  ground_truths.reserve(m);

  for (int i = 0; i < m; ++i) { // for each instance in the data set
    // construct a test data set by removing the class variable
    int e_num = num_nodes - 1;
    int *e_index = new int[e_num];
    int *e_value = new int[e_num];
    for (int j = 0; j < num_nodes; ++j) {
      if (j == class_var_index) {
        continue; // skip the class variable
      }
      e_index[j < class_var_index ? j : j - 1] = j;
      e_value[j < class_var_index ? j : j - 1] = dts->dataset_all_vars[i][j];
    }
    // convert to DiscreteConfig and construct the test set
    DiscreteConfig E = ArrayToDiscreteConfig(e_index, e_value, e_num);
    evidences.push_back(E);

    // construct the ground truth
    int g = dts->dataset_all_vars[i][class_var_index];
    ground_truths.push_back(g);

    delete[] e_index;
    delete[] e_value;
  }

  // predict the labels of the test instances
  // TODO: the only difference is the line below: use different function to obtain "predictions"
  vector<int> predictions = PredictUseSimpleBruteForce(evidences, class_var_index);
  double accuracy = Accuracy(ground_truths, predictions);
  cout << '\n' << "Accuracy: " << accuracy << endl;

  gettimeofday(&end,NULL);
  diff = (end.tv_sec-start.tv_sec) + ((double)(end.tv_usec-start.tv_usec))/1.0E6;
  setlocale(LC_NUMERIC, "");
  cout << "==================================================" << '\n'
       << "The time spent to test the accuracy is " << diff << " seconds" << endl;

  return accuracy;
}

//TODO: merge into EvaluateVarElimAccuracy(Dataset *dts)
double Network::EvaluateApproxInferAccuracy(Dataset *dts, int num_samp) {


  cout << "==================================================" << '\n'
       << "Begin testing the trained network." << endl;

  struct timeval start, end;
  double diff;
  gettimeofday(&start,NULL);

  int m = dts->num_instance;

  int class_var_index = dts->class_var_index;

  // draw "num_samp" samples TODO: difference
  // TODO: for the function below, the step of drawing samples is inside the step of approximate inference
  // TODO: it is better to use the same way
  vector<DiscreteConfig> samples = this->DrawSamplesByProbLogiSamp(num_samp);
  cout << "Finish drawing samples." << endl;

  // construct the test data set with labels
  vector<int> ground_truths;
  vector<DiscreteConfig> evidences;
  evidences.reserve(m);
  ground_truths.reserve(m);

  for (int i = 0; i < m; ++i) { // for each instance in the data set
    // construct a test data set by removing the class variable
    int e_num = num_nodes - 1;
    int *e_index = new int[e_num];
    int *e_value = new int[e_num];
    for (int j = 0; j < num_nodes; ++j) {
      if (j == class_var_index) {
        continue; // skip the class variable
      }
      e_index[j < class_var_index ? j : j - 1] = j;
      e_value[j < class_var_index ? j : j - 1] = dts->dataset_all_vars[i][j];
    }
    // convert to DiscreteConfig and construct the test set
    DiscreteConfig E = ArrayToDiscreteConfig(e_index, e_value, e_num);
    evidences.push_back(E);

    // construct the ground truth
    int g = dts->dataset_all_vars[i][class_var_index];
    ground_truths.push_back(g);

    delete[] e_index;
    delete[] e_value;
  }

  // predict the labels of the test instances TODO: difference in function
  vector<int> predictions = ApproxInferByProbLogiRejectSamp(evidences, class_var_index, samples);
  double accuracy = Accuracy(ground_truths, predictions);
  cout << '\n' << "Accuracy: " << accuracy << endl;

  gettimeofday(&end,NULL);
  diff = (end.tv_sec-start.tv_sec) + ((double)(end.tv_usec-start.tv_usec))/1.0E6;
  setlocale(LC_NUMERIC, "");
  cout << "==================================================" << '\n'
       << "The time spent to test the accuracy is " << diff << " seconds" << endl;

  return accuracy;
}

//TODO: merge with EvaluateVarElimAccuracy(Dataset *dts)
double Network::EvaluateLikelihoodWeightingAccuracy(Dataset *dts, int num_samp) {
  cout << "==================================================" << '\n'
       << "Begin testing the trained network." << endl;

  struct timeval start, end;
  double diff;
  gettimeofday(&start,NULL);

  int m = dts->num_instance;

  int class_var_index = dts->class_var_index;

  // construct the test data set with labels
  vector<int> ground_truths;
  vector<DiscreteConfig> evidences;
  evidences.reserve(m);
  ground_truths.reserve(m);

  for (int i = 0; i < m; ++i) { // for each instance in the data set
    // construct a test data set by removing the class variable
    int e_num = num_nodes - 1;
    int *e_index = new int[e_num];
    int *e_value = new int[e_num];
    for (int j = 0; j < num_nodes; ++j) {
      if (j == class_var_index) {
        continue; // skip the class variable
      }
      e_index[j < class_var_index ? j : j - 1] = j;
      e_value[j < class_var_index ? j : j - 1] = dts->dataset_all_vars[i][j];
    }
    // convert to DiscreteConfig and construct the test set
    DiscreteConfig E = ArrayToDiscreteConfig(e_index, e_value, e_num);
    evidences.push_back(E);

    // construct the ground truth
    int g = dts->dataset_all_vars[i][class_var_index];
    ground_truths.push_back(g);

    delete[] e_index;
    delete[] e_value;
  }

  // predict the labels of the test instances TODO: difference
  vector<int> predictions = ApproxinferByLikelihoodWeighting(evidences, class_var_index, num_samp);
  double accuracy = Accuracy(ground_truths, predictions);
  cout << '\n' << "Accuracy: " << accuracy << endl;


  gettimeofday(&end,NULL);
  diff = (end.tv_sec-start.tv_sec) + ((double)(end.tv_usec-start.tv_usec))/1.0E6;
  setlocale(LC_NUMERIC, "");
  cout << "==================================================" << '\n'
       << "The time spent to test the accuracy is " << diff << " seconds" << endl;

  return accuracy;
}

/**
 * @brief: for approximate inference; this function generate an instance using the network
 * @return an instance
 */
DiscreteConfig Network::GenerateInstanceByProbLogicSampleNetwork() {
  // Probabilistic logic sampling is a method
  // proposed by Max Henrion at 1988: "Propagating uncertainty in Bayesian networks by probabilistic logic sampling" TODO: double-check

  DiscreteConfig instance;
  // Cannot use OpenMP, because must draw samples in the topological ordering.
  // TODO: if we directly use "value", or use index to randomly pick one possible value of "n_p",
  // TODO: then we do not need to use the topological ordering...
  for (const auto &index : this->GetTopoOrd()) { // for each node following the topological ordering
    Node *n_p = FindNodePtrByIndex(index);
    int drawn_value = dynamic_cast<DiscreteNode*>(n_p)->SampleNodeGivenParents(instance); // todo: support continuous nodes
    instance.insert(pair<int,int>(index, drawn_value));
  }
  return instance;
}

/**
 * @brief: draw multiple instances
 * it repeats the function above "num_samp" times to select "num_samp" samples
 */
vector<DiscreteConfig> Network::DrawSamplesByProbLogiSamp(int num_samp) {
  vector<DiscreteConfig> samples;
  samples.reserve(num_samp);
#pragma omp parallel for
  for (int i = 0; i < num_samp; ++i) {
    DiscreteConfig samp = this->GenerateInstanceByProbLogicSampleNetwork();
#pragma omp critical
    { samples.push_back(samp); }
  }
  return samples;
}

/**
 * @brief: for approximate inference; this function generate an instance using the network.
 * @param evidence: this parameter is optional.
 * @return an instance and a weight based on likelihood
 */
 // TODO: can be merge with ""GenerateInstanceByProbLogicSampleNetwork""?
pair<DiscreteConfig, double> Network::DrawOneLikelihoodWeightingSample(const DiscreteConfig &evidence) {
  DiscreteConfig instance;
  double weight = 1;
  // SHOULD NOT use OpenMP, because must draw samples in the topological ordering.
  for (const auto &index : this->GetTopoOrd()) {  // For each node.
    Node *n_p = FindNodePtrByIndex(index);
    bool observed = false;
    for (const auto &var_val : evidence) {  // Find if this variable node is in evidence
      if (index == var_val.first) { // if "n_p" is in "evidence"
        observed = true;
        // Set the sample value to be the same as the evidence.
        instance.insert(pair<int, int>(index, var_val.second));

        // todo: check the correctness of this implementation for the 4 lines below
        // TODO: check "weight"
        // Update the weight.
        DiscreteConfig parents_config = dynamic_cast<DiscreteNode*>(n_p)->GetDiscParConfigGivenAllVarValue(instance);
        double cond_prob = dynamic_cast<DiscreteNode*>(n_p)->GetProbability(var_val.second, parents_config);
        weight *= cond_prob;
        break;
      }
    }
    if (!observed) { // if "n_p" is not in "evidence"
      int drawn_value = dynamic_cast<DiscreteNode*>(n_p)->SampleNodeGivenParents(instance);   // todo: Consider continuous nodes
      instance.insert(pair<int,int>(index, drawn_value));
    }
  }
  return pair<DiscreteConfig, double>(instance, weight);
}

/**
 * @brief: draw multiple instances for approximate inference
 * it repeats the function above "num_samp" times to select "num_samp" samples
 */
vector<pair<DiscreteConfig, double>> Network::DrawSamplesByLikelihoodWeighting(const DiscreteConfig &evidence,
                                                                               int num_samp) {
  vector<pair<DiscreteConfig, double>> results;
#pragma omp parallel for
  for (int i=0; i<num_samp; ++i) {
    auto samp = DrawOneLikelihoodWeightingSample(evidence);
#pragma omp critical
    { results.push_back(samp); }
  }
  return results;
}

/**
 * @brief: perform inference based on the drawn sample with weights.
 * @param node_index: target node which needs to compute marginal probabilities
 */
Factor Network::CalcuMargWithLikelihoodWeightingSamples(const vector<pair<DiscreteConfig, double>> &samples,
                                                        const int &node_index) {
  map<int, double> value_weight;
  DiscreteNode *target_node = dynamic_cast<DiscreteNode*>(this->FindNodePtrByIndex(node_index));

  // Initialize the map.
  for (int i=0; i<target_node->GetDomainSize(); ++i) {
    value_weight[target_node->vec_potential_vals.at(i)] = 0;
  }

  // Calculate the sum of weight for each value. Un-normalized.
  for (const auto &samp : samples) { // for each sample
    for (const auto &feature_value : samp.first) { // for each variable-value in the sample
      if (node_index == feature_value.first) { // find the target variable
        // accumulate the weight of each possible value of the target value
        value_weight[feature_value.second] += samp.second;
        break;
      }
    }
  }

  // Normalization. TODO: it seems to have another function
  double denominator = 0;
  for (const auto &kv : value_weight) {
    denominator += kv.second;
  }
  for (auto &kv : value_weight) {
    kv.second /= denominator;
  }

  // Construct a factor to return
  // 1. related variables; is the target node
  set<int> rv;
  rv.insert(node_index);
  // 2. all the configurations of the related variables
  set<DiscreteConfig> sc;
  for (int i=0; i<target_node->GetDomainSize(); ++i) {
    DiscreteConfig c;
    c.insert(pair<int, int>(node_index, target_node->vec_potential_vals.at(i)));
    sc.insert(c);
  }
  // 3. map<DiscreteConfig, double>; the weight/potential of each discrete config
  map<DiscreteConfig, double> mp;
  for (const auto &c : sc) {
    int value = (*c.begin()).second;
    mp[c] = value_weight[value];
  }
  Factor f(rv, sc, mp);
  return f;
}

/**
 * @brief: predice a label given a (part/full) evidence
 * @param e: evidence
 * @param node_index: target node index
 * @param num_samp: sample size
 * @return label
 */
int Network::ApproxinferByLikelihoodWeighting(DiscreteConfig e, const int &node_index, const int &num_samp) {
  vector<pair<DiscreteConfig, double>> samples_weight = this->DrawSamplesByLikelihoodWeighting(e, num_samp);
  Factor f = CalcuMargWithLikelihoodWeightingSamples(samples_weight, node_index);

  // Find the argmax. TODO: it seems to have ArgMax function
  DiscreteConfig c;
  double max = -1;
  for (const auto &kv : f.map_potentials) {
    if (kv.second > max) { // find the max probability
      c = kv.first; // mark the configuration
      max = kv.second; // mark the probability
    }
  }
  return (*c.begin()).second; // return one value of the target node that has the largest probability
}

/**
 * @brief: approximate inference given different evidences
 * it just repeats the function above multiple times, and print the progress at the meantime
 * @return a vector of labels for each evidence
 */
vector<int> Network::ApproxinferByLikelihoodWeighting(vector<DiscreteConfig> evidences,
                                                      const int &target_node_idx, const int &num_samp) {
  int size = evidences.size();

  cout << "Progress indicator: ";
  int every_1_of_20 = size / 20;
  int progress = 0;

  vector<int> results(size, 0);
#pragma omp parallel for
  for (int i = 0; i < size; ++i) {
#pragma omp critical
    { ++progress; }
//    string progress_detail = to_string(progress) + '/' + to_string(size);
//    fprintf(stdout, "%s\n", progress_detail.c_str());
//    fflush(stdout);

    if (progress % every_1_of_20 == 0) {
      string progress_percentage = to_string((double)progress/size * 100) + "%...\n";
      fprintf(stdout, "%s\n", progress_percentage.c_str());
      fflush(stdout);
    }

    int pred = ApproxinferByLikelihoodWeighting(evidences.at(i), target_node_idx, num_samp);
    results.at(i) = pred;
  }
  return results;
}

/**
 * @brief: get the Markov Blanket of a node
 */
set<int> Network::GetMarkovBlanketIndexesOfNode(Node *node_ptr) {
  // node: set does not contain repeated elements
  set<int> markov_blanket_node_index;

  // Add parents.
  for (auto &par_ptr : GetParentPtrsOfNode(node_ptr->GetNodeIndex())) {
    markov_blanket_node_index.insert(par_ptr->GetNodeIndex());
  }

  // Add children and parents of children.
  for (auto &chil_ptr : GetChildrenPtrsOfNode(node_ptr->GetNodeIndex())) {
    markov_blanket_node_index.insert(chil_ptr->GetNodeIndex());
    for (auto &par_chil_ptr : GetParentPtrsOfNode(chil_ptr->GetNodeIndex())) {
      markov_blanket_node_index.insert(par_chil_ptr->GetNodeIndex());
    }
  }

  markov_blanket_node_index.erase(node_ptr->GetNodeIndex());

  return markov_blanket_node_index;
}

/**
 * @brief: draw multiple instances
 * @param num_samp: the number of instances to draw
 * @param num_burn_in: a terminology in MCMC and Gibbs sampling; the number of instances drawn at the beginning to be ignored
 * @return a set of instances
 */
vector<DiscreteConfig> Network::DrawSamplesByGibbsSamp(int num_samp, int num_burn_in) {

  vector<DiscreteConfig> samples;
  samples.reserve(num_samp);

  // randomly pick one sample
  DiscreteConfig single_sample = this->GenerateInstanceByProbLogicSampleNetwork();

  auto it_idx_node = this->map_idx_node_ptr.begin(); // begin at the first node

  // Need burning in.
//  #pragma omp parallel for
  for (int i = 1; i < num_burn_in + num_samp; ++i) {//draw instances

    Node *node_ptr = (*(it_idx_node++)).second;
    if (it_idx_node == map_idx_node_ptr.end()) {
      it_idx_node = this->map_idx_node_ptr.begin();
    }

    set<int> markov_blanket_node_index = GetMarkovBlanketIndexesOfNode(node_ptr);

    // construct the markov blanket from the picked "single_sample"
    // i.e., filter out the variable-values that are not in the markov blanket
    DiscreteConfig markov_blanket;
    for (auto &p : single_sample) { // for each variable-value of the picked "single_sample"
      // check if the variable is in the Markov Blanket
      if (markov_blanket_node_index.find(p.first) != markov_blanket_node_index.end()) {
        markov_blanket.insert(p);
      }
    }

    // obtain a value of a variable given the Markov Blanket
    int value_index = SampleNodeGivenMarkovBlanketReturnValIndex(node_ptr, markov_blanket);

    // replace the value of the previous instance with the new value
    for (auto p : single_sample) {
      if (p.first == node_ptr->GetNodeIndex()) {
        single_sample.erase(p);
        p.second = dynamic_cast<DiscreteNode*>(node_ptr)->vec_potential_vals.at(value_index);
        single_sample.insert(p);
        break;
      }
    }

    // After burning in, we can store the samples now.
    #pragma omp critical
    { if (i >= num_burn_in) { samples.push_back(single_sample); } }
  }

  return samples;
}

/**
 * @brief: obtain a value index given the Markov Blanket
 * @param node_ptr: target node
 * @param markov_blanket includes (i) direct parents, (ii) direct children and (iii) direct parents of direct children of the target node
 * @return value index of the target node
 */
int Network::SampleNodeGivenMarkovBlanketReturnValIndex(Node *node_ptr, DiscreteConfig markov_blanket) {
  //use the Markov blanket to serve as the elimination order
  //int num_elim_ord = markov_blanket.size();
  vector<int> var_elim_ord;
  var_elim_ord.reserve(markov_blanket.size());
  for (auto &n_v : markov_blanket) {
    var_elim_ord.push_back(n_v.first);
  }

  // TODO: the same problem with "DiscreteNode::SampleNodeGivenParents"
  //obtain the marginal probabilities of the target node
  Factor f = VarElimInferReturnPossib(markov_blanket, node_ptr, var_elim_ord);

  //use the marginal probabilities of the target node for sampling
  vector<int> weights;
  for (int i = 0; i < dynamic_cast<DiscreteNode*>(node_ptr)->GetDomainSize(); ++i) {//for each possible value of the target node
    DiscreteConfig temp;
    temp.insert(pair<int,int>(node_ptr->GetNodeIndex(),
                              dynamic_cast<DiscreteNode*>(node_ptr)->vec_potential_vals.at(i)));
    weights.push_back(f.map_potentials[temp]*10000);//the marginal probability is converted into int
  }

  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  default_random_engine rand_gen(seed);
  discrete_distribution<int> this_distribution(weights.begin(),weights.end());
  return this_distribution(rand_gen); // randomly pick an index and return
}

/**
 * @brief: given a set of instances, evidence; the same sample with different evidence can lead to different probabilities
 * @param e: evidence
 * @param node: target node
 * @param samples: input
 * @return label of the target node
 */
int Network::ApproxInferByProbLogiRejectSamp(DiscreteConfig e, Node *node, vector<DiscreteConfig> &samples) {

  // obtain the possible discrete configuration of the target node
  // it just converts "vec_potential_vals" (vector<int>) into "possb_value" (set<pair<int, int> >)
  // where the first "int" is always the node id of the target node
  DiscreteConfig possb_values;
  for (int i = 0; i < dynamic_cast<DiscreteNode*>(node)->GetDomainSize(); ++i) {
    possb_values.insert(pair<int,int>(node->GetNodeIndex(),
                                      dynamic_cast<DiscreteNode*>(node)->vec_potential_vals.at(i)));
  }

  // compute the statistics of the instances which are consistent to the target node.
  int num_possible_value = possb_values.size();
  assert(dynamic_cast<DiscreteNode*>(node)->GetDomainSize() == num_possible_value);
  int *count_each_value = new int[num_possible_value](); // 1d array of size "num_possible_value"
  int num_valid_sample = 0;
  for (const auto &samp : samples) { // for each sample
    if(!Conflict(&e, &samp)) { // if the sample is consistent to the evidence
      ++num_valid_sample; //  it is a valid sample
      // for each possible value of the target node, check which one is consistent to the sample
      for (const auto &pv : possb_values) {
        if (samp.find(pv) != samp.end()) { // find the value of the target node of the sample
          ++count_each_value[pv.second]; // update the counter of the value that the sample has
          break;
        }
      }
    }
  }

  // If there is no valid sample, just take a random guess.
  if (num_valid_sample==0) {
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    default_random_engine rand_gen(seed);
    uniform_int_distribution<int> this_distribution(0,dynamic_cast<DiscreteNode*>(node)->GetDomainSize()-1);
    return dynamic_cast<DiscreteNode*>(node)->vec_potential_vals.at(this_distribution(rand_gen));
  }

  // Find the argmax.; the label which appears most frequently in the input set of instances. // TODO: function?
  int label_index_predict = -1;
  int max_occurred = 0;
  for (int i=0; i<dynamic_cast<DiscreteNode*>(node)->GetDomainSize(); ++i) {
    if (label_index_predict == -1 || count_each_value[i] > max_occurred) {
      label_index_predict = i; // mark the index
      max_occurred = count_each_value[i]; // mark the counter
    }
  }

  // Return the predicted label instead of the index.
  return dynamic_cast<DiscreteNode*>(node)->vec_potential_vals.at(label_index_predict);
}

//refer to the same function with node_ptr as input
int Network::ApproxInferByProbLogiRejectSamp(DiscreteConfig e, int node_index, vector<DiscreteConfig> &samples) {
  return ApproxInferByProbLogiRejectSamp(e, FindNodePtrByIndex(node_index), samples);
}

/**
 *  @brief: reuse the samples for inference, while the evidence/observation may be different.
 *  it just repeats the function above multiple times, and print the progress at the meantime
 *  drawing a new set of instances is often expensive. This function makes use of reusing the pre-drawn instances.
 */
vector<int> Network::ApproxInferByProbLogiRejectSamp(vector<DiscreteConfig> evidences, int node_idx, vector<DiscreteConfig> &samples) {
  int size = evidences.size();

  cout << "Progress indicator: ";
  int every_1_of_20 = size / 20;
  int progress = 0;

  vector<int> results(size, 0);
#pragma omp parallel for
  for (int i = 0; i < size; ++i) {
#pragma omp critical
    { ++progress; }
//    string progress_detail = to_string(progress) + '/' + to_string(size);
//    fprintf(stdout, "%s\n", progress_detail.c_str());
//    fflush(stdout);

    if (progress % every_1_of_20 == 0) {
      string progress_percentage = to_string((double)progress/size * 100) + "%...\n";
      fprintf(stdout, "%s\n", progress_percentage.c_str());
      fflush(stdout);
    }

    int pred = ApproxInferByProbLogiRejectSamp(evidences.at(i), node_idx, samples);
    results.at(i) = pred;
  }
  return results;
}


/**
 * @brief: compute F(node, candidate_parents) using dynamic programming
 * used in Ott's algorithm for structure learning
 * the paper chooses minimum score but this program chooses maximum score, this depends on the specific scoring function
 * @param dynamic_program_for_F: map<Node*, map<set<Node*>, double>>, where Node* refers to "node", set<Node*> refers to "candidate_parents"
 * @return pair<double, set<Node*>>: function value, candidate_parents
 */
pair<double, set<Node*>> Network::F(Node *node,
        set<Node*> &candidate_parents,
        Dataset *dts,
        map<Node*, map<set<Node*>, double>> &dynamic_program_for_F) {

  map<set<Node*>, double> &this_node_dynamic_program = dynamic_program_for_F[node];

  // look up the table and return the value
  if (this_node_dynamic_program.find(candidate_parents) != this_node_dynamic_program.end()) {
    return pair<double, set<Node*>>(this_node_dynamic_program[candidate_parents], candidate_parents);
  }

  pair<double, set<Node*>> max_score_parents;
  max_score_parents.first = -DBL_MAX;

  // Look up the dynamic programming table.
  for (auto n : candidate_parents) {
    // recursively compute F of (node, candidate_parents_temp), i.e., F(node, candidate_parents-{n})
    set<Node*> candidate_parents_temp = candidate_parents;
    candidate_parents_temp.erase(n);

    // if F(node, candidate_parents-{n}) has not been computed
    if (this_node_dynamic_program.find(candidate_parents_temp) == this_node_dynamic_program.end()) {
      this_node_dynamic_program[candidate_parents_temp] =
              F(node, candidate_parents_temp, dts, dynamic_program_for_F).first;
    }

    // find the maximum of F(node, candidate_parents-{n})
    if (this_node_dynamic_program[candidate_parents_temp] > max_score_parents.first) {
      max_score_parents.first = this_node_dynamic_program[candidate_parents_temp];
      max_score_parents.second = candidate_parents_temp;
    }
  }

  // if F(node, candidate_parents) has not been computed
  if (this_node_dynamic_program.find(candidate_parents)==this_node_dynamic_program.end()) {

    // construct a network with node and candidate_parents
    DiscreteNode node_copy = *dynamic_cast<DiscreteNode*>(node);
    vector<DiscreteNode> candidate_parents_copy;
    for (auto n : candidate_parents) {
      candidate_parents_copy.push_back(*(DiscreteNode*)(n));
    }

    Network temp_net;
    temp_net.AddNode(&node_copy);
    for (auto n : candidate_parents_copy) {
      temp_net.AddNode(&n);
      temp_net.SetParentChild(&n, &node_copy);
    }
    temp_net.GenDiscParCombsForAllNodes();

    // compute the scoring function s(node, candidate_parents)
    ScoreFunction sf(&temp_net, dts);
    this_node_dynamic_program[candidate_parents] = sf.ScoreForNode(&node_copy,"log BDeu");

  }

  // find the maximum and get F(node, candidate_parents)
  if (this_node_dynamic_program[candidate_parents] > max_score_parents.first) {
    max_score_parents.first = this_node_dynamic_program[candidate_parents];
    max_score_parents.second = candidate_parents;
  }

  return max_score_parents;
}

/*!
 * @brief: compute Q^{set_nodes}(M(set_nodes)) = Q^{set_nodes}(topo_ord)
 * @param topo_ord = M(set_nodes)
 * @param dynamic_program_for_Q: map<pair<set<Node*>, vector<int>>,   pair<double, vector<pair<Node*, set<Node*>>>>>
 *              1. set<Node*>:      set_nodes
 *              2. vector<int>>:    topo_ord
 *              3. double:          function value
 *              4. Node*:           node for F
 *              5. set<Node*>:      candidate_parents for F
 * @return pair<double, vector<pair<Node*, set<Node*>>>>: function value, (one node in order, set_nodes)?
 */
pair<double, vector<pair<Node*, set<Node*>>>>
 Network::Q(set<Node*> &set_nodes,
         vector<int> topo_ord,
         Dataset *dts,
         map<Node*,   map<set<Node*>, double>> &dynamic_program_for_F,
         map<pair<set<Node*>, vector<int>>,   pair<double, vector<pair<Node*, set<Node*>>>>> dynamic_program_for_Q) {

  // look up the table and return the value
  pair<set<Node*>, vector<int>> to_find(set_nodes, topo_ord);
  if (dynamic_program_for_Q.find(to_find)!=dynamic_program_for_Q.end()) {
    return dynamic_program_for_Q[to_find];
  }

  double score = 0;
  vector<pair<Node*, set<Node*>>> vec_node_parents;

  // accumulate Q^{set_nodes}(topo_ord)
  // Q^{set_nodes}(topo_ord) = sum of F(n, pre nodes of n according to topo_ord) over n
  for (auto &n : set_nodes) { // for each node n
    set<Node*> candidate_parents;
    // insert all the pre nodes of n according to the topological ordering "topo_ord"
    for (const auto &i : topo_ord) {
      if (n->GetNodeIndex() == i) {
        break;
       }
      candidate_parents.insert(this->FindNodePtrByIndex(i));
    }
    pair<double, set<Node*>> best_score_parents = F(n, candidate_parents, dts, dynamic_program_for_F);

    score += best_score_parents.first;
    vec_node_parents.push_back(pair<Node*, set<Node*>>(n, best_score_parents.second));
  }

  return pair<double, vector<pair<Node*, set<Node*>>>>(score, vec_node_parents);
}

/*!
 * @brief: compute M(set_nodes) using dynamic programming
 * used in Ott's algorithm for structure learning
 * @param set_nodes: all node ptrs of the network
 * @return M(set_nodes), which is a subset of all nodes, # also equals to |set_nodes|
 */
vector<int> Network::M(set<Node*> &set_nodes,
        Dataset *dts,
        map<Node*, map<set<Node*>, double>> &dynamic_program_for_F,
        map<pair<set<Node*>, vector<int>>,   pair<double, vector<pair<Node*, set<Node*>>>>> dynamic_program_for_Q,
        map<set<Node*>, vector<int>> dynamic_program_for_M) {

  if (set_nodes.empty()) { // M(∅) = ∅
    return vector<int> {};
  }

  // look up the table and return the value
  if (dynamic_program_for_M.find(set_nodes) != dynamic_program_for_M.end()) {
    return dynamic_program_for_M[set_nodes];
  }

  Node *g_star = nullptr;
  double score = -DBL_MAX;
  for (auto n : set_nodes) {
    // recursively compute M of set_nodes_temp, i.e., M(set_nodes-{n})
    set<Node*> set_nodes_temp = set_nodes;
    set_nodes_temp.erase(n);
    vector<int> m_of_set_nodes_temp = M(set_nodes_temp, dts,
                                        dynamic_program_for_F, dynamic_program_for_Q,
                                        dynamic_program_for_M);

    // compute F + Q TODO: double-check
    double score_temp = F(n, set_nodes_temp, dts, dynamic_program_for_F).first
                        + Q(set_nodes_temp, m_of_set_nodes_temp, dts, dynamic_program_for_F, dynamic_program_for_Q).first;

    // find the maximum.
    if (score_temp > score) {
      score = score_temp;
      g_star = n; // get the g*
    }
  }

  // compute M(set_nodes) and return
  set<Node*> set_nodes_remove_g_star = set_nodes;
  set_nodes_remove_g_star.erase(g_star);
  // for 1 <= i < |set_nodes|, M(set_nodes)[i] = M(set_nodes - {g*})[i]
  vector<int> result = M(set_nodes_remove_g_star, dts, dynamic_program_for_F, dynamic_program_for_Q, dynamic_program_for_M);
  // M(set_nodes)[|set_nodes|] = g*
  result.push_back(g_star->GetNodeIndex());

  return result;

}

/**
 * @brief: learn the structure with Ott's algorithm.
     * Ott, Sascha, Seiya Imoto, and Satoru Miyano. "Finding optimal models for small gene networks."
     * In Biocomputing 2004, pp. 557-567. 2003.
 */
void Network::StructLearnByOtt(Dataset *dts, vector<int> topo_ord_constraint) {//TODO: double check correctness

  map<Node*, map<set<Node*>, double>> dynamic_program_for_F;
  map< pair<set<Node*>, vector<int>>,  pair<double, vector<pair<Node*, set<Node*>>> > > dynamic_program_for_Q;

  set<Node*> set_node_ptr_container;
  for (auto id_np : map_idx_node_ptr) {
    set_node_ptr_container.insert(id_np.second); // insert all node ptrs of the network
  }

  // if "topo_ord_constraint" = "best"; no order is provided (i.e. no constraint)
  if (topo_ord_constraint.empty() || topo_ord_constraint.size() != num_nodes) {
    map<set<Node*>, vector<int>> dynamic_program_for_M;
    vector<int> m_of_all_nodes = M(set_node_ptr_container, dts, dynamic_program_for_F, dynamic_program_for_Q,
                                   dynamic_program_for_M);
    topo_ord_constraint = m_of_all_nodes; // provide a order constraint
  }

  pair<double, vector<pair<Node*, set<Node*>>>> score_vec_node_parents = Q(set_node_ptr_container, topo_ord_constraint, dts, dynamic_program_for_F, dynamic_program_for_Q);
  vector<pair<Node*, set<Node*>>> vec_node_parents = score_vec_node_parents.second;

  cout << "==================================================" << '\n'
       << "Setting children and parents......" << endl;
  for (auto p : vec_node_parents) {
    Node *chi = p.first;
    for (auto par : p.second) {
      SetParentChild(par, chi);
    }
  }

  cout << "==================================================" << '\n'
       << "Generating parents combinations for each node......" << endl;
  GenDiscParCombsForAllNodes();
}

/**
 * @brief: convert sparse to dense
 */
 // todo: change a name?
vector<int> Network::SparseInstanceFillZeroToDenseInstance(DiscreteConfig &sparse_instance) {
  vector<int> complete_instance(this->num_nodes, 0);
  for (const auto p : sparse_instance) {
    complete_instance.at(p.first) = p.second;
  }
  return complete_instance;
}

/**
 * @brief: learning network structure using Weka's algorithm
 * https://github.com/Waikato/weka-3.8
 *      under: weka/src/main/java/weka/classifiers/bayes/net/search/global/K2.java
 *      or     weka/src/main/java/weka/classifiers/bayes/net/search/local/K2.java
 *
 * @key idea: the key idea is to add a parent to a node based on topo_ord_constraint,
 *            if adding the node as a parent results in increment of the score
 *            -- there are different scoring functions.
 */
void Network::StructLearnLikeK2Weka(Dataset *dts, vector<int> topo_ord_constraint, int max_num_parents) { //checked
  // todo: test the correctness
  // if "topo_ord_constraint" = "best"; no order is provided (i.e. no constraint)
  if (topo_ord_constraint.empty() || topo_ord_constraint.size() != num_nodes) {
    topo_ord_constraint.reserve(num_nodes);
    for (int i = 0; i < num_nodes; ++i) {
      topo_ord_constraint.push_back(i); // provide a order constraint: 1, 2, ..., num_nodes-1
    }
  }
  GenDiscParCombsForAllNodes(); // generate all possible parent configurations ("set_discrete_parents_combinations") for all nodes

//#pragma omp parallel for
  for (int i = 0; i < num_nodes; ++i) {

    int var_index = topo_ord_constraint.at(i);
    DiscreteNode *node = (DiscreteNode*) this->map_idx_node_ptr.at(var_index); // TODO: function "FindNodePtrByIndex"？

    bool ok_to_proceed = (node->GetNumParents() < max_num_parents);
    while (ok_to_proceed) {
      int best_par_index = -1;
      double best_extra_score = 0;

      for (int j = 0; j < i; ++j) { // j < i: traverse all pre nodes, satisfy the ordering constraint
        int par_index = topo_ord_constraint.at(j);

        // TODO: this function calculates old score and new score and returns the delta score as "extra_score"
        // TODO: why not
        // TODO:      1. directly compute the new score and compare the i scores to find the maximum, rather than compare the delta scores
        // TODO:      2. compute the old score before calling this function, because the i old scores seem to be the same
        double extra_score = CalcuExtraScoreWithModifiedArc(par_index, var_index, dts, "add", "log K2");//use K2 as scoring function
        if (extra_score > best_extra_score) { // find the max g() as the "best_extra_score" iteratively
          // TODO: may not need to addarc and deletearc
          // TODO: because if "AddArc" function returns false, then current "extra_score" equals to 0 and cannot be the "best_extra_score"
          if (this->AddArc(par_index, var_index)) {
            best_par_index = j;
            best_extra_score = extra_score;
            this->DeleteArc(par_index, var_index);
          }
        }
      }
      // if the maximum new score is less than or equal to the old score, stop
      if (best_par_index == -1) {
        ok_to_proceed = false;
      }
      // if the maximum new score is better than the old score, add this arc and continue
      else {
        this->AddArc(best_par_index, var_index);
        ok_to_proceed = (node->GetNumParents() < max_num_parents);
      }
    }

  }
}

//TODO: move to other class
double Network::Accuracy(vector<int> ground_truth, vector<int> predictions) {
  int size = ground_truth.size(),
      num_of_correct = 0,
      num_of_wrong = 0;
  for (int i = 0; i < size; ++i) {
    int g = ground_truth.at(i),
        p = predictions.at(i);
    if (g == p) {
      ++num_of_correct;
    } else {
      ++num_of_wrong;
    }
  }
  double accuracy = num_of_correct / (double)(num_of_correct+num_of_wrong);
  return accuracy;
}

#pragma clang diagnostic pop