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

void Network::PrintEachNodeParents() {//print the parents of all the nodes
  for (const auto &id_node_ptr : map_idx_node_ptr) {
    auto node_ptr = id_node_ptr.second;
    cout << node_ptr->node_name << ":\t";
    for (const auto &par_node_ptr : GetParentPtrsOfNode(node_ptr->GetNodeIndex())) {
      cout << par_node_ptr->node_name << '\t';
    }
    cout << endl;
  }
}

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

Node* Network::FindNodePtrByIndex(const int &index) const {
  if (index < 0 || index >= num_nodes) {  // The node indexes are consecutive integers start at 0.
    fprintf(stderr, "Error in function %s! \nInvalid index [%d]!", __FUNCTION__, index);
    exit(1);
  }
  return map_idx_node_ptr.at(index);
}


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


void Network::ConstructNaiveBayesNetwork(Dataset *dts) {
  num_nodes = dts->num_vars;
  // Assign an index for each node.
#pragma omp parallel for
  for (int i = 0; i < num_nodes; ++i) {
    DiscreteNode *node_ptr = new DiscreteNode(i);  // For now, only support discrete node.
    node_ptr->SetDomainSize(dts->num_of_possible_values_of_disc_vars[i]);//set the cardinality of a discrete variable.
    for (auto v : dts->map_disc_vars_possible_values[i]) {//TODO: double check
      node_ptr->vec_potential_vals.push_back(v);
    }
#pragma omp critical
    {
      map_idx_node_ptr[i] = node_ptr;
    }
  }

  // Set parents and children.
  Node *class_node_ptr = FindNodePtrByIndex(dts->class_var_index);
  for (auto &i_n : map_idx_node_ptr) {
    if (i_n.second == class_node_ptr) { continue; }
    SetParentChild(class_node_ptr, i_n.second);
  }

  // Generate configurations of parents.
  GenDiscParCombsForAllNodes();

  // Generate topological ordering and default elimination ordering.
  vector<int> topo = GetTopoOrd();
  vec_default_elim_ord.reserve(num_nodes - 1);
  for (int i = 0; i < num_nodes-1; ++i) {
    vec_default_elim_ord.push_back(topo.at(num_nodes-1-i));
  }
  int vec_size = vec_default_elim_ord.size();
  int vec_capacity = vec_default_elim_ord.capacity();
  if (vec_size != vec_capacity) {
    fprintf(stderr, "Function [%s]: vec_size != vec_capacity\n", __FUNCTION__);
  }
}


