//
// Created by LinjianLi on 2019/2/16.
//

#include "JunctionTree.h"

JunctionTree::JunctionTree(Network *net): JunctionTree(net, "min-nei", true) {}

JunctionTree::JunctionTree(Network *net, bool elim_redundant_cliques): JunctionTree(net, "min-nei", elim_redundant_cliques) {}


JunctionTree::JunctionTree(Network *net, string elim_ord_strategy, bool elim_redundant_cliques) {

  struct timeval start, end;
  double diff;
  gettimeofday(&start,NULL);

  network = net;

  int **direc_adjac_matrix = ConvertDAGNetworkToAdjacencyMatrix(network);
  Moralize(direc_adjac_matrix, network->num_nodes);
  int **undirec_adjac_matrix = direc_adjac_matrix;  // Change a name because it has been moralized.

  // There are different ways of determining elimination ordering.
  vector<int> elim_ord;
  if (elim_ord_strategy == "min-nei") {
    elim_ord = MinNeighbourElimOrd(undirec_adjac_matrix, network->num_nodes);
  } else if (elim_ord_strategy == "rev-topo") {
    elim_ord = network->GetReverseTopoOrd();
  } else {
    fprintf(stderr, "The elimination ordering strategy should be one of the following:\n"
                    "{ min-nei, rev-topo }.");
    exit(1);
  }

  elimination_ordering = elim_ord;
  Triangulate(network, undirec_adjac_matrix, network->num_nodes, elim_ord, set_clique_ptr_container);


  if (elim_redundant_cliques) {
    // Theoretically, this step is not necessary.
    ElimRedundantCliques();
  }

  FormJunctionTree(set_clique_ptr_container);

  NumberTheCliquesAndSeparators();

  for (const auto &c : set_clique_ptr_container) {
    map_elim_var_to_clique[c->elimination_variable_index] = c;
  }

  AssignPotentials();
  BackUpJunctionTree();

  gettimeofday(&end,NULL);
  diff = (end.tv_sec-start.tv_sec) + ((double)(end.tv_usec-start.tv_usec))/1.0E6;
  setlocale(LC_NUMERIC, "");
  cout << "==================================================" << '\n'
       << "The time spent to construct junction tree is " << diff << " seconds" << endl;
  delete[] direc_adjac_matrix;
}


JunctionTree::JunctionTree(JunctionTree *jt) {
  this->network = jt->network;


  // The following block is to initialize the matrices
  // that are used to record the connections in order to restore them.
  // --------------------------------------------------------------------------
  int **seps_that_cliques_connect_to = new int* [jt->set_clique_ptr_container.size()],
      **cliques_that_seps_connect_to = new int* [jt->set_separator_ptr_container.size()];
  for (int i=0; i<jt->set_clique_ptr_container.size(); ++i) {
    seps_that_cliques_connect_to[i] = new int[jt->set_separator_ptr_container.size()]();
  }
  for (int i=0; i<jt->set_separator_ptr_container.size(); ++i) {
    cliques_that_seps_connect_to[i] = new int[jt->set_clique_ptr_container.size()]();
  }
  // --------------------------------------------------------------------------


  // The following block copy the cliques and separators without connections.
  // --------------------------------------------------------------------------
  map<int, Clique*> map_cliques;
  map<int, Separator*> map_separators;

  for (const auto &c : jt->set_clique_ptr_container) {
    map_cliques[c->clique_id] = c->CopyWithoutPtr();
    this->set_clique_ptr_container.insert(map_cliques[c->clique_id]);
    for (const auto &s_p : c->set_neighbours_ptr) {
      seps_that_cliques_connect_to[c->clique_id][s_p->clique_id] = 1; // Record the connections.
    }
  }
  for (const auto &s : jt->set_separator_ptr_container) {
    map_separators[s->clique_id] = s->CopyWithoutPtr();
    this->set_separator_ptr_container.insert(map_separators[s->clique_id]);
    for (const auto &c_p : s->set_neighbours_ptr) {
      cliques_that_seps_connect_to[s->clique_id][c_p->clique_id] = 1; // Record the connections
    }
  }
  // --------------------------------------------------------------------------


  // The following block is to restore the connections.
  // --------------------------------------------------------------------------
//  #pragma omp parallel for collapse(2)
  for (int i=0; i<jt->set_clique_ptr_container.size(); ++i) {
    for (int j=0; j<jt->set_separator_ptr_container.size(); ++j) {
      if (seps_that_cliques_connect_to[i][j]==1) {
        map_cliques[i]->set_neighbours_ptr.insert(map_separators[j]);
      }
    }
  }

  for (int i=0; i<jt->set_separator_ptr_container.size(); ++i) {
    for (int j=0; j<jt->set_clique_ptr_container.size(); ++j) {
      if (cliques_that_seps_connect_to[i][j]==1) {
        map_separators[i]->set_neighbours_ptr.insert(map_cliques[j]);
      }
    }
  }
  // --------------------------------------------------------------------------

  this->BackUpJunctionTree();

  for (int i=0; i<jt->set_separator_ptr_container.size(); ++i) {
    delete[] cliques_that_seps_connect_to[i];
  }
  delete[] cliques_that_seps_connect_to;

  for (int i=0; i<jt->set_clique_ptr_container.size(); ++i) {
    delete[] seps_that_cliques_connect_to[i];
  }
  delete[] seps_that_cliques_connect_to;
}


