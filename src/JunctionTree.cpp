//
// Created by LinjianLi on 2019/2/16.
//

#include "JunctionTree.h"

JunctionTree::JunctionTree(Network *net) {
  network = net;
  int **direc_adjac_matrix = ConvertDAGNetworkToAdjacencyMatrix(network);
  Moralize(direc_adjac_matrix, network->n_nodes);
  int **undirec_adjac_matrix = direc_adjac_matrix;  // Because it has been moralized.
  vector<int> elim_ord = MinNeighbourElimOrd(undirec_adjac_matrix, network->n_nodes);
  Triangulate(network, undirec_adjac_matrix, network->n_nodes, elim_ord, set_clique_ptr_container);

  // Theoretically, this step is not necessary.
  ElimRedundantCliques();

  FormJunctionTree(set_clique_ptr_container);
  AssignPotentials();
}



int** JunctionTree::ConvertDAGNetworkToAdjacencyMatrix(Network *net) {
  int num_nodes = net->n_nodes;
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
  vector<int> result;
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
  if (elim_ord.size()==1) {return;}
  set<int> set_neighbours;
  set<Node*> set_node_ptrs_to_form_a_clique;
  int first_node_in_elim_ord = elim_ord.front();
  set_node_ptrs_to_form_a_clique.insert(net->GivenIndexToFindNodePointer(first_node_in_elim_ord));
  for (int j=0; j<num_nodes; ++j) {
    if (adjac_matrix[first_node_in_elim_ord][j]==1) {
      set_neighbours.insert(j);
    }
  }

  // Form a clique.
  for (auto &nei : set_neighbours) {
    for (auto &index2 : set_neighbours) {
      if (nei!=index2) {
        adjac_matrix[nei][index2] = 1;
        adjac_matrix[index2][nei] = 1;
      }
    }
    set_node_ptrs_to_form_a_clique.insert(net->GivenIndexToFindNodePointer(nei));
  }

  cliques.insert(new Clique(set_node_ptrs_to_form_a_clique));
  
  // Remove the first node in elimination ordering, which has already form a clique.
  // The node has been removed, so the edges connected to it should be removed too.
  elim_ord.erase(elim_ord.begin());
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
        common_related_node_ptrs.insert(network->GivenIndexToFindNodePointer(v));
      }

      Separator *sep = new Separator(common_related_node_ptrs);

      // Let separator know the two cliques that it connects to.
      sep->set_neighbours_ptr.insert(clique_ptr);
      sep->set_neighbours_ptr.insert(clique_ptr_2);

      all_possible_seps.insert(sep);

    }
  }

  // Second, use Prim's algorithm to form a maximum spanning tree.
  set<Clique*> tree_so_far;
  tree_so_far.insert(*cliques.begin()); // randomly insert a clique in tree
  while (tree_so_far.size()<cliques.size()) {
    Separator* max_weight_sep = nullptr;
    for (auto &sep_ptr : all_possible_seps) {
      auto iter = sep_ptr->set_neighbours_ptr.begin();
      Clique *clq1 = *iter, *clq2 = *(++iter);

      if (tree_so_far.find(clq1)!=tree_so_far.end()
          &&
          tree_so_far.find(clq2)==tree_so_far.end()
          ||
          tree_so_far.find(clq1)==tree_so_far.end()
          &&
          tree_so_far.find(clq2)!=tree_so_far.end()) {
        if (max_weight_sep==nullptr || max_weight_sep->weight < sep_ptr->weight) {
          max_weight_sep = sep_ptr;
        }
      }
    }

    set_separator_ptr_container.insert(max_weight_sep);
    auto iter = max_weight_sep->set_neighbours_ptr.begin();
    Clique *clq1 = *iter, *clq2 = *(++iter);
    tree_so_far.insert(clq1);
    tree_so_far.insert(clq2);
  }

  // Now let the cliques to know the separators that they connect to.
  for (auto &sep_ptr : set_separator_ptr_container) {
    auto iter = sep_ptr->set_neighbours_ptr.begin();
    Clique *clq1 = *iter, *clq2 = *(++iter);
    clq1->set_neighbours_ptr.insert(sep_ptr);
    clq2->set_neighbours_ptr.insert(sep_ptr);
  }
}

void JunctionTree::AssignPotentials() {

  // When creating the cliques, the potentials have been initialized to 1.
  // Now we need to assign the original joint probabilities to these cliques.

  // First, convert the probabilities of nodes to factors, which make the "multiply" operation easier.
  vector<Factor> factors; // Can not use std::set, because Factor does not have definition on operator "<".
  for (auto &node_ptr : network->set_node_ptr_container) {
    Factor f;
    f.ConstructFactor(node_ptr);
    factors.push_back(f);
  }

  // Second, assign these factors to some appropriate cliques.
  // Each factor should be use only once.
  for (auto &f : factors) {
    for (auto &clique_ptr : set_clique_ptr_container) {

      if (f.related_variables.empty() || clique_ptr->related_variables.empty()) {break;}

      set<int> diff;
      set_difference(f.related_variables.begin(), f.related_variables.end(),
                     clique_ptr->related_variables.begin(), clique_ptr->related_variables.end(),
                     inserter(diff, diff.begin()));
      // If diff.empty(), that means that the set of related variables of the clique is a superset of the factor's.
      if (diff.empty()) {
        clique_ptr->MultiplyWithFactorSumOverExternalVars(f);
        break;  // Ensure that each factor of the original joint probability is used only once.
      }
    }
  }
}

void JunctionTree::LoadEvidence(Combination E) {
  if (E.empty()) {return;}
  for (auto &clique_ptr : set_clique_ptr_container) {  // For each clique
    for (auto &e : E) {  // For each node's observation in E
      if (clique_ptr->related_variables.find(e.first)!=clique_ptr->related_variables.end()) {  // If this clique is related to this node
        for (auto &comb : clique_ptr->set_combinations) {  // Update each row of map_potentials
          if (comb.find(e)==comb.end()) {
            clique_ptr->map_potentials[comb] = 0;
          }
        }
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

void JunctionTree::PrintAllCliquesPotentials() {
  cout << "Cliques" << '\n';
  for (auto &c : set_clique_ptr_container) {
    c->PrintPotentials();
  }
  cout << "=======================================================================" << endl;
}

void JunctionTree::PrintAllSeparatorsPotentials() {
  cout << "Separators" << '\n';
  for (auto &s : set_separator_ptr_container) {
    s->PrintPotentials();
  }
  cout << "=======================================================================" << endl;
}

Factor JunctionTree::InferenceForVarIndexsReturnPossib(set<int> &indexes) {

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
  f.SetMembers(selected_clique->related_variables, selected_clique->set_combinations, selected_clique->map_potentials);
  for (auto &index : diff) {
    f = f.SumOverVar(index);
  }
  f.Normalize();
  return f;


  // todo: implement the case where the query vairables appear in different cliques.
}