void Network::StructLearnCompData(Dataset *dts, bool print_struct, string algo, string topo_ord_constraint, int max_num_parents) {
  fprintf(stderr, "Not be completely implemented yet!");

  cout << "==================================================" << '\n'
       << "Begin structural learning with complete data......" << endl;

  struct timeval start, end;
  double diff;
  gettimeofday(&start,NULL);

  num_nodes = dts->num_vars;
  // Assign an index for each node.
#pragma omp parallel for
  for (int i = 0; i < num_nodes; ++i) {
    DiscreteNode *node_ptr = new DiscreteNode(i);  // For now, only support discrete node.
    if (dts->vec_var_names.size() == num_nodes) {
      node_ptr->node_name = dts->vec_var_names.at(i);
    } else {
      node_ptr->node_name = to_string(i);
    }
    node_ptr->SetDomainSize(dts->num_of_possible_values_of_disc_vars[i]);
    for (auto v : dts->map_disc_vars_possible_values[i]) {
      node_ptr->vec_potential_vals.push_back(v);
    }
#pragma omp critical
    {
      map_idx_node_ptr[i] = node_ptr;
    }
  }

  vector<int> ord;
  ord.reserve(num_nodes);
  for (int i = 0; i < num_nodes; ++i) {
    ord.push_back(i);   // Because the nodes are created the same order as in the dataset.
  }

  cout << "topo_ord_constraint: " << topo_ord_constraint << endl;

  if (topo_ord_constraint == "dataset-ord") {
    // Do nothing.
  } else if (topo_ord_constraint == "random") {
    std::srand(unsigned(std::time(0)));
    std::random_shuffle(ord.begin(), ord.end());
  } else if (topo_ord_constraint == "best") {
    ord = vector<int> {};
  } else {
    fprintf(stderr, "Error in function [%s]!\nInvalid topological ordering restriction!", __FUNCTION__);
    exit(1);
  }

  if (algo == "ott") {
    StructLearnByOtt(dts, ord);
  } else if (algo == "k2-weka") {
    StructLearnLikeK2Weka(dts, ord, max_num_parents);
  }


  cout << "==================================================" << '\n'
       << "Finish structural learning." << endl;

  gettimeofday(&end,NULL);
  diff = (end.tv_sec-start.tv_sec) + ((double)(end.tv_usec-start.tv_usec))/1.0E6;
  setlocale(LC_NUMERIC, "");
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


void Network::AddNode(Node *node_ptr) {
  map_idx_node_ptr[node_ptr->GetNodeIndex()] = node_ptr;
  ++num_nodes;
}


void Network::RemoveNode(int node_index) {
  map_idx_node_ptr.erase(node_index);
  --num_nodes;
}


bool Network::AddArc(int p_index, int c_index) {
    // If NOT form a circle, return true. Otherwise, return false and delete the added arc.
  SetParentChild(p_index, c_index);
  bool contain_circle = ContainCircle();
  if (contain_circle) {
    DeleteArc(p_index, c_index);
  }
  return !contain_circle;
}

void Network::DeleteArc(int p_index, int c_index) {
  RemoveParentChild(p_index, c_index);
}

bool Network::ReverseArc(int p_index, int c_index) {
  // If not form a circle, return true. Otherwise, return false.
  DeleteArc(p_index, c_index);
  return AddArc(c_index, p_index);
}


double Network::CalcuExtraScoreWithModifiedArc(int p_index, int c_index,
                                               Dataset *dts,
                                               string modification,
                                               string score_metric) {
  // todo: test correctness
  Network new_net(*this);

  // Convert the string to lowercase
  transform(modification.begin(), modification.end(), modification.begin(), ::tolower);

  Node *node = new_net.FindNodePtrByIndex(c_index);

  if (modification == "add") {
    if (node->set_parent_indexes.find(p_index)!=node->set_parent_indexes.end()) {
      return 0; // The parent already exists.
    }
    new_net.AddArc(p_index, c_index);
  } else if (modification == "delete") {
    if (node->set_parent_indexes.find(p_index)==node->set_parent_indexes.end()) {
      return 0; // The parent does not exist.
    }
    new_net.DeleteArc(p_index, c_index);
  } else if (modification == "reverse") {
    if (node->set_parent_indexes.find(p_index)==node->set_parent_indexes.end()) {
      return 0; // The parent does not exist.
    }
    new_net.ReverseArc(p_index, c_index);
  } else {
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


void Network::SetParentChild(int p_index, int c_index) {
  Node *p = FindNodePtrByIndex(p_index), *c = FindNodePtrByIndex(c_index);
  SetParentChild(p,c);
}


void Network::SetParentChild(Node *p, Node *c) {
  if (map_idx_node_ptr.find(p->GetNodeIndex()) == map_idx_node_ptr.end()
      ||
      map_idx_node_ptr.find(c->GetNodeIndex())==map_idx_node_ptr.end()) {
    fprintf(stderr, "Error in function [%s].\nThe nodes [%d] and [%d] do not belong to this network!",
            __FUNCTION__, p->GetNodeIndex(), c->GetNodeIndex());
    exit(1);
  }
  p->AddChild(c);
  c->AddParent(p);
}

void Network::RemoveParentChild(int p_index, int c_index) {
  Node *p = FindNodePtrByIndex(p_index), *c = FindNodePtrByIndex(c_index);
  RemoveParentChild(p,c);
}

void Network::RemoveParentChild(Node *p, Node *c) {
  if (map_idx_node_ptr.find(p->GetNodeIndex()) == map_idx_node_ptr.end()
      ||
      map_idx_node_ptr.find(c->GetNodeIndex())==map_idx_node_ptr.end()) {
    fprintf(stderr, "The nodes do not belong to this network!");
    exit(1);
  }
  p->RemoveChild(c);
  c->RemoveParent(p);
}

set<Node*> Network::GetParentPtrsOfNode(int node_index) {
  set<Node*> set_par_ptrs;
  Node *node = map_idx_node_ptr.at(node_index);
  for (const auto &idx : node->set_parent_indexes) {
    set_par_ptrs.insert(map_idx_node_ptr.at(idx));
  }
  return set_par_ptrs;
}

set<Node*> Network::GetChildrenPtrsOfNode(int node_index) {
  set<Node*> set_chi_ptrs;
  Node *node = map_idx_node_ptr.at(node_index);
  for (const auto &idx : node->set_children_indexes) {
    set_chi_ptrs.insert(map_idx_node_ptr.at(idx));
  }
  return set_chi_ptrs;
}

void Network::GenDiscParCombsForAllNodes() {
  for (auto id_np : this->map_idx_node_ptr) {
    auto np = id_np.second;
    np->GenDiscParCombs(GetParentPtrsOfNode(np->GetNodeIndex()));
  }
}

vector<int> Network::GetTopoOrd() {
  if (topo_ord.empty()) {
    this->GenTopoOrd();
  }
  return topo_ord;
}

vector<int> Network::GetReverseTopoOrd() {
  auto ord = this->GetTopoOrd();
  reverse(ord.begin(), ord.end());
  return ord;
}

vector<int> Network::GenTopoOrd() {//TODO: double check

  if (this->pure_discrete) {

    // First, convert the network to a directed adjacency matrix.
    int **graph = new int*[num_nodes];
    #pragma omp for
    for (int i=0; i<num_nodes; ++i) {graph[i] = new int[num_nodes]();}
    for (auto &i_n_p : map_idx_node_ptr) {
      auto n_p = i_n_p.second;
      for (auto &c_p : GetChildrenPtrsOfNode(n_p->GetNodeIndex())) {
        graph[n_p->GetNodeIndex()][c_p->GetNodeIndex()] = 1;
      }
    }

    topo_ord = TopoSortOfDAGZeroInDegreeFirst(graph, num_nodes);

    for (int i=0; i<num_nodes; ++i) { delete[] graph[i]; }
    delete[] graph;

  } else {  

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


int** Network::ConvertDAGNetworkToAdjacencyMatrix() {
  int **adjac_matrix = new int* [num_nodes];
  for (int i=0; i<num_nodes; ++i) {
    adjac_matrix[i] = new int[num_nodes]();
  }
  for (auto &id_node_ptr : map_idx_node_ptr) {
    auto node_ptr = id_node_ptr.second;
    int from, from2, to;
    from = node_ptr->GetNodeIndex();
    for (auto &child_ptr : GetChildrenPtrsOfNode(node_ptr->GetNodeIndex())) {
      to = child_ptr->GetNodeIndex();
      adjac_matrix[from][to] = 1;
    }
  }
  return adjac_matrix;
}


bool Network::ContainCircle() {
  int **graph = ConvertDAGNetworkToAdjacencyMatrix();
  bool result = DirectedGraphContainsCircleByBFS(graph, num_nodes);
  for (int i = 0; i < num_nodes; ++i) {
    delete[] graph[i];
  }
  delete[] graph;
  return result;
}


void Network::LearnParamsKnowStructCompData(const Dataset *dts, int alpha, bool print_params){
  cout << "==================================================" << '\n'
       << "Begin learning parameters with known structure and complete data." << '\n'
       << "Laplace smoothing param: alpha = " << alpha << endl;

  struct timeval start, end;
  double diff;
  gettimeofday(&start,NULL);

  int num_cores = omp_get_num_procs();
  omp_set_num_threads(num_cores);
  int max_work_per_thread = (dts->num_vars + num_cores - 1) / num_cores;
  #pragma omp parallel
  {
    // For every node.
    for (int i = max_work_per_thread*omp_get_thread_num();
         i < max_work_per_thread*(omp_get_thread_num()+1) && i < dts->num_vars;
         ++i) {
//    for (int i=0; i<dts->num_vars; ++i) {
      DiscreteNode *this_node = dynamic_cast<DiscreteNode*>(FindNodePtrByIndex(i));   // todo: support continuous node
      this_node->SetLaplaceSmooth(alpha);

      for (int s = 0; s < dts->num_instance; ++s) {
        vector<int> values = vector<int>(dts->dataset_all_vars[s], dts->dataset_all_vars[s]+dts->num_vars);
        DiscreteConfig instance;
        for (int j = 0; j < values.size(); ++j) {
          instance.insert(pair<int, int>(j, values.at(j)));
        }
        this_node->AddInstanceOfVarVal(instance);
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


int Network::GetNumParams() const {
  int result = 0;
  for (const auto &i_n : map_idx_node_ptr) {
    result += i_n.second->GetNumParams();
  }
  return result;
}


void Network::ClearStructure() {
  for (auto &i_n_p : this->map_idx_node_ptr) {
    i_n_p.second->ClearParams();
    i_n_p.second->ClearParents();
    i_n_p.second->ClearChildren();
  }
}


void Network::ClearParams() {
  for (auto &i_n_p : this->map_idx_node_ptr) {
    i_n_p.second->ClearParams();
  }
}


vector<int> Network::SimplifyDefaultElimOrd(DiscreteConfig evidence) {
  fprintf(stderr, "Function [%s] not implemented yet!", __FUNCTION__);
  exit(1);
}


vector<Factor> Network::ConstructFactors(vector<int> Z, Node *Y) {
  vector<Factor> factors_list;
  factors_list.push_back(Factor(dynamic_cast<DiscreteNode*>(Y), this));
  for (int i = 0; i < Z.size(); ++i) {
    Node* n = FindNodePtrByIndex(Z.at(i));
    factors_list.push_back(Factor(dynamic_cast<DiscreteNode*>(n), this));
  }
  return factors_list;
}


void Network::LoadEvidenceIntoFactors(vector<Factor> *factors_list,
                                      DiscreteConfig E, set<int> all_related_vars) {

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
        // If this factor is related to this node
        if (f.related_variables.find(e.first) != f.related_variables.end()) {
          // Update each row of map_potentials
          for (const auto &comb : f.set_combinations) {
            // If this entry is not compatible to the evidence.
            if (comb.find(e) == comb.end()) {
              f.map_potentials[comb] = 0;
            }
          }
        }
      }

      //--------------------------------------------------------------------------------
      // This block is to fix the bug occurring when the target node
      // is not the root and the variable elimination order do not start at root.
      // For example:  A --> B --> C
      // When given the markov blanket of node "C", which is "{B}",
      // there is no need to calculate the other nodes, which is "{A}".
      // However, when using this function,
      // the parent of parent of this node, which is "A",
      // still appears in the constructed factor of the parent which is "B".
      // todo: check correctness
      set<int> related_vars_of_f = f.related_variables;
      for (auto &v : related_vars_of_f) {
        if (all_related_vars.find(v) == all_related_vars.end()) {
          f = f.SumOverVar(v);
        }
      }
      //--------------------------------------------------------------------------------
    }

  }   // end of: #pragma omp parallel
}


Factor Network::SumProductVarElim(vector<Factor> factors_list, vector<int> elim_order) {
  for (int i = 0; i < elim_order.size(); ++i) {
    vector<Factor> tempFactorsList;
    Node* nodePtr = FindNodePtrByIndex(elim_order.at(i));
    // Move every factor that is related to the node elim_order[i] from factors_list to tempFactorsList.
    /*
     * Note: This for loop does not contain "++it" in the parentheses.
     *      Because if we do so, it may cause some logic faults which,
     *      however, may or may not cause runtime error, causing the program hard to debug.
     *      For example:
     *        When "it" reaches the second to last element, and this element is related to the node.
     *        Then this element will be erase from factors_list,
     *        and then "++it" which will move "it" to the end.
     *        Then the for loop will end because "it" has reached the end.
     *        However, at this time, the last element has been ignored,
     *        even if it is related to the node.
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
    Factor newFactor = tempFactorsList.back().SumOverVar(dynamic_cast<DiscreteNode*>(nodePtr));
    factors_list.push_back(newFactor);
  }

  /*
   *   If we are calculating a node's posterior probability given evidence about its children,
   *   then when the program runs to here,
   *   the "factors_list" will contain several factors
   *   about the same node which is the query node Y.
   *   When it happens, we need to multiply these several factors.
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
  // After all the processing shown above, the only remaining factor is the factor about Y.
  return factors_list.back();
}


Factor Network::VarElimInferReturnPossib(DiscreteConfig evid, Node *target_node, vector<int> elim_order) {
  // elim_order is the array of variable elimination order.
  // evid is the evidences.

  if (elim_order.empty()) {
    elim_order = SimplifyDefaultElimOrd(evid);
  }

  vector<Factor> factorsList = ConstructFactors(elim_order, target_node);

  //--------------------------------------------------------------------------------
  // This block is to fix the bug occurring when the target node
  // is not the root and the variable elimination order do not start at root.
  // For example:  A --> B --> C
  // When given the markov blanket, which is "{B}", of node "C",
  // there is no need to calculate the other nodes, which is "{A}".
  // However, when using this function,
  // the parent of parent of this node, which is "A",
  // still appears in the constructed factor of the parent which is "B".
  set<int> all_related_vars;
  all_related_vars.insert(target_node->GetNodeIndex());
  for (int i = 0; i < elim_order.size(); ++i) { all_related_vars.insert(elim_order.at(i)); }
  //--------------------------------------------------------------------------------


  LoadEvidenceIntoFactors(&factorsList, evid, all_related_vars);
  Factor F = SumProductVarElim(factorsList, elim_order);
  F.Normalize();
  return F;
}


map<int, double> Network::DistributionOfValueIndexGivenCompleteInstanceValueIndex(int target_var_index, DiscreteConfig evidence) {
  if (!this->pure_discrete) {
    fprintf(stderr, "Function [%s] only works on pure discrete networks!", __FUNCTION__);
    exit(1);
  }

  map<int, double> result;

  DiscreteNode *target_node = (DiscreteNode*) FindNodePtrByIndex(target_var_index);
  auto vec_complete_instance_values = SparseInstanceFillZeroToDenseInstance(evidence);

  for (int i = 0; i < target_node->GetDomainSize(); ++i) {
    vec_complete_instance_values.at(target_var_index) = target_node->vec_potential_vals.at(i);
    evidence.insert(DiscVarVal(target_var_index, target_node->vec_potential_vals.at(i)));
    result[i] = 0;
    for (int j = 0; j < num_nodes; ++j) {
      DiscreteNode *node_j = (DiscreteNode*) FindNodePtrByIndex(j);
      DiscreteConfig par_config = node_j->GetDiscParConfigGivenAllVarValue(vec_complete_instance_values);

      double temp_prob = 0;
      if (j == target_var_index) {
        temp_prob = target_node->GetProbability(target_node->vec_potential_vals.at(i), par_config);
      } else {
        temp_prob = node_j->GetProbability(vec_complete_instance_values.at(j), par_config);
      }
      result[i] += log(temp_prob);
    }
    evidence.erase(DiscVarVal(target_var_index, target_node->vec_potential_vals.at(i)));
  }
  for (int i = 0; i < target_node->GetDomainSize(); ++i) {
    result[i] = exp(result[i]);
  }
  result = Normalize(result);
  return result;
}




int Network::PredictUseVarElimInfer(DiscreteConfig evid, int target_node_idx, vector<int> elim_order) {
  Node *Y = FindNodePtrByIndex(target_node_idx);
  Factor F = VarElimInferReturnPossib(evid, Y, elim_order);
  double max_prob = 0;
  DiscreteConfig comb_predict;
  for (auto &comb : F.set_combinations) {
    if (F.map_potentials[comb] > max_prob) {
      max_prob = F.map_potentials[comb];
      comb_predict = comb;
    }
  }
  int label_predict = comb_predict.begin()->second;
  return label_predict;
}

vector<int> Network::PredictUseVarElimInfer(vector<DiscreteConfig> evidences, int target_node_idx, vector<vector<int>> elim_orders) {
  int size = evidences.size();

  cout << "Progress indicator: ";
  int every_1_of_20 = size / 20,
          progress = 0;

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


int Network::PredictUseSimpleBruteForce(DiscreteConfig E, int Y_index) {
  map<int, double> distribution = DistributionOfValueIndexGivenCompleteInstanceValueIndex(Y_index, E);
  int label_index = ArgMax(distribution);
  int label_pridict = ((DiscreteNode*)FindNodePtrByIndex(Y_index))->vec_potential_vals.at(label_index);
  return label_pridict;
}


vector<int> Network::PredictUseSimpleBruteForce(vector<DiscreteConfig> evidences, int target_node_idx) {
  int size = evidences.size();

  cout << "Progress indicator: ";
  int every_1_of_20 = size / 20,
      progress = 0;


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

    int pred = PredictUseSimpleBruteForce(evidences.at(i), target_node_idx);
    results.at(i) = pred;
  }
  return results;
}


double Network::EvaluateVarElimAccuracy(Dataset *dts) {

  cout << "==================================================" << '\n'
       << "Begin testing the trained network." << endl;

  struct timeval start, end;
  double diff;
  gettimeofday(&start,NULL);

  int m = dts->num_instance;

  int class_var_index = dts->class_var_index;


  vector<int> ground_turths;
  vector<DiscreteConfig> evidences;
  evidences.reserve(m);
  ground_turths.reserve(m);
  for (int i = 0; i < m; ++i) {
    int e_num = num_nodes - 1, *e_index = new int[e_num], *e_value = new int[e_num];
    for (int j = 0; j < num_nodes; ++j) {
      if (j == class_var_index) {continue;}
      e_index[j < class_var_index ? j : j - 1] = j;
      e_value[j < class_var_index ? j : j - 1] = dts->dataset_all_vars[i][j];
    }
    DiscreteConfig E = ArrayToDiscreteConfig(e_index, e_value, e_num);
    evidences.push_back(E);
    int g = dts->dataset_all_vars[i][class_var_index];
    ground_turths.push_back(g);
    delete[] e_index;
    delete[] e_value;
  }


  vector<int> predictions = PredictUseVarElimInfer(evidences, class_var_index);

  double accuracy = Accuracy(ground_turths, predictions);

  cout << '\n' << "Accuracy: " << accuracy << endl;


  gettimeofday(&end,NULL);
  diff = (end.tv_sec-start.tv_sec) + ((double)(end.tv_usec-start.tv_usec))/1.0E6;
  setlocale(LC_NUMERIC, "");
  cout << "==================================================" << '\n'
       << "The time spent to test the accuracy is " << diff << " seconds" << endl;

  return accuracy;
}


double Network::EvaluateAccuracyGivenAllCompleteInstances(Dataset *dts) {

  cout << "==================================================" << '\n'
       << "Begin testing the trained network." << endl;

  struct timeval start, end;
  double diff;
  gettimeofday(&start,NULL);

  int m = dts->num_instance;

  int class_var_index = dts->class_var_index;

  vector<int> ground_turths;
  vector<DiscreteConfig> evidences;
  evidences.reserve(m);
  ground_turths.reserve(m);
  for (int i = 0; i < m; ++i) {
    int e_num = num_nodes - 1, *e_index = new int[e_num], *e_value = new int[e_num];
    for (int j = 0; j < num_nodes; ++j) {
      if (j == class_var_index) {continue;}
      e_index[j < class_var_index ? j : j - 1] = j;
      e_value[j < class_var_index ? j : j - 1] = dts->dataset_all_vars[i][j];
    }
    DiscreteConfig E = ArrayToDiscreteConfig(e_index, e_value, e_num);
    evidences.push_back(E);
    int g = dts->dataset_all_vars[i][class_var_index];
    ground_turths.push_back(g);
    delete[] e_index;
    delete[] e_value;
  }

  vector<int> predictions = PredictUseSimpleBruteForce(evidences, class_var_index);

  double accuracy = Accuracy(ground_turths, predictions);

  cout << '\n' << "Accuracy: " << accuracy << endl;

  gettimeofday(&end,NULL);
  diff = (end.tv_sec-start.tv_sec) + ((double)(end.tv_usec-start.tv_usec))/1.0E6;
  setlocale(LC_NUMERIC, "");
  cout << "==================================================" << '\n'
       << "The time spent to test the accuracy is " << diff << " seconds" << endl;

  return accuracy;
}


double Network::EvaluateApproxInferAccuracy(Dataset *dts, int num_samp) {


  cout << "==================================================" << '\n'
       << "Begin testing the trained network." << endl;

  struct timeval start, end;
  double diff;
  gettimeofday(&start,NULL);

  int m = dts->num_instance;

  int class_var_index = dts->class_var_index;

  vector<DiscreteConfig> samples = this->DrawSamplesByProbLogiSamp(num_samp);
  cout << "Finish drawing samples." << endl;

  vector<int> ground_turths;
  vector<DiscreteConfig> evidences;
  evidences.reserve(m);
  ground_turths.reserve(m);
  for (int i = 0; i < m; ++i) {
    int e_num = num_nodes - 1, *e_index = new int[e_num], *e_value = new int[e_num];
    for (int j = 0; j < num_nodes; ++j) {
      if (j == class_var_index) {continue;}
      e_index[j < class_var_index ? j : j - 1] = j;
      e_value[j < class_var_index ? j : j - 1] = dts->dataset_all_vars[i][j];
    }
    DiscreteConfig E = ArrayToDiscreteConfig(e_index, e_value, e_num);
    evidences.push_back(E);
    int g = dts->dataset_all_vars[i][class_var_index];
    ground_turths.push_back(g);
    delete[] e_index;
    delete[] e_value;
  }

  vector<int> predictions = ApproxInferByProbLogiRejectSamp(evidences, class_var_index, samples);

  double accuracy = Accuracy(ground_turths, predictions);

  cout << '\n' << "Accuracy: " << accuracy << endl;

  gettimeofday(&end,NULL);
  diff = (end.tv_sec-start.tv_sec) + ((double)(end.tv_usec-start.tv_usec))/1.0E6;
  setlocale(LC_NUMERIC, "");
  cout << "==================================================" << '\n'
       << "The time spent to test the accuracy is " << diff << " seconds" << endl;

  return accuracy;
}

double Network::EvaluateLikelihoodWeightingAccuracy(Dataset *dts, int num_samp) {
  cout << "==================================================" << '\n'
       << "Begin testing the trained network." << endl;

  struct timeval start, end;
  double diff;
  gettimeofday(&start,NULL);

  int m = dts->num_instance;

  int class_var_index = dts->class_var_index;

  vector<int> ground_turths;
  vector<DiscreteConfig> evidences;
  evidences.reserve(m);
  ground_turths.reserve(m);
  for (int i = 0; i < m; ++i) {
    int e_num = num_nodes - 1, *e_index = new int[e_num], *e_value = new int[e_num];
    for (int j = 0; j < num_nodes; ++j) {
      if (j == class_var_index) {continue;}
      e_index[j < class_var_index ? j : j - 1] = j;
      e_value[j < class_var_index ? j : j - 1] = dts->dataset_all_vars[i][j];
    }
    DiscreteConfig E = ArrayToDiscreteConfig(e_index, e_value, e_num);
    evidences.push_back(E);
    int g = dts->dataset_all_vars[i][class_var_index];
    ground_turths.push_back(g);
    delete[] e_index;
    delete[] e_value;
  }

  vector<int> predictions = ApproxinferByLikelihoodWeighting(evidences, class_var_index, num_samp);

  double accuracy = Accuracy(ground_turths, predictions);

  cout << '\n' << "Accuracy: " << accuracy << endl;


  gettimeofday(&end,NULL);
  diff = (end.tv_sec-start.tv_sec) + ((double)(end.tv_usec-start.tv_usec))/1.0E6;
  setlocale(LC_NUMERIC, "");
  cout << "==================================================" << '\n'
       << "The time spent to test the accuracy is " << diff << " seconds" << endl;

  return accuracy;
}



DiscreteConfig Network::ProbLogicSampleNetwork() {
  // Probabilistic logic sampling is a method
  // proposed by Max Henrion at 1988.

  DiscreteConfig instance;
  // Cannot use OpenMP, because must draw samples in the topological ordering.
  for (const auto &index : this->GetTopoOrd()) {
    Node *n_p = FindNodePtrByIndex(index);
    int drawn_value = dynamic_cast<DiscreteNode*>(n_p)->SampleNodeGivenParents(instance); // todo: support continuous nodes
    instance.insert(pair<int,int>(index, drawn_value));
  }
  return instance;
}

pair<DiscreteConfig, double> Network::DrawOneLikelihoodWeightingSample(const DiscreteConfig &evidence) {
  DiscreteConfig instance;
  double weight = 1;
  // SHOULD NOT use OpenMP, because must draw samples in the topological ordering.
  for (const auto &index : this->GetTopoOrd()) {  // For each node.
    Node *n_p = FindNodePtrByIndex(index);
    bool observed = false;
    for (const auto &var_val : evidence) {  // Find if this variable node is in evidence.
      if (index==var_val.first) {
        observed = true;
        // Set the sample value to be the same as the evidence.
        instance.insert(pair<int, int>(index, var_val.second));

        // todo: check the correctness of this implementation for the 4 lines below
        // Update the weight.
        DiscreteConfig parents_config = dynamic_cast<DiscreteNode*>(n_p)->GetDiscParConfigGivenAllVarValue(instance);
        double cond_prob = dynamic_cast<DiscreteNode*>(n_p)->GetProbability(var_val.second, parents_config);
        weight *= cond_prob;

        break;
      }
    }
    if (!observed) {
      int drawn_value = dynamic_cast<DiscreteNode*>(n_p)->SampleNodeGivenParents(instance);   // todo: Consider continuous nodes
      instance.insert(pair<int,int>(index, drawn_value));
    }
  }
  return pair<DiscreteConfig, double>(instance, weight);
}


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

Factor Network::CalcuMargWithLikelihoodWeightingSamples(const vector<pair<DiscreteConfig, double>> &samples,
                                                        const int &node_index) {
  map<int, double> value_weight;
  DiscreteNode *n_p = dynamic_cast<DiscreteNode*>(this->FindNodePtrByIndex(node_index));

  // Initialize the map.
  for (int i=0; i<n_p->GetDomainSize(); ++i) {
    value_weight[n_p->vec_potential_vals.at(i)] = 0;
  }

  // Calculate the sum of weight for each value. Un-normalized.
  for (const auto &samp : samples) {
    for (const auto &feature_value : samp.first) {
      if (node_index==feature_value.first) {
        value_weight[feature_value.second] += samp.second;
        break;
      }
    }
  }

  // Normalization.
  double denominator = 0;
  for (const auto &kv : value_weight) {
    denominator += kv.second;
  }
  for (auto &kv : value_weight) {
    kv.second /= denominator;
  }

  // Construct a factor to return
  set<int> rv;
  rv.insert(node_index);
  set<DiscreteConfig> sc;
  for (int i=0; i<n_p->GetDomainSize(); ++i) {
    DiscreteConfig c;
    c.insert(pair<int, int>(node_index, n_p->vec_potential_vals.at(i)));
    sc.insert(c);
  }
  map<DiscreteConfig, double> mp;
  for (const auto &c : sc) {
    int value = (*c.begin()).second;
    mp[c] = value_weight[value];
  }
  Factor f(rv, sc, mp);
  return f;
}


int Network::ApproxinferByLikelihoodWeighting(DiscreteConfig e, const int &node_index, const int &num_samp) {
  vector<pair<DiscreteConfig, double>> samples_weight = this->DrawSamplesByLikelihoodWeighting(e, num_samp);
  Factor f = CalcuMargWithLikelihoodWeightingSamples(samples_weight, node_index);
  // Find the argmax.
  DiscreteConfig c;
  double max = -1;
  for (const auto &kv : f.map_potentials) {
    if (kv.second > max) {
      c = kv.first;
      max = kv.second;
    }
  }
  return (*c.begin()).second;
}

vector<int> Network::ApproxinferByLikelihoodWeighting(vector<DiscreteConfig> evidences,
                                                      const int &target_node_idx, const int &num_samp) {
  int size = evidences.size();

  cout << "Progress indicator: ";
  int every_1_of_20 = size / 20,
          progress = 0;


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

vector<DiscreteConfig> Network::DrawSamplesByProbLogiSamp(int num_samp) {
  vector<DiscreteConfig> samples;
  samples.reserve(num_samp);
  #pragma omp parallel for
  for (int i=0; i<num_samp; ++i) {
    DiscreteConfig samp = this->ProbLogicSampleNetwork();
    #pragma omp critical
    { samples.push_back(samp); }
  }
  return samples;
}


set<int> Network::GetMarkovBlanketIndexesOfNode(Node *node_ptr) {
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


vector<DiscreteConfig> Network::DrawSamplesByGibbsSamp(int num_samp, int num_burn_in) {

  vector<DiscreteConfig> samples;
  samples.reserve(num_samp);

  DiscreteConfig single_sample = this->ProbLogicSampleNetwork();

  auto it_idx_node = this->map_idx_node_ptr.begin();


  // Need burning in.
//  #pragma omp parallel for
  for (int i=1; i<num_burn_in+num_samp; ++i) {

    Node *node_ptr = (*(it_idx_node++)).second;
    if (it_idx_node == map_idx_node_ptr.end()) {
      it_idx_node = this->map_idx_node_ptr.begin();
    }

    set<int> markov_blanket_node_index = GetMarkovBlanketIndexesOfNode(node_ptr);

    DiscreteConfig markov_blanket;
    for (auto &p : single_sample) {
      if (markov_blanket_node_index.find(p.first)
          !=
          markov_blanket_node_index.end()) {
        markov_blanket.insert(p);
      }
    }

    int value_index =
            SampleNodeGivenMarkovBlanketReturnValIndex(node_ptr,markov_blanket);

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


int Network::SampleNodeGivenMarkovBlanketReturnValIndex(Node *node_ptr, DiscreteConfig markov_blanket) {
  int num_elim_ord = markov_blanket.size();
  vector<int> var_elim_ord;
  var_elim_ord.reserve(markov_blanket.size());
  for (auto &n_v : markov_blanket) {
    var_elim_ord.push_back(n_v.first);
  }

  Factor f = VarElimInferReturnPossib(markov_blanket, node_ptr, var_elim_ord);

  vector<int> weights;
  for (int i=0; i<dynamic_cast<DiscreteNode*>(node_ptr)->GetDomainSize(); ++i) {
    DiscreteConfig temp;
    temp.insert(pair<int,int>(node_ptr->GetNodeIndex(),
                              dynamic_cast<DiscreteNode*>(node_ptr)->vec_potential_vals.at(i)));
    weights.push_back(f.map_potentials[temp]*10000);
  }

  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  default_random_engine rand_gen(seed);
  discrete_distribution<int> this_distribution(weights.begin(),weights.end());
  return this_distribution(rand_gen);
}


int Network::ApproxInferByProbLogiRejectSamp(DiscreteConfig e, Node *node, vector<DiscreteConfig> &samples) {
  DiscreteConfig possb_values;
  for (int i=0; i<dynamic_cast<DiscreteNode*>(node)->GetDomainSize(); ++i) {
    possb_values.insert(pair<int,int>(node->GetNodeIndex(),
                                      dynamic_cast<DiscreteNode*>(node)->vec_potential_vals.at(i)));
  }

  int *count_each_value = new int[this->num_nodes]();
  int num_valid_sample = 0;
  for (const auto &samp : samples) {
    if(!Conflict(&e, &samp)) {
      ++num_valid_sample;
      for (const auto &pv : possb_values) {
        if (samp.find(pv)!=samp.end()) {
          ++count_each_value[pv.second];
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

  // Find the argmax.
  int lable_index_predict = -1;
  int max_occurred = 0;
  for (int i=0; i<dynamic_cast<DiscreteNode*>(node)->GetDomainSize(); ++i) {
    if (lable_index_predict==-1 || count_each_value[i]>max_occurred) {
      lable_index_predict = i;
      max_occurred = count_each_value[i];
    }
  }

  // Return the predicted label instead of the index.
  return dynamic_cast<DiscreteNode*>(node)->vec_potential_vals.at(lable_index_predict);
}


int Network::ApproxInferByProbLogiRejectSamp(DiscreteConfig e, int node_index, vector<DiscreteConfig> &samples) {
  return ApproxInferByProbLogiRejectSamp(e, FindNodePtrByIndex(node_index), samples);
}

vector<int> Network::ApproxInferByProbLogiRejectSamp(vector<DiscreteConfig> evidences, int node_idx, vector<DiscreteConfig> &samples) {
  int size = evidences.size();

  cout << "Progress indicator: ";
  int every_1_of_20 = size / 20,
          progress = 0;


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



pair<double, set<Node*>> Network::F(Node *node,
        set<Node*> &candidate_parents,
        Dataset *dts,
        map<Node*, map<set<Node*>, double>> &dynamic_program_for_F) {

  // The original paper chooses minimum score but this program chooses maximum score.

  map<set<Node*>, double> &this_node_dynamic_program = dynamic_program_for_F[node];

  // Look up the table.
  if (this_node_dynamic_program.find(candidate_parents) != this_node_dynamic_program.end()) {
    return pair<double, set<Node*>>(this_node_dynamic_program[candidate_parents], candidate_parents);
  }

  pair<double, set<Node*>> max_score_parents;
  max_score_parents.first = -DBL_MAX;

  // Look up the dynamic programming table.
  for (auto n : candidate_parents) {
    set<Node*> candidate_parents_temp = candidate_parents;
    candidate_parents_temp.erase(n);

    if (this_node_dynamic_program.find(candidate_parents_temp) == this_node_dynamic_program.end()) {
      this_node_dynamic_program[candidate_parents_temp] =
              F(node, candidate_parents_temp, dts, dynamic_program_for_F).first;
    }

    if (this_node_dynamic_program[candidate_parents_temp] > max_score_parents.first) {
      max_score_parents.first = this_node_dynamic_program[candidate_parents_temp];
      max_score_parents.second = candidate_parents_temp;
    }
  }

  if (this_node_dynamic_program.find(candidate_parents)==this_node_dynamic_program.end()) {

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

    ScoreFunction sf(&temp_net, dts);
    this_node_dynamic_program[candidate_parents] = sf.ScoreForNode(&node_copy,"log BDeu");

  }

  if (this_node_dynamic_program[candidate_parents] > max_score_parents.first) {
    max_score_parents.first = this_node_dynamic_program[candidate_parents];
    max_score_parents.second = candidate_parents;
  }

  return max_score_parents;
}

pair<double, vector<pair<Node*, set<Node*>>>>
 Network::Q(set<Node*> &set_nodes,
         vector<int> topo_ord,
         Dataset *dts,
         map<Node*,   map<set<Node*>, double>> &dynamic_program_for_F,
         map<pair<set<Node*>, vector<int>>,   pair<double, vector<pair<Node*, set<Node*>>>>> dynamic_program_for_Q) {

  // Look up the table.
  pair<set<Node*>, vector<int>> to_find(set_nodes, topo_ord);
  if (dynamic_program_for_Q.find(to_find)!=dynamic_program_for_Q.end()) {
    return dynamic_program_for_Q[to_find];
  }

  double score = 0;
  vector<pair<Node*, set<Node*>>> vec_node_parents;

  for (auto &n : set_nodes) {
    set<Node*> candidate_parents;
    for (const auto &i : topo_ord) {
      if (n->GetNodeIndex() == i) { break; }
      candidate_parents.insert(this->FindNodePtrByIndex(i));
    }
    pair<double, set<Node*>> best_score_parents = F(n, candidate_parents, dts, dynamic_program_for_F);

    score += best_score_parents.first;
    vec_node_parents.push_back(pair<Node*, set<Node*>>(n, best_score_parents.second));
  }

  return pair<double, vector<pair<Node*, set<Node*>>>>(score, vec_node_parents);
}

vector<int> Network::M(set<Node*> &set_nodes,
        Dataset *dts,
        map<Node*, map<set<Node*>, double>> &dynamic_program_for_F,
        map<pair<set<Node*>, vector<int>>,   pair<double, vector<pair<Node*, set<Node*>>>>> dynamic_program_for_Q,
        map<set<Node*>, vector<int>> dynamic_program_for_M) {

  if (set_nodes.empty()) { return vector<int> {}; }

  // Look up the table.
  if (dynamic_program_for_M.find(set_nodes) != dynamic_program_for_M.end()) {
    return dynamic_program_for_M[set_nodes];
  }

  Node *g_star = nullptr;
  double score = -DBL_MAX;
  for (auto n : set_nodes) {
    set<Node*> set_nodes_temp = set_nodes;
    set_nodes_temp.erase(n);
    vector<int> m_of_set_nodes_temp = M(set_nodes_temp, dts,
                                        dynamic_program_for_F, dynamic_program_for_Q,
                                        dynamic_program_for_M);
    double score_temp = F(n, set_nodes_temp, dts, dynamic_program_for_F).first
                        + Q(set_nodes_temp, m_of_set_nodes_temp, dts, dynamic_program_for_F, dynamic_program_for_Q).first;
    if (score_temp > score) {
      score = score_temp;
      g_star = n;
    }
  }

  set<Node*> set_nodes_remove_g_star = set_nodes;
  set_nodes_remove_g_star.erase(g_star);
  vector<int> result = M(set_nodes_remove_g_star, dts, dynamic_program_for_F, dynamic_program_for_Q, dynamic_program_for_M);
  result.push_back(g_star->GetNodeIndex());

  return result;

}


void Network::StructLearnByOtt(Dataset *dts, vector<int> topo_ord_constraint) {

  map<Node*, map<set<Node*>, double>> dynamic_program_for_F;
  map<pair<set<Node*>, vector<int>>,   pair<double, vector<pair<Node*, set<Node*>>>>> dynamic_program_for_Q;

  set<Node*> set_node_ptr_container;
  for (auto id_np : map_idx_node_ptr) {
    set_node_ptr_container.insert(id_np.second);
  }

  if (topo_ord_constraint.empty() || topo_ord_constraint.size() != num_nodes) {
    map<set<Node*>, vector<int>> dynamic_program_for_M;
    vector<int> m_of_all_nodes = M(set_node_ptr_container, dts, dynamic_program_for_F, dynamic_program_for_Q,
                                   dynamic_program_for_M);
    topo_ord_constraint = m_of_all_nodes;
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


vector<int> Network::SparseInstanceFillZeroToDenseInstance(DiscreteConfig &sparse_instance) {
  vector<int> complete_instance(this->num_nodes, 0);
  for (const auto p : sparse_instance) {
    complete_instance.at(p.first) = p.second;
  }
  return complete_instance;
}


void Network::StructLearnLikeK2Weka(Dataset *dts, vector<int> topo_ord_constraint, int max_num_parents) {
  // todo: test the correctness
  if (topo_ord_constraint.empty() || topo_ord_constraint.size() != num_nodes) {
    topo_ord_constraint.reserve(num_nodes);
    for (int i = 0; i < num_nodes; ++i) {
      topo_ord_constraint.push_back(i);
    }
  }
  GenDiscParCombsForAllNodes();
//#pragma omp parallel for
  for (int i = 0; i < num_nodes; ++i) {

    int var_index = topo_ord_constraint.at(i);
    DiscreteNode *node = (DiscreteNode*) this->map_idx_node_ptr.at(var_index);

    bool progress = (node->GetNumParents() < max_num_parents);
    while (progress) {
      int best_par_index = -1;
      double best_extra_score = 0;
      for (int j = 0; j < i; ++j) {
        int par_index = topo_ord_constraint.at(j);
        double extra_score = CalcuExtraScoreWithModifiedArc(par_index, var_index, dts, "add", "log K2");
        if (extra_score > best_extra_score) {
          if (this->AddArc(par_index, var_index)) {
            best_par_index = j;
            best_extra_score = extra_score;
            this->DeleteArc(par_index, var_index);
          }
        }
      }
      if (best_par_index == -1) {
        progress = false;
      } else {
        this->AddArc(best_par_index, var_index);
        progress = (node->GetNumParents() < max_num_parents);
      }
    }

  }
}

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