int** JunctionTree::ConvertDAGNetworkToAdjacencyMatrix(Network *net) {
  int num_nodes = net->num_nodes;
  int **adjac_matrix = new int* [num_nodes];
  for (int i=0; i<num_nodes; ++i) {
    adjac_matrix[i] = new int[num_nodes]();
  }
  for (auto &node_ptr : net->set_node_ptr_container) {
    int from, from2, to;
    from = node_ptr->GetNodeIndex();
    for (auto &child_ptr : node_ptr->set_children_ptrs) {
      to = child_ptr->GetNodeIndex();
      adjac_matrix[from][to] = 1;
    }
  }
  return adjac_matrix;
}


void JunctionTree::Moralize(int **direc_adjac_matrix, int &num_nodes) {
//  #pragma omp parallel for collapse(2)
  for (int i=0; i<num_nodes; ++i) {
    for (int j=0; j<num_nodes; ++j) {
      if (direc_adjac_matrix[i][j]==1 || direc_adjac_matrix[j][i]==1) {
        direc_adjac_matrix[i][j] = 1;
        direc_adjac_matrix[j][i] = 1;
      }
    }
  }
}



vector<int> JunctionTree::MinNeighbourElimOrd(int **adjac_matrix, int &num_nodes) {
  vector< pair<int,int> > to_be_sorted;
  for (int i=0; i<num_nodes; ++i) {
    pair<int,int> p;
    p.first = i;
    p.second = 0;
    for (int j=0; j<num_nodes; ++j) {
      if (adjac_matrix[i][j]==1) {++p.second;}
    }
    to_be_sorted.push_back(p);
  }
  sort(to_be_sorted.begin(), to_be_sorted.end(), [](pair<int,int> a, pair<int,int> b){return a.second < b.second;});  // Using lambda expression.
  vector< pair<int,int> > &sorted = to_be_sorted;
  vector<int> result;
  result.reserve(sorted.size());
  for (auto &p : sorted) {
    result.push_back(p.first);
  }
  return result;
}



