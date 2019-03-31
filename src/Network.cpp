//
// Created by Linjian Li on 2018/11/29.
//

#include "Network.h"


void Network::PrintNetworkStruct() {
  for (auto &node_ptr : set_node_ptr_container) {
    cout << node_ptr->GetNodeIndex() << ":\t";
    for (auto &par_node_ptr : node_ptr->set_parents_ptrs) {
      cout << par_node_ptr->GetNodeIndex() << '\t';
    }
    cout << endl;
  }
}

Node* Network::FindNodePtrByIndex(int index) {
  if (index<0 || index>num_nodes) {
    fprintf(stderr, "Error in function %s! \nInvalid index!", __FUNCTION__);
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


Node* Network::FindNodePtrByName(string name) {
  Node* node_ptr = nullptr;
  for (auto n_ptr : set_node_ptr_container) {
    if (n_ptr->node_name==name) {
      node_ptr = n_ptr;
      break;
    }
  }
  return node_ptr;
}


void Network::SetParentChild(int p_index, int c_index) {
  Node *p = FindNodePtrByIndex(p_index), *c = FindNodePtrByIndex(c_index);
  SetParentChild(p,c);
}


void Network::SetParentChild(Node *p, Node *c) {
  p->AddChild(c);
  c->AddParent(p);
}


void Network::RemoveParentChild(int p_index, int c_index) {
  Node *p = FindNodePtrByIndex(p_index), *c = FindNodePtrByIndex(c_index);
  RemoveParentChild(p,c);
}


vector<int> Network::GenTopoOrd() {

  // First, convert the network to a directed adjacency matrix.
  int **graph = new int*[num_nodes];
  for (int i=0; i<num_nodes; ++i) {graph[i] = new int[num_nodes]();}
  for (auto &n_p : set_node_ptr_container) {
    for (auto &p_p : n_p->set_children_ptrs) {
      graph[n_p->GetNodeIndex()][p_p->GetNodeIndex()] = 1;
    }
  }

  topo_ord = TopoSortOfDAGZeroInDegreeFirst(graph, num_nodes);
  return topo_ord;
}


void Network::RemoveParentChild(Node *p, Node *c) {
  p->RemoveChild(c);
  c->RemoveParent(p);
}


void Network::LearnParmsKnowStructCompData(const Trainer *trainer){
  cout << "=======================================================================" << '\n'
       << "Begin learning parameters with known structure and complete data." << endl;

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

  // For every feature node.
  for (int i=1; i<trainer->num_vars; ++i) { // Because feature index start at 1.
                                                     // Using "train_set_y_X".
    Node *this_node = FindNodePtrByIndex(i);

    map<int, map<Combination, double> >* CPT = &(this_node->map_cond_prob_table);
    set<Combination>* ptr_set_par_combs = &(this_node->set_parents_combinations);
    for (auto &par_comb : *ptr_set_par_combs) {    // For each column in CPT. Because the sum over column of CPT must be 1.
      int denominator = 0;
      for (int s=0; s<trainer->num_train_instance; ++s) {
        int compatibility = 1;  // We assume compatibility is 1,
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
      for (int j=0; j<this_node->num_potential_vals; ++j) {
        int query = this_node->potential_vals[j];
        (*CPT)[query][par_comb] /= denominator;
      }
    }
  }
  cout << "=======================================================================" << '\n'
       << "Finish training with known structure and complete data." << endl;

  // The following code are just to print the result.
  cout << "=======================================================================" << '\n'
       << "Each node's conditional probability table: " << endl;
  for (auto thisNode : set_node_ptr_container) {  // For each node
    cout << thisNode->GetNodeIndex() << ":\t";


    if (thisNode->set_parents_ptrs.empty()) {    // If this node has no parents
      for(int i=0; i<thisNode->num_potential_vals; i++) {    // For each row of MPT
        int query = thisNode->potential_vals[i];
        cout << "P(" << query << ")=" << thisNode->map_marg_prob_table[query] << '\t';
      }
      cout << endl;
      continue;
    }


    for(int i=0; i<thisNode->num_potential_vals; ++i) {    // For each row of CPT
      int query = thisNode->potential_vals[i];
      for (auto itParCom = thisNode->set_parents_combinations.begin(); itParCom != thisNode->set_parents_combinations.end(); ++itParCom) {  // For each column of CPT
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
  factors_list.push_back(Factor(Y));
  for (int i=0; i<nz; i++) {
    Node* n = FindNodePtrByIndex(Z[i]);
    factors_list.push_back(Factor(n));
  }
  return factors_list;
}


void Network::LoadEvidence(vector<Factor> *factors_list, Combination E) {
  for (auto &f : *factors_list) {  // For each factor
    for (auto &e : E) {  // For each node's observation in E
      if (f.related_variables.find(e.first)!=f.related_variables.end()) {  // If this factor is related to this node
        for (auto &comb : f.set_combinations) {  // Update each row of map_potentials
          if (comb.find(e)==comb.end()) {
            f.map_potentials[comb] = 0;
          }
        }
      }
    }
  }
}


Factor Network::SumProductVarElim(vector<Factor> factors_list, int *Z, int nz) {
  for (int i=0; i<nz; i++) {
    vector<Factor> tempFactorsList;
    Node* nodePtr = FindNodePtrByIndex(Z[i]);
    // Move every factor that is related to the node Z[i] from factors_list to tempFactorsList.
    /*
     * Note: This for loop does not contain "it++" in the parentheses.
     *      Because if we do so, it may cause some logic faults which, however, will not cause runtime error, so hard to debug.
     *      For example:
     *        When "it" reaches the second to last element, and this element is related to the node.
     *        Then this element will be erase from factors_list, and then "it++" which will move "it" to the end.
     *        Then the for loop will end because "it" has reached the end.
     *        However, at this time, the last element has been ignored, even if it is related to the node.
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
    Factor newFactor = tempFactorsList.back().SumOverVar(nodePtr);
    factors_list.push_back(newFactor);
  }

  /*
   *   If we are calculating a node's posterior probability given evidence about its children,
   *   then when the program runs to here,
   *   the "factors_list" will contain several factors about the same node which is the query node Y.
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

  return factors_list.back();  // After all the processing shown above, the only remaining factor is the factor about Y.
}


Factor Network::VarElimInferReturnPossib(int *Z, int nz, Combination E, Node *Y) {
  vector<Factor> factorsList = ConstructFactors(Z, nz, Y);
  LoadEvidence(&factorsList, E);
  Factor F = SumProductVarElim(factorsList, Z, nz);
  F.Normalize();
  return F;
}


Factor Network::VarElimInferReturnPossib(Combination E, Node *Y) {
  pair<int*, int> simplified_elimination_order = SimplifyDefaultElimOrd(E);
  return this->VarElimInferReturnPossib(simplified_elimination_order.first, simplified_elimination_order.second, E, Y);
}


int Network::PredictUseVarElimInfer(int *Z, int nz, Combination E, int Y_index) {
  Node *Y = FindNodePtrByIndex(Y_index);
  Factor F = VarElimInferReturnPossib(Z, nz, E, Y);
  double max_prob = 0;
  Combination comb_predict;
  for (auto &comb : F.set_combinations) {
    if (F.map_potentials[comb] > max_prob) {
      max_prob = F.map_potentials[comb];
      comb_predict = comb;
    }
  }
  int label_predict = comb_predict.begin()->second;
  return label_predict;
}


int Network::PredictUseVarElimInfer(Combination E, int Y_index) {
  Node *Y = FindNodePtrByIndex(Y_index);
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
  return label_predict;
}


double Network::TestNetReturnAccuracy(Trainer *tester) {

  cout << "=======================================================================" << '\n'
       << "Begin testing the trained network." << endl;

  cout << "Progress indicator: ";

  int num_of_correct=0, num_of_wrong=0, m=tester->num_train_instance, m10=m/10, percent=0;

  for (int i=0; i<m; i++) {  // For each sample in test set

    if (i%m10==0) {
      cout << (percent++)*10 << "%... " << flush;
    }


    // For now, only support complete data.
    int e_num=num_nodes-1, *e_index=new int[e_num], *e_value=new int[e_num];
    for (int j=0; j<e_num; ++j) {
      e_index[j] = j+1;
      e_value[j] = tester->train_set_X[i][j];
    }
    Combination E = ConstructEvidence(e_index, e_value, e_num);
    int label_predict = PredictUseVarElimInfer(E, 0); // The root node (label) has index of 0.
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


vector<int> Network::TopoSort() {
  // Convert network to directed adjacency matrix.
  int num_nodes = num_nodes;
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

  // Topological sort.
  topo_ord = TopoSortOfDAGZeroInDegreeFirst(adjac_matrix, num_nodes);

  return topo_ord;
}


Combination Network::SampleNetwork() {
  // todo: implement
  if (topo_ord.empty()) {
    fprintf(stderr, "Error in function %s!", __FUNCTION__);
    fprintf(stderr, "Do not have a topological order of nodes!");
    exit(1);
  }
  Combination instance;
  for (auto &index : topo_ord) {
    Node *n_p = FindNodePtrByIndex(index);
    instance.insert(pair<int,int>(index,n_p->SampleNodeGiven(instance)));
  }
  return instance;
}