#pragma clang diagnostic push
#pragma ide diagnostic ignored "openmp-use-default-none"
//
// Created by Linjian Li on 2018/11/29.
//

#include "Network.h"
#include "ScoreFunction.h"

Network::Network(): Network(true) {}

Network::Network(bool pure_disc) {
  this->pure_discrete = pure_disc;
}


void Network::PrintEachNodeParents() {
  for (const auto &node_ptr : set_node_ptr_container) {
    cout << node_ptr->GetNodeIndex() << ":\t";
    for (const auto &par_node_ptr : node_ptr->set_parents_ptrs) {
      cout << par_node_ptr->GetNodeIndex() << '\t';
    }
    cout << endl;
  }
}

void Network::PrintEachNodeChildren() {
  for (const auto &node_ptr : set_node_ptr_container) {
    cout << node_ptr->GetNodeIndex() << ":\t";
    for (const auto &par_node_ptr : node_ptr->set_children_ptrs) {
      cout << par_node_ptr->GetNodeIndex() << '\t';
    }
    cout << endl;
  }
}

Node* Network::FindNodePtrByIndex(const int &index) const {
  if (index<0 || index>num_nodes) {
    fprintf(stderr, "Error in function %s! \nInvalid index!", __FUNCTION__);
    exit(1);
  }
  Node* node_ptr = nullptr;
  for (const auto n_ptr : set_node_ptr_container) {
    if (n_ptr->GetNodeIndex()==index) {
      node_ptr = n_ptr;
      break;
    }
  }
  return node_ptr;
}


Node* Network::FindNodePtrByName(const string &name) const {
  Node* node_ptr = nullptr;
  for (const auto n_ptr : set_node_ptr_container) {
    if (n_ptr->node_name==name) {
      node_ptr = n_ptr;
      break;
    }
  }
  return node_ptr;
}


void Network::StructLearnCompData(Dataset *dts, bool print_struct) {
  fprintf(stderr, "Not be implemented yet!");
//  exit(1);



  cout << "==================================================" << '\n'
       << "Begin structural learning with complete data......" << endl;

  num_nodes = dts->num_vars;
  // Assign an index for each node.
  #pragma omp parallel for
  for (int i=0; i<num_nodes; ++i) {
    Node *node_ptr = new DiscreteNode(i);  // For now, only support discrete node.
    node_ptr->num_potential_vals = dts->num_of_possible_values_of_disc_vars[i];
    node_ptr->potential_vals = new int[node_ptr->num_potential_vals];
    int j = 0;
    for (auto v : dts->map_disc_vars_possible_values[i]) {
      node_ptr->potential_vals[j++] = v;
    }
    #pragma omp critical
    { set_node_ptr_container.insert(node_ptr); }
  }


  // todo: implement structure learning




//  cout << "==================================================" << '\n'
//       << "Setting children and parents......" << endl;
//  #pragma omp parallel for
//  for (int i=0; i<num_nodes; ++i) {
//    for (int j=0; j<i; ++j) {  // graphAdjacencyMatrix is symmetric, so loop while j<i instead of j<n
//      if (i==j) continue;
//      if (graphAdjacencyMatrix[i][j]==1){
//
//        // Determine the topological position of i and j.
//        int topoIndexI=-1, topoIndexJ=-1;
//        for (int k=0; k<num_nodes; ++k) {
//          if (topologicalSortedPermutation[k]==i && topoIndexI==-1) {
//            topoIndexI = k;
//          } else if (topologicalSortedPermutation[k]==j && topoIndexJ==-1) {
//            topoIndexJ = k;
//          }
//          if (topoIndexI!=-1 && topoIndexJ!=-1) { break; }
//        }
//
//        if (topoIndexI<topoIndexJ) {
//          SetParentChild(i, j);
//        } else {
//          SetParentChild(j, i);
//        }
//      }
//    }
//  }

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
    auto topo = GetTopoOrd();
    for (int m = 0; m < num_nodes; ++m) {
      cout << topo.at(m) << '\t';
    }
    cout << endl;

    cout << "==================================================" << '\n'
         << "Each node's parents: " << endl;
    this->PrintEachNodeParents();

  }