void JunctionTree::Triangulate(Network *net,
                               int **adjac_matrix,
                               int &num_nodes,
                               vector<int> elim_ord,
                               set<Clique*> &cliques) {
  if (elim_ord.size()==0) {return;}
  set<int> set_neighbours;
  set<Node*> set_node_ptrs_to_form_a_clique;
  int first_node_in_elim_ord = elim_ord.front();
  set_node_ptrs_to_form_a_clique.insert(net->FindNodePtrByIndex(first_node_in_elim_ord));
  for (int j=0; j<num_nodes; ++j) {
    if (adjac_matrix[first_node_in_elim_ord][j]==1) {
      set_neighbours.insert(j);
    }
  }

  // Form a clique.
  for (auto &nei : set_neighbours) {
    for (auto &index2 : set_neighbours) {
      if (nei!=index2) {
        // Connect its neighbours to each other.
        adjac_matrix[nei][index2] = 1;
        adjac_matrix[index2][nei] = 1;
      }
    }
    set_node_ptrs_to_form_a_clique.insert(net->FindNodePtrByIndex(nei));
  }

  cliques.insert(new Clique(set_node_ptrs_to_form_a_clique, first_node_in_elim_ord));

  
  // Remove the first node in elimination ordering, which has already form a clique.
  elim_ord.erase(elim_ord.begin());
  // The node has been removed, so the edges connected to it should be removed too.
  for (auto &nei : set_neighbours) {
    adjac_matrix[first_node_in_elim_ord][nei] = 0;
    adjac_matrix[nei][first_node_in_elim_ord] = 0;
  }

  Triangulate(net, adjac_matrix, num_nodes, elim_ord, cliques);
}


void JunctionTree::ElimRedundantCliques() {
  set<Clique*> to_be_eliminated;

  for (auto &ptr_clq1 : set_clique_ptr_container) {
    for (auto &ptr_clq2 : set_clique_ptr_container) {
      if (ptr_clq1==ptr_clq2) {continue;}
      Clique *ptr_smaller_clique, *ptr_bigger_clique;
      if (ptr_clq1->related_variables.size() < ptr_clq2->related_variables.size()) {
        ptr_smaller_clique = ptr_clq1;
        ptr_bigger_clique = ptr_clq2;
      } else {
        ptr_smaller_clique = ptr_clq2;
        ptr_bigger_clique = ptr_clq1;
      }
      set<int> intersection;
      set_intersection(ptr_smaller_clique->related_variables.begin(),ptr_smaller_clique->related_variables.end(),
                       ptr_bigger_clique->related_variables.begin(),ptr_bigger_clique->related_variables.end(),
                       std::inserter(intersection,intersection.begin()));
      if (intersection==ptr_smaller_clique->related_variables) {
        to_be_eliminated.insert(ptr_smaller_clique);
        break;
      }
    }
  }

  for (auto &ptr_clq : to_be_eliminated) {
    set_clique_ptr_container.erase(ptr_clq);
  }
}


