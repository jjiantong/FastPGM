//
// Created by LinjianLi on 2019/2/16.
//

#include "JunctionTree.h"

JunctionTree::JunctionTree(Network *net)
  : JunctionTree(net, "min-nei") {}

JunctionTree::JunctionTree(Network *net, string elim_ord_strategy)
  : JunctionTree(net, elim_ord_strategy, vector<int>()) {}

JunctionTree::JunctionTree(Network *net, string elim_ord_strategy, vector<int> custom_elim_ord) {

    cout << "begin construction function of JunctionTree..." << endl;
  struct timeval start, end;
  double diff;
  gettimeofday(&start,NULL);

  network = net;

  int **direc_adjac_matrix = network->ConvertDAGNetworkToAdjacencyMatrix();
//  cout << "finish ConvertDAGNetworkToAdjacencyMatrix" << endl;
//    for (int i = 0; i < network->num_nodes; ++i) {
//        for (int j = 0; j < network->num_nodes; ++j) {
//            cout << direc_adjac_matrix[i][j] << " ";
//        }
//        cout << endl;
//    }

  Moralize(direc_adjac_matrix, network->num_nodes);
  int **moral_graph_adjac_matrix = direc_adjac_matrix;
//  cout << "finish Moralize" << endl;
//    for (int i = 0; i < network->num_nodes; ++i) {
//        for (int j = 0; j < network->num_nodes; ++j) {
//            cout << moral_graph_adjac_matrix[i][j] << " ";
//        }
//        cout << endl;
//    }

//  cout << "elimination order = " << elim_ord_strategy << endl;
  // There are different ways of determining elimination ordering.
  if (elim_ord_strategy == "min-nei") {
    elimination_ordering = MinNeighbourElimOrd(moral_graph_adjac_matrix, network->num_nodes);
  }
  else if (elim_ord_strategy == "rev-topo") {
    elimination_ordering = network->GetReverseTopoOrd();
  }
  else if (elim_ord_strategy == "custom") {
    if (custom_elim_ord.size() != net->num_nodes) {
      fprintf(stderr, "Error in function [%s]\nSize of custom elimination"
                      "ordering and size of the network is not the same!", __FUNCTION__);
      exit(1);
    }
    elimination_ordering = custom_elim_ord;
  }
  else {
    fprintf(stderr, "The elimination ordering strategy should be one of the following:\n"
                    "{ min-nei, rev-topo, custom }.");
    exit(1);
  }
//    cout << "finish order, order: ";
//    for (int i = 0; i < network->num_nodes; ++i) {
//        cout << elimination_ordering[i] << " ";
//    }
//    cout << endl;

  //construct a clique for each node in the network
  Triangulate(network, moral_graph_adjac_matrix, network->num_nodes, elimination_ordering, set_clique_ptr_container);
//  cout << "finish Triangulate, number of cliques = " << set_clique_ptr_container.size() << endl;

  //construct map from main variable to a clique
//  GenMapElimVarToClique();
//  cout << "finish GenMapElimVarToClique" << endl;

  FormJunctionTree(set_clique_ptr_container);//for discrete nodes
//  FormListShapeJunctionTree(set_clique_ptr_container);//for continuous nodes
    cout << "finish FormJunctionTree, number of cliques = " << set_clique_ptr_container.size()
         << ", number of separators = " << set_separator_ptr_container.size() << endl;

  //assign id to each clique
  NumberTheCliquesAndSeparators();
//  cout << "finish NumberTheCliquesAndSeparators" << endl;

  AssignPotentials();
//  cout << "finish AssignPotentials" << endl;

//    cout << "cliques: " << endl;
//    for (auto &c : set_clique_ptr_container) {
//        cout << c->clique_id << ": ";
//        for (auto &v : c->related_variables) {
//            cout << v << " ";
//        }
//        cout << endl;
//        // set<DiscreteConfig> set_disc_configs; DiscreteConfig: set< pair<int, int> >
//        // map<DiscreteConfig, double> map_potentials
//        for (auto &config: c->set_disc_configs) {
//            cout << "config: ";
//            for (auto &varval: config) {
//                cout << varval.first << "=" << varval.second << " ";
//            }
//            cout << ": " << c->map_potentials[config] << endl;
//        }
//    }
//    cout << "separators: " << endl;
//    for (auto &s : set_separator_ptr_container) {
//        cout << s->clique_id << ": ";
//        for (auto &v : s->related_variables) {
//            cout << v << " ";
//        }
//        cout << endl;
//        // set<DiscreteConfig> set_disc_configs; DiscreteConfig: set< pair<int, int> >
//        // map<DiscreteConfig, double> map_potentials
//        for (auto &config: s->set_disc_configs) {
//            cout << "config: ";
//            for (auto &varval: config) {
//                cout << varval.first << "=" << varval.second << " ";
//            }
//            cout << ": " << s->map_potentials[config] << endl;
//        }
//    }

  BackUpJunctionTree();
//  cout << "finish BackUpJunctionTree" << endl;

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


/**
 * @brief: moralization -> moral graph
 * 1. connect all the parents of each node (connect all the v-structure)
 * 2. directed graph -> undirected graph
 */
void JunctionTree::Moralize(int **direc_adjac_matrix, int &num_nodes) { //checked
//  // TODO: can we just get parents of each node and marry them?
//
//  // Find the parents that have common child(ren).
//  // if node x has more than 1 parent, ex., i, j and k,
//  // then we will find every one of them, and all of the
//  // (i, j) (i, k) (j, i) (j, k) (k, i) (k, j) will be inserted into "to_marry"
//  set<pair<int, int>> to_marry;
////  #pragma omp parallel for collapse(2)
//  for (int i = 0; i < num_nodes; ++i) {
//    for (int j = 0; j < num_nodes; ++j) {
//      if (i == j) {
//        continue;
//      }
//      if (direc_adjac_matrix[i][j] == 1) {
//        // "i" is a parent of "j"
//        // The next step is to find other parents, "k", of "j"
//        for (int k = 0; k < num_nodes; ++k) {
//          // TODO: what if k = i?
//          if (direc_adjac_matrix[k][j] == 1) {
//            to_marry.insert(pair<int, int>(i, k));
//          }
//        }
//      }
//    }
//  }
//
//  // Making the adjacency matrix undirected.
////  #pragma omp parallel for collapse(2)
//  for (int i = 0; i < num_nodes; ++i) {
//    for (int j = 0; j < num_nodes; ++j) {
//      if (i==j) {
//        continue;
//      }
//      // TODO: repeated
//      if (direc_adjac_matrix[i][j] == 1 || direc_adjac_matrix[j][i] == 1) {
//        direc_adjac_matrix[i][j] = 1;
//        direc_adjac_matrix[j][i] = 1;
//      }
//    }
//  }
//
//  // Marrying parents.
//  for (const auto &p : to_marry) {
//    direc_adjac_matrix[p.first][p.second] = 1;
//  }

    // TODO: new (right) version
    set<pair<int, int>> to_marry;

    for (int i = 0; i < num_nodes; ++i) {
        vector<int> parents;
        // get parents of each node i
        for (int j = 0; j < num_nodes; ++j) {
            if (i == j) {
                continue;
            }
            if (direc_adjac_matrix[j][i] == 1) {
                // j is a parent of i
                parents.push_back(j);
            }
        }

        if (parents.size() < 2) {
            // if num of parents is 0 or 1, no need to marry
            continue;
        }
        ChoiceGenerator *cg = new ChoiceGenerator(parents.size(), 2);
        vector<int> choice = cg->Next();
        while (!choice.empty()) {
            to_marry.insert(pair<int, int>(parents.at(choice.at(0)), parents.at(choice.at(1))));
            choice = cg->Next();
        }
        delete cg;
        cg = nullptr;
    }

    // Making the adjacency matrix undirected.
    for (int i = 0; i < num_nodes; ++i) {
        for (int j = 0; j < i; ++j) {
            if (direc_adjac_matrix[i][j] == 1 || direc_adjac_matrix[j][i] == 1) {
                direc_adjac_matrix[i][j] = 1;
                direc_adjac_matrix[j][i] = 1;
            }
        }
    }

    // Marrying parents.
    for (const auto &p : to_marry) {
        direc_adjac_matrix[p.first][p.second] = 1;
        direc_adjac_matrix[p.second][p.first] = 1;
    }
}

///**
// * @brief: construct a map for ease of look up the clique
// */
//void JunctionTree::GenMapElimVarToClique() { //checked
//  for (const auto &c : set_clique_ptr_container) {
//    map_elim_var_to_clique[c->elimination_variable_index] = c;
//  }
//}

/**
 * @brief: Generate the elimination order by the number of neighbours.
 * The node with the minimum number of neighbours is eliminated first.
 */
vector<int> JunctionTree::MinNeighbourElimOrd(int **adjac_matrix, int &num_nodes) { //checked
  //TODO: double-check correctness
  vector< pair<int,int> > to_be_sorted;

  // get the number of neighbors for each node
  for (int i = 0; i < num_nodes; ++i) { // for each node
    pair<int,int> p; // key: node id; value: number of neighbors
    p.first = i;
    p.second = 0;
    for (int j = 0; j < num_nodes; ++j) {
      if (adjac_matrix[i][j]==1) { // j is i's neighbor
        ++p.second;
      }
    }
    to_be_sorted.push_back(p);
  }

  // sort by the number of neighbors from smallest to largest
  sort(to_be_sorted.begin(), to_be_sorted.end(), [](pair<int,int> a, pair<int,int> b){return a.second < b.second;});  // Using lambda expression.
  vector< pair<int,int> > &sorted = to_be_sorted;

  vector<int> result;
  result.reserve(sorted.size());
  for (auto &p : sorted) {
    result.push_back(p.first);
  }
  return result;
}


/**
 * @brief: triangulation -> induced graph
 * definition: no loops of length > 3 without a "bridge"/"chord"
 * one way to construct a induced graph: select a node based on the "elim_ord", connect all its neighbors, and remove this node;
 * the selected node should try to minimize the added edges, so the "elim_ord" may according to the number or neighbors
 * (if we connect two nodes if they appeared in the same factor in a run of the VE algorithm, we can exactly get a induced graph)
 * in this function, the purpose is to generate a clique while constructing the induced graph
 */
void JunctionTree::Triangulate(Network *net,
                               int **adjac_matrix,
                               int &num_nodes,
                               vector<int> elim_ord,
                               set<Clique*> &cliques) { //checked
  if (elim_ord.size() == 0) {
    return;
  }

    vector<int> vec_neighbors;
    set<Node*> set_node_ptrs_to_form_a_clique;
    int first_node_in_elim_ord = elim_ord.front();

    // insert the first node in the elimination order into "set_node_ptrs_to_form_a_clique"
    set_node_ptrs_to_form_a_clique.insert(net->FindNodePtrByIndex(first_node_in_elim_ord));
    // insert all its neighbors into "vec_neighbors"
    for (int j = 0; j < num_nodes; ++j) {
        if (adjac_matrix[first_node_in_elim_ord][j] == 1) {
            vec_neighbors.push_back(j);
        }
    }

    // Form a clique that contains
    for (int neighbor = 0; neighbor < vec_neighbors.size(); ++neighbor) {
        for (int neighbor2 = neighbor + 1; neighbor2 < vec_neighbors.size(); ++neighbor2) {
            adjac_matrix[vec_neighbors.at(neighbor)][vec_neighbors.at(neighbor2)] = 1;
            adjac_matrix[vec_neighbors.at(neighbor2)][vec_neighbors.at(neighbor)] = 1;
        }
        set_node_ptrs_to_form_a_clique.insert(net->FindNodePtrByIndex(vec_neighbors.at(neighbor)));
    }

    // before adding a clique, we need to check whether the clique is redundant
    // if a clique is fully contained by another (existing/previous) clique, then the clique is no need to be inserted.
    Clique* clique = new Clique(set_node_ptrs_to_form_a_clique, first_node_in_elim_ord);
    bool to_be_inserted = true;
    for (auto &ptr_clq : cliques) {
        set<int> intersection;
        set_intersection(clique->related_variables.begin(), clique->related_variables.end(),
                         ptr_clq->related_variables.begin(), ptr_clq->related_variables.end(),
                         std::inserter(intersection, intersection.begin()));
        if (intersection == clique->related_variables) {
            to_be_inserted = false;
            break;
        }
    }

    if (to_be_inserted) {
        cliques.insert(clique);
//        cout << "clique " << cliques.size() << ": ";
//        for (auto &node: set_node_ptrs_to_form_a_clique) {
//            cout << node->GetNodeIndex() << ", ";
//        }
//        cout << endl;
    } else {
        delete clique;
    }

    // Remove the first node in elimination ordering, which has already form a clique.
    elim_ord.erase(elim_ord.begin());
    // The node has been removed, so the edges connected to it should be removed too.
    for (int neighbor = 0; neighbor < vec_neighbors.size(); ++neighbor) {
        adjac_matrix[first_node_in_elim_ord][vec_neighbors.at(neighbor)] = 0;
        adjac_matrix[vec_neighbors.at(neighbor)][first_node_in_elim_ord] = 0;
    }

    Triangulate(net, adjac_matrix, num_nodes, elim_ord, cliques);
}

///**
// * @brief: the Junction Tree here looks like a linked list.
// * This is used for continuous variables, based on a tutorial or some unpublished work.
// */
//void JunctionTree::FormListShapeJunctionTree(set<Clique*> &cliques) { //checked
//  //TODO: double-check correctness (continuous)
//  // This method is described in
//  // [Local Propagation in Conditional Gaussian Bayesian Networks (Cowell, 2005)]
//  // section 3.2.
//  // The last sentence.
//
//  // todo: test correctness
//  for (int i=0; i<elimination_ordering.size()-1; ++i) {
//    Clique *this_clq = map_elim_var_to_clique[elimination_ordering.at(i)];
//    Clique *next_clq = nullptr;
//    for (int j=i+1; j<elimination_ordering.size(); ++j) {
//      if (this_clq->related_variables.find(elimination_ordering.at(j))!=this_clq->related_variables.end()) {
//        next_clq = map_elim_var_to_clique[elimination_ordering.at(j)];
//        break;
//      }
//    }
//    set<int> common_related_variables;
//    set_intersection(this_clq->related_variables.begin(),this_clq->related_variables.end(),
//                     next_clq->related_variables.begin(),next_clq->related_variables.end(),
//                     std::inserter(common_related_variables,common_related_variables.begin()));
//
//    // If they have no common variables, then they will not be connected by separator.
//    if (common_related_variables.empty()) {continue;}
//
//    set<Node*> common_related_node_ptrs;
//    for (auto &v : common_related_variables) {
//      common_related_node_ptrs.insert(network->FindNodePtrByIndex(v));
//    }
//
//    Separator *sep = new Separator(common_related_node_ptrs);
//
//    // Let separator know the two cliques that it connects to.
//    sep->set_neighbours_ptr.insert(this_clq);
//    sep->set_neighbours_ptr.insert(next_clq);
//
//    set_separator_ptr_container.insert(sep);
//  }
//
//  // Now let the cliques to know the separators that they connect to.
//  for (auto &sep_ptr : set_separator_ptr_container) {
//    auto iter = sep_ptr->set_neighbours_ptr.begin();
//    Clique *clq1 = *iter, *clq2 = *(++iter);
//    clq1->set_neighbours_ptr.insert(sep_ptr);
//    clq2->set_neighbours_ptr.insert(sep_ptr);
//  }
//}

/**
 * @brief: construct a tree where each node is a clique and each edge is a separator.
 * use Prim algorithm; the weights of edges is represented by the weights of the separators
 */
void JunctionTree::FormJunctionTree(set<Clique*> &cliques) {
//TODO: double-check correctness
  // First, generate all possible separators.
  set<pair<Clique*,Clique*>> mark;
  set<Separator*> all_possible_seps;
  // TODO: can we just traverse i from 0 to cliques.size and j from i+1 to cliques.size?
  // TODO: then we dont need to use "mark" or frequently find pair from "mark"
  for (auto &clique_ptr : cliques) {
    for (auto &clique_ptr_2 : cliques) {

      if (clique_ptr == clique_ptr_2) {
        continue; // The same cliques do not need a separator
      } else if (mark.find(pair<Clique*,Clique*>(clique_ptr,clique_ptr_2)) != mark.end()) {
        continue; // The separator of this pair of cliques has been generated
      }

      // Mark this pair.
      mark.insert(pair<Clique*,Clique*>(clique_ptr,clique_ptr_2));
      mark.insert(pair<Clique*,Clique*>(clique_ptr_2,clique_ptr));

      set<int> common_related_variables;
      set_intersection(clique_ptr->related_variables.begin(),clique_ptr->related_variables.end(),
                       clique_ptr_2->related_variables.begin(),clique_ptr_2->related_variables.end(),
                       std::inserter(common_related_variables,common_related_variables.begin()));

      // If they have no common variables, then they will not be connected by separator.
      if (common_related_variables.empty()) {
          continue;
      }

      set<Node*> common_related_node_ptrs;
      for (auto &v : common_related_variables) {
        common_related_node_ptrs.insert(network->FindNodePtrByIndex(v));
      }

        // TODO: all separators are generated but part of them are used,
        //  where and how to delete them? -- memory leakage?
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
  tree_so_far.insert(*cliques.begin()); // randomly insert a clique in tree, as the start of the Prim algorithm
//    cout << "insert clique ";
//    for (auto &v: (*cliques.begin())->related_variables) {
//        cout << v << " ";
//    }
//    cout << endl;

  while (tree_so_far.size()<cliques.size()) {
    Separator* max_weight_sep = nullptr;
    for (auto &sep_ptr : all_possible_seps) { // traverse all separators
      // find the two cliques the separator connected
      auto iter = sep_ptr->set_neighbours_ptr.begin();
      Clique *clq1 = *iter, *clq2 = *(++iter);

      // if one of the cliques is in the "tree_so_far", and the other is not
      if ((tree_so_far.find(clq1) != tree_so_far.end() && tree_so_far.find(clq2) == tree_so_far.end())
          ||
          (tree_so_far.find(clq1) == tree_so_far.end() && tree_so_far.find(clq2) != tree_so_far.end())) {
        // And if the weight of this separator is the largest.
        if (max_weight_sep==nullptr || max_weight_sep->weight < sep_ptr->weight) {
          max_weight_sep = sep_ptr;
        }
      }
    }

//    max_weight_sep->clique_id = set_separator_ptr_container.size();
    set_separator_ptr_container.insert(max_weight_sep);
//      cout << "insert separator ";
//      for (auto &v: max_weight_sep->related_variables) {
//          cout << v << " ";
//      }
//      cout << "weight = " << max_weight_sep->weight << endl;

    auto iter = max_weight_sep->set_neighbours_ptr.begin();
    Clique *clq1 = *iter, *clq2 = *(++iter);
    tree_so_far.insert(clq1);
    tree_so_far.insert(clq2);

//      cout << "insert clique ";
//      for (auto &v: clq1->related_variables) {
//          cout << v << " ";
//      }
//      cout << endl;
//      cout << "insert clique ";
//      for (auto &v: clq2->related_variables) {
//          cout << v << " ";
//      }
//      cout << endl;

  }   // end of: while. Until all cliques are in "tree_so_far"

  // Now let the cliques to know the separators that they connect to.
  for (auto &sep_ptr : set_separator_ptr_container) {
    auto iter = sep_ptr->set_neighbours_ptr.begin();
    Clique *clq1 = *iter, *clq2 = *(++iter);
    clq1->set_neighbours_ptr.insert(sep_ptr);
    clq2->set_neighbours_ptr.insert(sep_ptr);
  }
}

/**
 * @brief: assign an id to each clique and separator
 */
void JunctionTree::NumberTheCliquesAndSeparators() {//checked
  int i = 0;
  for (auto c : set_clique_ptr_container) {
    c->clique_id = i++;
  }
  int j = 0;
  for (auto s : set_separator_ptr_container) {
    s->clique_id = j++;
  }
}

/**
 * @brief: each clique has a potential;
 * the potentials of continuous and discrete cliques are computed differently
 */
void JunctionTree::AssignPotentials() { //checked
  // todo: test the correctness of the continuous part (discrete part works correctly)

  // For purely discrete cliques, the potentials have been initialized to 1 on creation,
  // so we need to assign the probabilities of the network nodes to these cliques.
  // For continuous cliques, the lp_potentials and post_bags are set to empty list,
  // so we need to assign the CG regressions to these cliques. The method is described in
  // [Local Propagation in Conditional Gaussian Bayesian Networks (Cowell, 2005)], section 5.2.

  // 1, extract the information of nodes.
  // generate all the factors of the network, which are the probabilities of discrete nodes given their parents
  // Extract the CG regressions of continuous nodes.
  vector<Factor> factors; // Can not use std::set, because Factor does not have definition on operator "<".
//  vector<CGRegression> cgrs;

  for (auto &id_node_ptr : network->map_idx_node_ptr) { // for each node of the network
    auto node_ptr = id_node_ptr.second;
    if (node_ptr->is_discrete) {
      // add the factor that consists of this node and its parents
      factors.push_back(Factor(dynamic_cast<DiscreteNode*>(node_ptr), this->network)); // each node has one factor
    }
//    else {  // If the node is continuous.
//      cgrs.push_back(CGRegression(node_ptr, network->GetParentPtrsOfNode(node_ptr->GetNodeIndex())));
//    }
  }

  // 2, (part of) BP algorithm
  //    2.1 assign each factors and CG regressions to a clique
  //    each factor and CG regression should be use only once

  // For potentials from discrete nodes, they should be assigned to purely discrete cliques.
  for (auto &f : factors) { // for each factor of the network
    for (auto &clique_ptr : set_clique_ptr_container) { // for each clique of the graph

      if (f.related_variables.empty() || clique_ptr->related_variables.empty()) {
        break;
      }
      if (!clique_ptr->pure_discrete) {
        continue;
      }

      // get the variables that in the factor but not in the clique
      set<int> diff;
      set_difference(f.related_variables.begin(), f.related_variables.end(),
                     clique_ptr->related_variables.begin(), clique_ptr->related_variables.end(),
                     inserter(diff, diff.begin()));
      // If "diff" is empty, i.e., all the variables in the factor are in the clique,
      // which means that the clique can accommodate the scope of the factor - satisfy the family preservation property of cluster graph
      // (clique tree is a special case of cluster graph)
      // so we can assign this factor to this clique
      if (diff.empty()) {
        // 2.2 construct the initial potential of this clique,
        // which is the product of factors that assigned to it
//          cout << "assign factor ";
//          for (auto &v: f.related_variables) {
//              cout << v << " ";
//          }
//          cout << "to clique ";
//          for (auto &v: clique_ptr->related_variables) {
//              cout << v << " ";
//          }
//          cout << endl;
        clique_ptr->MultiplyWithFactorSumOverExternalVars(f);
        break;  // Ensure that each factor is used only once.
      }
    }
  }
//  for (auto &cgr : cgrs) {
//    for (auto &clique_ptr : set_clique_ptr_container) {
//
//      if (clique_ptr->pure_discrete) { continue; }
//
//      set<int> cgr_related_vars = cgr.set_all_tail_index;
//      cgr_related_vars.insert(cgr.head_var_index);
//      set<int> diff;
//      set_difference(cgr_related_vars.begin(), cgr_related_vars.end(),
//                     clique_ptr->related_variables.begin(), clique_ptr->related_variables.end(),
//                     inserter(diff, diff.begin()));
//      // If diff.empty(), that means that the set of related variables of the clique is a superset of the CG regression's.
//      if (diff.empty()) {
//        if (clique_ptr->elimination_variable_index == cgr.head_var_index) {
//          clique_ptr->lp_potential.push_back(cgr);
//        } else {
//          clique_ptr->post_bag.push_back(cgr);
//        }
//        break;  // Ensure that each CG regression is used only once.
//      }
//      // todo: fix it
//      //   There is a problem when the discrete variables of cgr and clique_ptr is not the same.
//      //   I don't know what will happen.
//    }
//  }
}

/**
 * The inference process will modify the junction tree itself.
 * So, we need to backup the tree and restore it after an inference.
 * Otherwise, we need to re-construct the tree each time we want to make inference.
 */
void JunctionTree::BackUpJunctionTree() {//checked
  for (const auto &c : set_clique_ptr_container) {
    map_cliques_backup[c] = *c;
  }
  for (const auto &s : set_separator_ptr_container) {
    map_separators_backup[s] = *s;
  }
}

/**
 * The inference process will modify the junction tree itself.
 * So, we need to backup the tree and restore it after an inference.
 * Otherwise, we need to re-construct the tree each time we want to make inference.
 */
void JunctionTree::ResetJunctionTree() {
  for (auto &c : set_clique_ptr_container) {
    *c = map_cliques_backup[c];
  }
  for (auto &s : set_separator_ptr_container) {
    *s = map_separators_backup[s];
  }
}

void JunctionTree::LoadEvidenceAndMessagePassingUpdateJT(const DiscreteConfig &E) {
  LoadDiscreteEvidence(E);
  MessagePassingUpdateJT();
}

/**
 * @brief: when inferring, an evidence is given. The evidence needs to be loaded and propagate in the network.
 */
void JunctionTree::LoadDiscreteEvidence(const DiscreteConfig &E) {
  if (E.empty()) {
      return;
  }
  for (auto &e : E) {  // For each node's observation in E
//      // DiscreteConfig: set< pair<int, int> >
//      cout << "evidence " << e.first << "=" << e.second << endl;

    for (auto &clique_ptr : set_clique_ptr_container) {  // For each clique

//        cout << "  check clique ";
//        for (auto &var: clique_ptr->related_variables) {
//            cout << var << " ";
//        }
//        cout << ": ";

      // If this clique is related to this node
      if (clique_ptr->related_variables.find(e.first) != clique_ptr->related_variables.end()) {

//          cout << "yes" << endl;

        for (auto &comb : clique_ptr->set_disc_configs) {  // Update each row of map_potentials

//            cout << "    check config "; // set<DiscreteConfig> set_disc_configs
//            for (auto &c: comb) {
//                cout << c.first << "=" << c.second << " ";
//            }
//            cout << ": ";

          if (comb.find(e) == comb.end()) {

//              cout << "conflict, set 0" << endl;

            clique_ptr->map_potentials[comb] = 0;
          } else {

//              cout << "no conflict, no change" << endl;

          }
        }
//        // I do not know if the "break" is optional.
//        // Entering the evidence to one clique that contains it,
//        // or to all cliques that contain it.
//        // Are the results after message passing process both correct???
//        // todo: figure it out
//        break;
      } else {
//          cout << "no" << endl;
      }
    }
  }
}

///**
// * @brief: when inferring, an evidence is given. The evidence needs to be loaded and propagate in the network.
// */
//void JunctionTree::LoadDiscreteEvidence(const DiscreteConfig &E) {
//  if (E.empty()) {
//    return;
//  }
//    // TODO: double-check: only load evidence on one clique? or all cliques containing evidence?
//  for (auto &e : E) {  // For each node's observation in E
//    if (network->FindNodePtrByIndex(e.first)->is_discrete) {
//      Clique *clique_ptr = map_elim_var_to_clique[e.first];
//      for (auto &comb : clique_ptr->set_disc_configs) {  // Update each row of map_potentials
//        if (comb.find(e) == comb.end()) {
//          clique_ptr->map_potentials[comb] = 0;//conflict with the evidence; set the potential to 0.
//        }
//      }
//    }
//    else {
//      fprintf(stderr, "Error in Function [%s]", __FUNCTION__);
//      exit(1);
//    }
//  }
//}

/**
 * 
 * Message passing is just COLLECT and DISTRIBUTE (these two words is used by paper and text book).
 * The order between COLLECT and DISTRIBUTE does not matter, but they must not interleave.
 * After message passing, any clique (junction tree node) contains the right distribution of the related variables.
 */
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

/**
 * @brief: compute the marginal distribution for a query variable
 **/
Factor JunctionTree::BeliefPropagationCalcuDiscreteVarMarginal(int query_index) {

  // The input is a set of query_indexes of variables.
  // The output is a factor representing the joint marginal of these variables.
  // TODO: here only support one query variable

  int min_potential_size = INT32_MAX;
  Clique *selected_clique = nullptr;

  // The case where the query variables are all appear in one clique.
  // Find the clique that contains this variable,
  // whose size of potentials table is the smallest,
  // which can reduce the number of sum operation.
  // TODO: find from separator
  for (auto &c : set_clique_ptr_container) {
    if (!c->pure_discrete) {
        continue;
    }
    if (c->related_variables.find(query_index) == c->related_variables.end()) { // cannot find the query variable
        continue;
    }
    if (c->map_potentials.size() >= min_potential_size) { // TODO: can we use clique_size?
        continue;
    }
    min_potential_size = c->map_potentials.size(); // TODO: clique size
    selected_clique = c;
  }

  if (selected_clique == nullptr) {
    fprintf(stderr, "Error in function [%s]\n"
                    "Variable [%d] does not appear in any clique!", __FUNCTION__, query_index);
    exit(1);
  }

  set<int> other_vars = selected_clique->related_variables;
  other_vars.erase(query_index);

  Factor f(selected_clique->related_variables, selected_clique->set_disc_configs, selected_clique->map_potentials);


//    cout << "factor: related vars = ";
//    for (auto &var: f.related_variables) {
//        cout << var << " ";
//    }
//    cout << endl;
//    // set<DiscreteConfig> set_disc_config; DiscreteConfig: set< pair<int, int> >
//    // map<DiscreteConfig, double> map_potentials
//    for (auto &config: f.set_disc_config) {
//        cout << "config: ";
//        for (auto &varval: config) {
//            cout << varval.first << "=" << varval.second << " ";
//        }
//        cout << ": " << f.map_potentials[config] << endl;
//    }


  for (auto &index : other_vars) {
    f = f.SumOverVar(index);
  }


//    cout << "after sum over: factor: related vars = ";
//    for (auto &var: f.related_variables) {
//        cout << var << " ";
//    }
//    cout << endl;
//    // set<DiscreteConfig> set_disc_config; DiscreteConfig: set< pair<int, int> >
//    // map<DiscreteConfig, double> map_potentials
//    for (auto &config: f.set_disc_config) {
//        cout << "config: ";
//        for (auto &varval: config) {
//            cout << varval.first << "=" << varval.second << " ";
//        }
//        cout << ": " << f.map_potentials[config] << endl;
//    }


  f.Normalize(); // todo: no need
  return f;

}

/**
 * @brief: predict the lable for a given variable.
 */
int JunctionTree::InferenceUsingBeliefPropagation(int &query_index) {
  Factor f = BeliefPropagationCalcuDiscreteVarMarginal(query_index);
  double max_prob = 0;
  DiscreteConfig comb_predict; // set< pair<int, int> >
  for (auto &comb : f.set_disc_config) {
    if (f.map_potentials[comb] > max_prob) {
      max_prob = f.map_potentials[comb];
      comb_predict = comb;
    }
  }
  int label_predict = comb_predict.begin()->second; // TODO: use "query_index"
  return label_predict;
}

/**
 * @brief: test the Junction Tree given a data set
 */
double JunctionTree::EvaluateJTAccuracy(int class_var, Dataset *dts) {

  cout << "==================================================" << '\n'
       << "Begin testing the trained network." << endl;

  struct timeval start, end;
  double diff;
  gettimeofday(&start,NULL);

    struct timeval start1, end1, start2, end2, start3, end3;
    double diff1, diff2, diff3;

  int num_of_correct = 0,
      num_of_wrong = 0,
      m = dts->num_instance,
      m20 = m / 20,
      progress = 0;

//  vector<int> truths, predictions;
//  truths.reserve(m);
//  predictions.reserve(m);

    cout << "Progress indicator: ";

  // If I use OpenMP to parallelize,
  // process may exit with code 137,
  // which means the memory consumption is too large.
  // I don't know how to solve yet.
//  #pragma omp parallel for
  for (int i = 1; i < m; ++i) {  // For each sample in test set

//    #pragma omp critical
    { ++progress; }
//    string progress_detail = to_string(progress) + '/' + to_string(m);
//    fprintf(stdout, "%s\n", progress_detail.c_str());
//    fflush(stdout);

    if (progress % m20 == 0) {
      string progress_percentage = to_string((double)progress/m * 100) + "%...\n";
      fprintf(stdout, "Progress: %s\n", progress_percentage.c_str());
      double acc_so_far = num_of_correct / (double)(num_of_correct+num_of_wrong);
      fprintf(stdout, "Accuracy so far: %f\n", acc_so_far);
      fflush(stdout);
    }

    // For now, only support complete data.
    int e_num = network->num_nodes - 1,
        *e_index = new int[e_num],
        *e_value = new int[e_num];

    for (int j = 0; j < network->num_nodes; ++j) {
      if (j == dts->class_var_index) {
          continue;
      }
      e_index[j < dts->class_var_index ? j : j - 1] = j;
      e_value[j < dts->class_var_index ? j : j - 1] = dts->dataset_all_vars[i][j];
    }

    DiscreteConfig E = ArrayToDiscreteConfig(e_index, e_value, e_num);

    delete[] e_index;
    delete[] e_value;


//      cout << "testing sample " << i << endl << "before load evidences: " << endl << "cliques: " << endl;
//      for (auto &c : set_clique_ptr_container) {
//          cout << c->clique_id << ": ";
//          for (auto &v : c->related_variables) {
//              cout << v << " ";
//          }
//          cout << endl;
//          // set<DiscreteConfig> set_disc_configs; DiscreteConfig: set< pair<int, int> >
//          // map<DiscreteConfig, double> map_potentials
//          for (auto &config: c->set_disc_configs) {
//              cout << "config: ";
//              for (auto &varval: config) {
//                  cout << varval.first << "=" << varval.second << " ";
//              }
//              cout << ": " << c->map_potentials[config] << endl;
//          }
//      }


      gettimeofday(&start1,NULL);
    //update a clique using the evidence
    LoadDiscreteEvidence(E);
      gettimeofday(&end1,NULL);
      diff1 += (end1.tv_sec-start1.tv_sec) + ((double)(end1.tv_usec-start1.tv_usec))/1.0E6;


//      cout << "after load evidences: " << endl << "cliques: " << endl;
//      for (auto &c : set_clique_ptr_container) {
//          cout << c->clique_id << ": ";
//          for (auto &v : c->related_variables) {
//              cout << v << " ";
//          }
//          cout << endl;
//          // set<DiscreteConfig> set_disc_configs; DiscreteConfig: set< pair<int, int> >
//          // map<DiscreteConfig, double> map_potentials
//          for (auto &config: c->set_disc_configs) {
//              cout << "config: ";
//              for (auto &varval: config) {
//                  cout << varval.first << "=" << varval.second << " ";
//              }
//              cout << ": " << c->map_potentials[config] << endl;
//          }
//      }


      gettimeofday(&start2,NULL);
    //update the whole Junction Tree
    MessagePassingUpdateJT();
      gettimeofday(&end2,NULL);
      diff2 += (end2.tv_sec-start2.tv_sec) + ((double)(end2.tv_usec-start2.tv_usec))/1.0E6;

      gettimeofday(&start3,NULL);
    int label_predict = InferenceUsingBeliefPropagation(class_var);
    ResetJunctionTree();
      gettimeofday(&end3,NULL);
      diff3 += (end3.tv_sec-start3.tv_sec) + ((double)(end3.tv_usec-start3.tv_usec))/1.0E6;

    if (label_predict == dts->dataset_all_vars[i][dts->class_var_index]) {
//        cout << "correct: " << label_predict << endl;
//      #pragma omp critical
      { ++num_of_correct; }
    } else {
//        cout << "wrong: predict = " << label_predict
//             << ", ground truth = " << dts->dataset_all_vars[i][dts->class_var_index] << endl;
//      #pragma omp critical
      { ++num_of_wrong; }
    }

  }

  gettimeofday(&end,NULL);
  diff = (end.tv_sec-start.tv_sec) + ((double)(end.tv_usec-start.tv_usec))/1.0E6;
  setlocale(LC_NUMERIC, "");
  cout << "==================================================" << '\n'
       << "The time spent to test the accuracy is " << diff << " seconds" << endl;
    cout << "load evidence: " << diff1
         << " seconds, message passing: " << diff2
         << "seconds, predict and reset: " << diff3 << "seconds" << endl;

  double accuracy = num_of_correct / (double)(num_of_correct+num_of_wrong);
  cout << '\n' << "Accuracy: " << accuracy << endl;
  return accuracy;
}