//  for (int i=0; i<num_nodes; ++i) {
//    delete[] graphAdjacencyMatrix[i];
//  }
//  delete[] graphAdjacencyMatrix;
}


void Network::AddNode(Node *node_ptr) {
  set_node_ptr_container.insert(node_ptr);
  num_nodes = set_node_ptr_container.size();
}


void Network::SetParentChild(int p_index, int c_index) {
  Node *p = FindNodePtrByIndex(p_index), *c = FindNodePtrByIndex(c_index);
  SetParentChild(p,c);
}


void Network::SetParentChild(Node *p, Node *c) {
  if (set_node_ptr_container.find(p)==set_node_ptr_container.end()
      ||
      set_node_ptr_container.find(c)==set_node_ptr_container.end()) {
    fprintf(stderr, "The nodes do not belong to this network!");
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
  if (set_node_ptr_container.find(p)==set_node_ptr_container.end()
      ||
      set_node_ptr_container.find(c)==set_node_ptr_container.end()) {
    fprintf(stderr, "The nodes do not belong to this network!");
    exit(1);
  }
  p->RemoveChild(c);
  c->RemoveParent(p);
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

vector<int> Network::GenTopoOrd() {

  if (this->pure_discrete) {

    // First, convert the network to a directed adjacency matrix.
    int **graph = new int*[num_nodes];
    #pragma omp for
    for (int i=0; i<num_nodes; ++i) {graph[i] = new int[num_nodes]();}
    for (auto &n_p : set_node_ptr_container) {
      for (auto &c_p : n_p->set_children_ptrs) {
        graph[n_p->GetNodeIndex()][c_p->GetNodeIndex()] = 1;
      }
    }

    topo_ord = TopoSortOfDAGZeroInDegreeFirst(graph, num_nodes);

    for (int i=0; i<num_nodes; ++i) { delete[] graph[i]; }
    delete[] graph;

  } else {  // If the network is not pure discrete, then it is conditional Gaussian.
            // Discrete nodes should not have continuous parents.
            // Continuous nodes can have both discrete and continuous parents.
            // In topological ordering, all discrete nodes should
            // occur before any continuous node.
            // todo: test correctness of the case of Gaussian network

    set<Node*> set_disc_node_ptr, set_cont_node_ptr;
    for (const auto &n_p : set_node_ptr_container) {
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
      for (const auto &c_p : n_p->set_children_ptrs) {
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
      for (const auto &c_p : n_p->set_children_ptrs) {
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
  for (auto &node_ptr : set_node_ptr_container) {
    int from, from2, to;
    from = node_ptr->GetNodeIndex();
    for (auto &child_ptr : node_ptr->set_children_ptrs) {
      to = child_ptr->GetNodeIndex();
      adjac_matrix[from][to] = 1;
    }
  }
  return adjac_matrix;
}


void Network::LearnParamsKnowStructCompData(const Dataset *dts, bool print_params){
  cout << "==================================================" << '\n'
       << "Begin learning parameters with known structure and complete data." << endl;

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
      Node *this_node = FindNodePtrByIndex(i);
      if (this_node->set_parents_ptrs.empty()) {

        map<int, double> *MPT = &(dynamic_cast<DiscreteNode*>(this_node)->map_marg_prob_table);
        int denominator = 0;
        for (int s = 0; s < dts->num_instance; ++s) {
          denominator += 1;
          int query = dts->dataset_all_vars[s][i];
          (*MPT)[query] += 1;
        }
        for (int ii = 0; ii < this_node->num_potential_vals; ++ii) {
          int query = this_node->potential_vals[ii];
          (*MPT)[query] /= denominator;
        }

      } else {  // If the node has parents.

        map<int, map<DiscreteConfig, double> > *CPT = &(dynamic_cast<DiscreteNode*>(this_node)->map_cond_prob_table);
        set<DiscreteConfig> *ptr_set_par_combs = &(this_node->set_discrete_parents_combinations);
        for (auto &par_comb : *ptr_set_par_combs) {    // For each column in CPT. Because the sum over column of CPT must be 1.
          int denominator = 0;
          for (int s = 0; s < dts->num_instance; ++s) {
            int compatibility = 1;  // We assume compatibility is 1,
            // and set it to 0 if we find that (*it_par_comb) is not compatible.
            // If we support learning with incomplete data,
            // the compatibility can be between 0 and 1.

            for (const auto &index_value : par_comb) {
              if (dts->dataset_all_vars[s][index_value.first] != index_value.second) {
                compatibility = 0;
                break;
              }
            }
            denominator += compatibility;
            int query = dts->dataset_all_vars[s][i];
            (*CPT)[query][par_comb] += compatibility;
          }
          // Normalize so that the sum is 1.
          for (int j = 0; j < this_node->num_potential_vals; ++j) {
            int query = this_node->potential_vals[j];
            (*CPT)[query][par_comb] /= denominator;
          }
        }

      }
    }
  }   // end of: #pragma omp parallel
  cout << "==================================================" << '\n'
       << "Finish training with known structure and complete data." << endl;

  if (print_params) {
    cout << "==================================================" << '\n'
         << "Each node's conditional probability table: " << endl;
    for (const auto &node_ptr : set_node_ptr_container) {  // For each node
      dynamic_cast<DiscreteNode*>(node_ptr)->PrintProbabilityTable();
    }
  }

  gettimeofday(&end,NULL);
  diff = (end.tv_sec-start.tv_sec) + ((double)(end.tv_usec-start.tv_usec))/1.0E6;
  setlocale(LC_NUMERIC, "");
  cout << "==================================================" << '\n'
       << "The time spent to learn the parameters is " << diff << " seconds" << endl;

}


void Network::ClearParams() {
  for (auto &n_p : this->set_node_ptr_container) {
    n_p->ClearParams();
  }
}


DiscreteConfig Network::ConstructEvidence(int *nodes_indexes, int *observations, int num_of_observations) {
  DiscreteConfig result;
  pair<int, int> p;
  for (int i=0; i<num_of_observations; ++i) {
    p.first = nodes_indexes[i];
    p.second = observations[i];
    result.insert(p);
  }
  return result;
}


vector<Factor> Network::ConstructFactors(int *Z, int nz, Node *Y) {
  vector<Factor> factors_list;
  factors_list.push_back(Factor(dynamic_cast<DiscreteNode*>(Y)));
  for (int i=0; i<nz; ++i) {
    Node* n = FindNodePtrByIndex(Z[i]);
    factors_list.push_back(Factor(dynamic_cast<DiscreteNode*>(n)));
  }
  return factors_list;
}


void Network::LoadEvidenceIntoFactors(vector<Factor> *factors_list, DiscreteConfig E, set<int> all_related_vars) {

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
    for (int i=0; i<factors_list->size(); ++i) {
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
          f.CopyFactor(f.SumOverVar(v));
        }
      }
      //--------------------------------------------------------------------------------
    }

  }   // end of: #pragma omp parallel
}


Factor Network::SumProductVarElim(vector<Factor> factors_list, int *Z, int nz) {
  for (int i=0; i<nz; ++i) {
    vector<Factor> tempFactorsList;
    Node* nodePtr = FindNodePtrByIndex(Z[i]);
    // Move every factor that is related to the node Z[i] from factors_list to tempFactorsList.
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


Factor Network::VarElimInferReturnPossib(int *Z, int nz, DiscreteConfig E, Node *Y) {
  // Z is the array of variable elimination order.
  // E is the evidences.
  vector<Factor> factorsList = ConstructFactors(Z, nz, Y);

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
  all_related_vars.insert(Y->GetNodeIndex());
  for (int i=0; i<nz; ++i) {all_related_vars.insert(Z[i]);}
  //--------------------------------------------------------------------------------


  LoadEvidenceIntoFactors(&factorsList, E, all_related_vars);
  Factor F = SumProductVarElim(factorsList, Z, nz);
  F.Normalize();
  return F;
}


Factor Network::VarElimInferReturnPossib(DiscreteConfig E, Node *Y) {
  pair<int*, int> simplified_elimination_order = SimplifyDefaultElimOrd(E);
  return this->VarElimInferReturnPossib(
                  simplified_elimination_order.first,
                  simplified_elimination_order.second,
                  E,
                  Y
               );
}


int Network::PredictUseVarElimInfer(int *Z, int nz, DiscreteConfig E, int Y_index) {
  Node *Y = FindNodePtrByIndex(Y_index);
  Factor F = VarElimInferReturnPossib(Z, nz, E, Y);
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


int Network::PredictUseVarElimInfer(DiscreteConfig E, int Y_index) {
  Node *Y = FindNodePtrByIndex(Y_index);
  Factor F = VarElimInferReturnPossib(E, Y);

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


double Network::TestNetReturnAccuracy(Dataset *dts) {

  cout << "==================================================" << '\n'
       << "Begin testing the trained network." << endl;

  struct timeval start, end;
  double diff;
  gettimeofday(&start,NULL);

  cout << "Progress indicator: ";
  int num_of_correct=0, num_of_wrong=0, m=dts->num_instance, m20= m / 20, progress=0;

//  int num_cores = omp_get_num_procs();
//  omp_set_num_threads(num_cores);
  // For each sample in test set
  #pragma omp parallel for
  for (int i=0; i<m; ++i) {

    #pragma omp critical
    { ++progress; }

    if (progress % m20 == 0) {
      cout << (double)progress/m * 100 << "%... " << endl;
    }


    // For now, only support complete data.
    int e_num = num_nodes - 1, *e_index = new int[e_num], *e_value = new int[e_num];
    for (int j = 0; j < num_nodes; ++j) {
      if (j == dts->class_var_index) {continue;}
      e_index[j < dts->class_var_index ? j : j - 1] = j;
      e_value[j < dts->class_var_index ? j : j - 1] = dts->dataset_all_vars[i][j];
    }
    DiscreteConfig E = ConstructEvidence(e_index, e_value, e_num);
    int label_predict = PredictUseVarElimInfer(E, 0); // The root node (label) has index of 0.
    if (label_predict == dts->dataset_all_vars[i][dts->class_var_index]) {
      #pragma omp critical
      { ++num_of_correct; }
    } else {
      #pragma omp critical
      { ++num_of_wrong; }
    }

    delete[] e_index;
    delete[] e_value;
  }

  gettimeofday(&end,NULL);
  diff = (end.tv_sec-start.tv_sec) + ((double)(end.tv_usec-start.tv_usec))/1.0E6;
  setlocale(LC_NUMERIC, "");
  cout << "==================================================" << '\n'
       << "The time spent to test the accuracy is " << diff << " seconds" << endl;

  double accuracy = num_of_correct / (double)(num_of_correct+num_of_wrong);
  cout << '\n' << "Accuracy: " << accuracy << endl;
  return accuracy;
}


double Network::TestNetByApproxInferReturnAccuracy(Dataset *dts, int num_samp) {

  // implement by Gibbs sampling
  cout << "==================================================" << '\n'
       << "Begin testing the trained network." << endl;

  cout << "Progress indicator: ";

  int num_of_correct=0, num_of_wrong=0, m=dts->num_instance, m20= m / 20, progress=0;

  vector<DiscreteConfig> samples = this->DrawSamplesByProbLogiSamp(10000);

//  #pragma omp parallel for
  for (int i=0; i<m; ++i) {  // For each sample in test set

//    #pragma omp critical
    { ++progress; }

    if (progress % m20 == 0) {
      cout << (double)progress/m * 100 << "%... " << endl;
    }


    // For now, only support complete data.
    int e_num=num_nodes-1, *e_index=new int[e_num], *e_value=new int[e_num];
    for (int j=0; j<num_nodes; ++j) {
      if (j == dts->class_var_index) { continue; }
      e_index[j < dts->class_var_index ? j : j - 1] = j + 1;
      e_value[j < dts->class_var_index ? j : j - 1] = dts->dataset_all_vars[i][j];
    }
    DiscreteConfig E = ConstructEvidence(e_index, e_value, e_num);
    int label_predict = ApproxInferByProbLogiRejectSamp(E, 0, samples); // The root node (label) has index of 0.
    if (label_predict == dts->dataset_all_vars[i][dts->class_var_index]) {
      ++num_of_correct;
    } else {
      ++num_of_wrong;
    }

  }
  double accuracy = num_of_correct / (double)(num_of_correct+num_of_wrong);
  cout << '\n' << "Accuracy: " << accuracy << endl;
  return accuracy;
}

double Network::TestAccuracyByLikelihoodWeighting(Dataset *dts, int num_samp) {
  cout << "==================================================" << '\n'
       << "Begin testing the trained network." << endl;

  struct timeval start, end;
  double diff;
  gettimeofday(&start,NULL);

  cout << "Progress indicator: ";

  int num_of_correct=0, num_of_wrong=0, m=dts->num_instance, m20= m / 20, progress=0;

  #pragma omp parallel for
  for (int i=0; i<m; ++i) {  // For each sample in test set

    #pragma omp critical
    { ++progress; }

    if (progress % m20 == 0) {
      cout << (double)progress/m * 100 << "%... " << endl;
    }


    // For now, only support complete data.
    int e_num=num_nodes-1, *e_index=new int[e_num], *e_value=new int[e_num];
    for (int j=0; j<num_nodes; ++j) {
      if (j == dts->class_var_index) { continue; }
      e_index[j < dts->class_var_index ? j : j - 1] = j;
      e_value[j < dts->class_var_index ? j : j - 1] = dts->dataset_all_vars[i][j];
    }
    DiscreteConfig E = ConstructEvidence(e_index, e_value, e_num);
    int label_predict = ApproxinferByLikelihoodWeighting(E, 0, num_samp); // The root node (label) has index of 0.
    if (label_predict == dts->dataset_all_vars[i][dts->class_var_index]) {
      #pragma omp critical
      { ++num_of_correct; }
    } else {
      #pragma omp critical
      { ++num_of_wrong; }
    }

    delete[] e_index;
    delete[] e_value;

  }

  gettimeofday(&end,NULL);
  diff = (end.tv_sec-start.tv_sec) + ((double)(end.tv_usec-start.tv_usec))/1.0E6;
  setlocale(LC_NUMERIC, "");
  cout << "==================================================" << '\n'
       << "The time spent to test the accuracy is " << diff << " seconds" << endl;

  double accuracy = num_of_correct / (double)(num_of_correct+num_of_wrong);
  cout << '\n' << "Accuracy: " << accuracy << endl;
  return accuracy;
}



DiscreteConfig Network::ProbLogicSampleNetwork() {
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
  for (const auto &index : this->GetTopoOrd()) {
    Node *n_p = FindNodePtrByIndex(index);
    bool observed = false;
    for (const auto &p : evidence) {
      if (index==p.first) {
        observed = true;
        // Set the sample value to be the same as the evidence.
        instance.insert(pair<int, int>(index, p.second));
        // Update the weight.
        if(!n_p->set_parents_ptrs.empty()) {
          set<int> parents_indexes;
          for (const auto &par : n_p->set_parents_ptrs) {
            parents_indexes.insert(par->GetNodeIndex());
          }
          DiscreteConfig parents_index_value;
          for (const auto &i : instance) {
            if (parents_indexes.find(i.first) != parents_indexes.end()) {
              parents_index_value.insert(i);
            }
          }
          weight *= dynamic_cast<DiscreteNode*>(n_p)->map_cond_prob_table[p.second][parents_index_value];
        } else {
          weight *= dynamic_cast<DiscreteNode*>(n_p)->map_marg_prob_table[p.second];
        }
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


vector<pair<DiscreteConfig, double>> Network::DrawSamplesByLikelihoodWeighting(const DiscreteConfig &evidence, int num_samp) {
  vector<pair<DiscreteConfig, double>> results;
  #pragma omp parallel for
  for (int i=0; i<num_samp; ++i) {
    auto samp = DrawOneLikelihoodWeightingSample(evidence);
    #pragma omp critical
    { results.push_back(samp); }
  }
  return results;
}

Factor Network::CalcuMargWithLikelihoodWeightingSamples(const vector<pair<DiscreteConfig, double>> &samples, const int &node_index) {
  map<int, double> value_weight;
  Node *n_p = this->FindNodePtrByIndex(node_index);

  // Initialize the map.
  for (int i=0; i<n_p->num_potential_vals; ++i) {
    value_weight[n_p->potential_vals[i]] = 0;
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
  Factor f;
  set<int> rv;
  rv.insert(node_index);
  set<DiscreteConfig> sc;
  for (int i=0; i<n_p->num_potential_vals; ++i) {
    DiscreteConfig c;
    c.insert(pair<int, int>(node_index, n_p->potential_vals[i]));
    sc.insert(c);
  }
  map<DiscreteConfig, double> mp;
  for (const auto &c : sc) {
    int value = (*c.begin()).second;
    mp[c] = value_weight[value];
  }
  f.SetMembers(rv, sc, mp);
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
  for (auto &par_ptr : node_ptr->set_parents_ptrs) {
    markov_blanket_node_index.insert(par_ptr->GetNodeIndex());
  }

  // Add children and parents of children.
  for (auto &chil_ptr : node_ptr->set_children_ptrs) {
    markov_blanket_node_index.insert(chil_ptr->GetNodeIndex());
    for (auto &par_chil_ptr : chil_ptr->set_parents_ptrs) {
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

  auto it_node = this->set_node_ptr_container.begin();


  // Need burning in.
//  #pragma omp parallel for
  for (int i=1; i<num_burn_in+num_samp; ++i) {

    Node *node_ptr = *(it_node++);
    if (it_node==set_node_ptr_container.end()) {
      it_node = this->set_node_ptr_container.begin();
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
        p.second = node_ptr->potential_vals[value_index];
        single_sample.insert(p);
        break;
      }
    }

    // After burning in, we can store the samples now.
    #pragma omp critical
    { if (i>=num_burn_in) {samples.push_back(single_sample);} }
  }

  return samples;
}


int Network::SampleNodeGivenMarkovBlanketReturnValIndex(Node *node_ptr, DiscreteConfig markov_blanket) {
  int num_elim_ord = markov_blanket.size();
  int *var_elim_ord = new int[num_elim_ord];
  int temp = 0;
  for (auto &n_v : markov_blanket) {
    var_elim_ord[temp++] = n_v.first;
  }

  Factor f = VarElimInferReturnPossib(var_elim_ord, num_elim_ord, markov_blanket, node_ptr);

  vector<int> weights;
  for (int i=0; i<node_ptr->num_potential_vals; ++i) {
    DiscreteConfig temp;
    temp.insert(pair<int,int>(node_ptr->GetNodeIndex(),node_ptr->potential_vals[i]));
    weights.push_back(f.map_potentials[temp]*10000);
  }

  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  default_random_engine rand_gen(seed);
  discrete_distribution<int> this_distribution(weights.begin(),weights.end());
  return this_distribution(rand_gen);
}


int Network::ApproxInferByProbLogiRejectSamp(DiscreteConfig e, Node *node, vector<DiscreteConfig> &samples) {
  DiscreteConfig possb_values;
  for (int i=0; i<node->num_potential_vals; ++i) {
    possb_values.insert(pair<int,int>(node->GetNodeIndex(),node->potential_vals[i]));
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
    uniform_int_distribution<int> this_distribution(0,node->num_potential_vals-1);
    return node->potential_vals[this_distribution(rand_gen)];
  }

  // Find the argmax.
  int lable_index_predict = -1;
  int max_occurred = 0;
  for (int i=0; i<node->num_potential_vals; ++i) {
    if (lable_index_predict==-1 || count_each_value[i]>max_occurred) {
      lable_index_predict = i;
      max_occurred = count_each_value[i];
    }
  }

  // Return the predicted label instead of the index.
  return node->potential_vals[lable_index_predict];
}


int Network::ApproxInferByProbLogiRejectSamp(DiscreteConfig e, int node_index, vector<DiscreteConfig> &samples) {
  return ApproxInferByProbLogiRejectSamp(e, FindNodePtrByIndex(node_index), samples);
}




pair<double, set<Node*>> Network::F(Node *node, set<Node*> candidate_parents) {
  
}


#pragma clang diagnostic pop