void JunctionTree::FormJunctionTree(set<Clique*> &cliques) {

  // First, generate all possible separators.
  set<pair<Clique*,Clique*>> mark;
  set<Separator*> all_possible_seps;
  for (auto &clique_ptr : cliques) {
    for (auto &clique_ptr_2 : cliques) {

      if (clique_ptr==clique_ptr_2) {
        continue; // The same cliques do not need a separator.
      } else if (mark.find(pair<Clique*,Clique*>(clique_ptr,clique_ptr_2))!=mark.end()) {
        continue; // The separator of this pair of cliques has been generated.
      }

      // Mark this pair.
      mark.insert(pair<Clique*,Clique*>(clique_ptr,clique_ptr_2));
      mark.insert(pair<Clique*,Clique*>(clique_ptr_2,clique_ptr));

      set<int> common_related_variables;
      set_intersection(clique_ptr->related_variables.begin(),clique_ptr->related_variables.end(),
                       clique_ptr_2->related_variables.begin(),clique_ptr_2->related_variables.end(),
                       std::inserter(common_related_variables,common_related_variables.begin()));

      // If they have no common variables, then they will not be connected by separator.
      if (common_related_variables.empty()) {continue;}

      set<Node*> common_related_node_ptrs;
      for (auto &v : common_related_variables) {
        common_related_node_ptrs.insert(network->FindNodePtrByIndex(v));
      }

      Separator *sep = new Separator(common_related_node_ptrs);

      // Let separator know the two cliques that it connects to.
      sep->set_neighbours_ptr.insert(clique_ptr);
      sep->set_neighbours_ptr.insert(clique_ptr_2);

      all_possible_seps.insert(sep);

    }
  }

  // Second, use Prim's algorithm to form a maximum spanning tree.
  // If we construct a maximum spanning tree by the weights of the separators,
  // then the tree will satisfy running intersection property.
  set<Clique*> tree_so_far;
  tree_so_far.insert(*cliques.begin()); // randomly insert a clique in tree
  while (tree_so_far.size()<cliques.size()) {
    Separator* max_weight_sep = nullptr;
    for (auto &sep_ptr : all_possible_seps) {
      auto iter = sep_ptr->set_neighbours_ptr.begin();
      Clique *clq1 = *iter, *clq2 = *(++iter);

      // If one of the cliques connected
      // by this separator is in the tree_so_far.
      if ((tree_so_far.find(clq1)!=tree_so_far.end()
          &&
          tree_so_far.find(clq2)==tree_so_far.end())
          ||
          (tree_so_far.find(clq1)==tree_so_far.end()
          &&
          tree_so_far.find(clq2)!=tree_so_far.end())) {
        // And if the weight of this separator is the largest.
        if (max_weight_sep==nullptr || max_weight_sep->weight < sep_ptr->weight) {
          max_weight_sep = sep_ptr;
        }
      }
    }

//    max_weight_sep->clique_id = set_separator_ptr_container.size();
    set_separator_ptr_container.insert(max_weight_sep);
    auto iter = max_weight_sep->set_neighbours_ptr.begin();
    Clique *clq1 = *iter, *clq2 = *(++iter);
    tree_so_far.insert(clq1);
    tree_so_far.insert(clq2);

  }   // end of: while. Until all cliques are in "tree_so_far"

  // Now let the cliques to know the separators that they connect to.
  for (auto &sep_ptr : set_separator_ptr_container) {
    auto iter = sep_ptr->set_neighbours_ptr.begin();
    Clique *clq1 = *iter, *clq2 = *(++iter);
    clq1->set_neighbours_ptr.insert(sep_ptr);
    clq2->set_neighbours_ptr.insert(sep_ptr);
  }
}


void JunctionTree::NumberTheCliquesAndSeparators() {
  int i = 0;
  for (auto c : set_clique_ptr_container) {
    c->clique_id = i++;
  }
  int j = 0;
  for (auto s : set_separator_ptr_container) {
    s->clique_id = j++;
  }
}


void JunctionTree::AssignPotentials() {

  // todo: test the correctness of the continuous part (discrete part works correctly)

  // For purely discrete cliques, the potentials have been initialized to 1 on creation,
  // so we need to assign the probabilities of the network nodes to these cliques.
  // For continuous cliques, the lp_potentials and post_bags are set to empty list,
  // so we need to assign the CG regressions to these cliques. The method is described in
  // [Local Propagation in Conditional Gaussian Bayesian Networks (Cowell, 2005)], section 5.2.

  // First, extract the information of nodes.
  // Convert the probabilities of discrete nodes to factors, which make the "multiply" operation easier.
  // Extract the CG regressions of continuous nodes.
  vector<Factor> factors; // Can not use std::set, because Factor does not have definition on operator "<".
  vector<CGRegression> cgrs;
  for (auto &node_ptr : network->set_node_ptr_container) {
    if (node_ptr->is_discrete) {
      factors.push_back(Factor(node_ptr));
    } else {  // If the node is continuous.
      cgrs.push_back(CGRegression(node_ptr));
    }
  }

  // Second, assign these factors and CG regressions to some appropriate cliques.
  // Each factor and CG regression should be use only once.

  // For potentials from discrete nodes, they should be assigned to purely discrete cliques.
  for (auto &f : factors) {
    for (auto &clique_ptr : set_clique_ptr_container) {

      if (f.related_variables.empty() || clique_ptr->related_variables.empty()) { break; }

      if (!clique_ptr->pure_discrete) { continue; }

      set<int> diff;
      set_difference(f.related_variables.begin(), f.related_variables.end(),
                     clique_ptr->related_variables.begin(), clique_ptr->related_variables.end(),
                     inserter(diff, diff.begin()));
      // If diff.empty(), that means that the set of related variables of the clique is a superset of the factor's.
      if (diff.empty()) {
        clique_ptr->MultiplyWithFactorSumOverExternalVars(f);
        break;  // Ensure that each factor is used only once.
      }
    }
  }
  for (auto &cgr : cgrs) {
    for (auto &clique_ptr : set_clique_ptr_container) {

      if (clique_ptr->pure_discrete) { continue; }

      set<int> cgr_related_vars = cgr.set_all_tail_index;
      cgr_related_vars.insert(cgr.head_var_index);
      set<int> diff;
      set_difference(cgr_related_vars.begin(), cgr_related_vars.end(),
                     clique_ptr->related_variables.begin(), clique_ptr->related_variables.end(),
                     inserter(diff, diff.begin()));
      // If diff.empty(), that means that the set of related variables of the clique is a superset of the CG regression's.
      if (diff.empty()) {
        if (clique_ptr->elimination_variable_index == cgr.head_var_index) {
          clique_ptr->lp_potential.push_back(cgr);
        } else {
          clique_ptr->post_bag.push_back(cgr);
        }
        break;  // Ensure that each CG regression is used only once.
      }
      // todo: fix it
      //   There is a problem when the discrete variables of cgr and clique_ptr is not the same.
      //   I don't know what will happen.
    }
  }
}

void JunctionTree::BackUpJunctionTree() {
  for (const auto &c : set_clique_ptr_container) {
    map_cliques_backup[c] = *c;
  }
  for (const auto &s : set_separator_ptr_container) {
    map_separators_backup[s] = *s;
  }
}

void JunctionTree::ResetJunctionTree() {
  for (auto &c : set_clique_ptr_container) {
    *c = map_cliques_backup[c];
  }
  for (auto &s : set_separator_ptr_container) {
    *s = map_separators_backup[s];
  }
}

//void JunctionTree::LoadEvidence(const Combination &E) {
//  if (E.empty()) { return; }
//  for (auto &e : E) {  // For each node's observation in E
//    for (auto &clique_ptr : set_clique_ptr_container) {  // For each cliqueauto tmp = map_elim_var_to_clique[83];
//      if (clique_ptr->related_variables.find(e.first)!=clique_ptr->related_variables.end()) {  // If this clique is related to this node
//        for (auto &comb : clique_ptr->set_disc_combinations) {  // Update each row of map_potentials
//          if (comb.find(e)==comb.end()) {
//            clique_ptr->map_potentials[comb] = 0;
//          }
//        }
//        // I do not know if the "break" is optional.
//        // Entering the evidence to one clique that contains it,
//        // or to all cliques that contain it.
//        // Are the results after message passing process both correct???
//        // todo: figure it out
//        break;
//      }
//    }
//  }
//}
void JunctionTree::LoadEvidence(const Combination &E) {
  if (E.empty()) { return; }
  for (auto &e : E) {  // For each node's observation in E
    Clique* clique_ptr = map_elim_var_to_clique[e.first];
    for (auto &comb : clique_ptr->set_disc_combinations) {  // Update each row of map_potentials
      if (comb.find(e)==comb.end()) {
        clique_ptr->map_potentials[comb] = 0;
      }
    }
  }
}

void JunctionTree::MessagePassingUpdateJT() {
  // Arbitrarily select a clique as the root.
  auto iter = set_clique_ptr_container.begin();
  Clique *arb_root = *iter;
  arb_root->Collect();
  arb_root->Distribute();
}

void JunctionTree::PrintAllCliquesPotentials() const {
  cout << "Cliques" << '\n';
  for (auto &c : set_clique_ptr_container) {
    c->PrintPotentials();
  }
  cout << "==================================================" << endl;
}

void JunctionTree::PrintAllSeparatorsPotentials() const {
  cout << "Separators" << '\n';
  for (auto &s : set_separator_ptr_container) {
    s->PrintPotentials();
  }
  cout << "==================================================" << endl;
}

Factor JunctionTree::BeliefPropagationReturnPossib(set<int> &indexes) {

  // The input is a set of indexes of variables.
  // The output is a factor representing the joint marginal of these variables.

  int min_potential_size = INT32_MAX;
  Clique *selected_clique = nullptr;

  // The case where the query variables are all appear in one clique.
  // Find the clique that contains this variable,
  // whose size of potentials table is the smallest,
  // which can reduce the number of sum operation.
  for (auto &c : set_clique_ptr_container) {
    set<int> diff;
    set_difference(indexes.begin(), indexes.end(),
                   c->related_variables.begin(), c->related_variables.end(),
                   inserter(diff, diff.begin()));
    if (!diff.empty()) {continue;}  // If diff is not empty, then this clique does not cover all indexes.
    if (c->map_potentials.size()>=min_potential_size) {continue;}
    min_potential_size = c->map_potentials.size();
    selected_clique = c;
  }

  set<int> diff;
  set_difference(selected_clique->related_variables.begin(), selected_clique->related_variables.end(),
                 indexes.begin(), indexes.end(),
                 inserter(diff, diff.begin()));
  Factor f;
  f.SetMembers(selected_clique->related_variables, selected_clique->set_disc_combinations, selected_clique->map_potentials);
  for (auto &index : diff) {
    f = f.SumOverVar(index);
  }
  f.Normalize();
  return f;

  // todo: implement the case where the query variables appear in different cliques.
}

int JunctionTree::InferenceUsingBeliefPropagation(set<int> &indexes) {
  Factor f = BeliefPropagationReturnPossib(indexes);
  double max_prob = 0;
  Combination comb_predict;
  for (auto &comb : f.set_combinations) {
    if (f.map_potentials[comb] > max_prob) {
      max_prob = f.map_potentials[comb];
      comb_predict = comb;
    }
  }
  int label_predict = comb_predict.begin()->second;
  return label_predict;
}

double JunctionTree::TestNetReturnAccuracy(int class_var, Trainer *tst) {
  set<int> query;
  query.insert(class_var);
  cout << "==================================================" << '\n'
       << "Begin testing the trained network." << endl;

  struct timeval start, end;
  double diff;
  gettimeofday(&start,NULL);

  cout << "Progress indicator: ";

  int num_of_correct=0, num_of_wrong=0, m=tst->num_instance, m20=m/20, progress=0;

  // If I use OpenMP to parallelize,
  // process may exit with code 137,
  // which means the memory consumption is too large.
  // I don't know how to solve yet.
//  #pragma omp parallel for
  for (int i=0; i<m; i++) {  // For each sample in test set

//    #pragma omp critical
    { ++progress; }

    if (progress % m20 == 0) {
      cout << (double)progress/m * 100 << "%... " << endl;
    }


    // For now, only support complete data.
    int e_num=network->num_nodes-1, *e_index=new int[e_num], *e_value=new int[e_num];
    for (int j=0; j<e_num; ++j) {
      e_index[j] = j+1;
      e_value[j] = tst->train_set_X[i][j];
    }
    Combination E = network->ConstructEvidence(e_index, e_value, e_num);

    delete[] e_index;
    delete[] e_value;

//    auto jt = new JunctionTree(this);
//    jt->LoadEvidence(E);
//    jt->MessagePassingUpdateJT();
//    int label_predict = jt->InferenceUsingBeliefPropagation(query); // The root node (label) has index of 0.
//    delete jt;
    LoadEvidence(E);
    MessagePassingUpdateJT();
    int label_predict = InferenceUsingBeliefPropagation(query); // The root node (label) has index of 0.
    ResetJunctionTree();

    if (label_predict == tst->train_set_y[i]) {
//      #pragma omp critical
      { ++num_of_correct; }
    } else {
//      #pragma omp critical
      { ++num_of_wrong; }
    }

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