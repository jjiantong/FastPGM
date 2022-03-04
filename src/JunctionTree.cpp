#include "JunctionTree.h"

JunctionTree::JunctionTree(Network *net)
  : JunctionTree(net, "min-nei") {}

JunctionTree::JunctionTree(Network *net, string elim_ord_strategy)
  : JunctionTree(net, elim_ord_strategy, vector<int>()) {}

JunctionTree::JunctionTree(Network *net, string elim_ord_strategy, vector<int> custom_elim_ord) {

    cout << "begin construction function of JunctionTree..." << endl;

    Timer *timer = new Timer();
    // record time
    timer->Start("construct jt");

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
  Triangulate(network, moral_graph_adjac_matrix, elimination_ordering);
//  cout << "finish Triangulate, number of cliques = " << set_clique_ptr_container.size() << endl;

  //construct map from main variable to a clique
//  GenMapElimVarToClique();
//  cout << "finish GenMapElimVarToClique" << endl;

  FormJunctionTree();//for discrete nodes
//  FormListShapeJunctionTree(set_clique_ptr_container);//for continuous nodes
    cout << "finish FormJunctionTree, number of cliques = " << set_clique_ptr_container.size()
         << ", number of separators = " << set_separator_ptr_container.size() << endl;

    CliqueMerging(3);
    cout << "finish CliqueMerging, number of cliques = " << set_clique_ptr_container.size()
         << ", number of separators = " << set_separator_ptr_container.size() << endl;

  //assign id to each clique
  NumberTheCliquesAndSeparators();
//  cout << "finish NumberTheCliquesAndSeparators" << endl;

    cout << "cliques: " << endl;
    for (auto &c : set_clique_ptr_container) {
        cout << c->clique_id << ": ";
        // set<int> related_variables
        for (auto &v : c->p_table.related_variables) {
            cout << v << " ";
        }
        cout << endl;
//        // int num_variables
//        // int table_size
//        cout << "num variables = " << c->p_table.num_variables << ", table size = " << c->p_table.table_size << endl;
//        // vector<int> var_dims
//        cout << "var dims: ";
//        for (int j = 0; j < c->p_table.var_dims.size(); ++j) {
//            cout << c->p_table.var_dims[j] << " ";
//        }
//        // vector<int> cum_levels
//        cout << "cum_levels: ";
//        for (int j = 0; j < c->p_table.cum_levels.size(); ++j) {
//            cout << c->p_table.cum_levels[j] << " ";
//        }
//        // vector<double> potentials
//        cout << "table: " << endl;
//        for (int j = 0; j < c->p_table.potentials.size(); ++j) {
//            cout << c->p_table.potentials[j] << endl;
//        }
    }
    cout << "separators: " << endl;
    for (auto &s : set_separator_ptr_container) {
        cout << s->clique_id << ": ";
        // set<int> related_variables
        for (auto &v : s->p_table.related_variables) {
            cout << v << " ";
        }
        cout << endl;
//        // int num_variables
//        // int table_size
//        cout << "num variables = " << s->p_table.num_variables << ", table size = " << s->p_table.table_size << endl;
//        // vector<int> var_dims
//        cout << "var dims: ";
//        for (int j = 0; j < s->p_table.var_dims.size(); ++j) {
//            cout << s->p_table.var_dims[j] << " ";
//        }
//        // vector<int> cum_levels
//        cout << "cum_levels: ";
//        for (int j = 0; j < s->p_table.cum_levels.size(); ++j) {
//            cout << s->p_table.cum_levels[j] << " ";
//        }
//        // vector<double> potentials
//        cout << "table: " << endl;
//        for (int j = 0; j < s->p_table.potentials.size(); ++j) {
//            cout << s->p_table.potentials[j] << endl;
//        }
    }

  AssignPotentials(timer);
//  cout << "finish AssignPotentials" << endl;

//    cout << "cliques: " << endl;
//    for (auto &c : set_clique_ptr_container) {
//        cout << c->clique_id << ": ";
//        for (auto &v : c->table.related_variables) {
//            cout << v << " ";
//        }
//        cout << endl;
//        // set<DiscreteConfig> set_disc_configs; DiscreteConfig: set< pair<int, int> >
//        // map<DiscreteConfig, double> map_potentials
//        for (auto &config: c->table.set_disc_configs) {
//            cout << "config: ";
//            for (auto &varval: config) {
//                cout << varval.first << "=" << varval.second << " ";
//            }
//            cout << ": " << c->table.map_potentials[config] << endl;
//        }
//    }
//    cout << "separators: " << endl;
//    for (auto &s : set_separator_ptr_container) {
//        cout << s->clique_id << ": ";
//        for (auto &v : s->table.related_variables) {
//            cout << v << " ";
//        }
//        cout << endl;
//        // set<DiscreteConfig> set_disc_configs; DiscreteConfig: set< pair<int, int> >
//        // map<DiscreteConfig, double> map_potentials
//        for (auto &config: s->table.set_disc_configs) {
//            cout << "config: ";
//            for (auto &varval: config) {
//                cout << varval.first << "=" << varval.second << " ";
//            }
//            cout << ": " << s->table.map_potentials[config] << endl;
//        }
//    }

//    cout << "cliques: " << endl;
//    for (auto &c : set_clique_ptr_container) {
//        cout << c->clique_id << ": ";
//        // set<int> related_variables
//        for (auto &v : c->p_table.related_variables) {
//            cout << v << " ";
//        }
//        cout << endl;
//        // int num_variables
//        // int table_size
//        cout << "num variables = " << c->p_table.num_variables << ", table size = " << c->p_table.table_size << endl;
//        // vector<int> var_dims
//        cout << "var dims: ";
//        for (int j = 0; j < c->p_table.var_dims.size(); ++j) {
//            cout << c->p_table.var_dims[j] << " ";
//        }
//        // vector<int> cum_levels
//        cout << "cum_levels: ";
//        for (int j = 0; j < c->p_table.cum_levels.size(); ++j) {
//            cout << c->p_table.cum_levels[j] << " ";
//        }
//        // vector<double> potentials
//        cout << "table: " << endl;
//        for (int j = 0; j < c->p_table.potentials.size(); ++j) {
//            cout << c->p_table.potentials[j] << endl;
//        }
//    }
//    cout << "separators: " << endl;
//    for (auto &s : set_separator_ptr_container) {
//        cout << s->clique_id << ": ";
//        // set<int> related_variables
//        for (auto &v : s->p_table.related_variables) {
//            cout << v << " ";
//        }
//        cout << endl;
//        // int num_variables
//        // int table_size
//        cout << "num variables = " << s->p_table.num_variables << ", table size = " << s->p_table.table_size << endl;
//        // vector<int> var_dims
//        cout << "var dims: ";
//        for (int j = 0; j < s->p_table.var_dims.size(); ++j) {
//            cout << s->p_table.var_dims[j] << " ";
//        }
//        // vector<int> cum_levels
//        cout << "cum_levels: ";
//        for (int j = 0; j < s->p_table.cum_levels.size(); ++j) {
//            cout << s->p_table.cum_levels[j] << " ";
//        }
//        // vector<double> potentials
//        cout << "table: " << endl;
//        for (int j = 0; j < s->p_table.potentials.size(); ++j) {
//            cout << s->p_table.potentials[j] << endl;
//        }
//    }

  BackUpJunctionTree();
//  cout << "finish BackUpJunctionTree" << endl;

    cout << "==================================================";
    timer->Stop("construct jt");
    timer->Print("construct jt"); cout << endl;
    delete timer;
    timer = nullptr;

  delete[] direc_adjac_matrix;
}


//JunctionTree::JunctionTree(JunctionTree *jt) {
//  this->network = jt->network;
//
//
//  // The following block is to initialize the matrices
//  // that are used to record the connections in order to restore them.
//  // --------------------------------------------------------------------------
//  int **seps_that_cliques_connect_to = new int* [jt->set_clique_ptr_container.size()],
//      **cliques_that_seps_connect_to = new int* [jt->set_separator_ptr_container.size()];
//  for (int i=0; i<jt->set_clique_ptr_container.size(); ++i) {
//    seps_that_cliques_connect_to[i] = new int[jt->set_separator_ptr_container.size()]();
//  }
//  for (int i=0; i<jt->set_separator_ptr_container.size(); ++i) {
//    cliques_that_seps_connect_to[i] = new int[jt->set_clique_ptr_container.size()]();
//  }
//  // --------------------------------------------------------------------------
//
//
//  // The following block copy the cliques and separators without connections.
//  // --------------------------------------------------------------------------
//  map<int, Clique*> map_cliques;
//  map<int, Separator*> map_separators;
//
//  for (const auto &c : jt->set_clique_ptr_container) {
//    map_cliques[c->clique_id] = c->CopyWithoutPtr();
//    this->set_clique_ptr_container.insert(map_cliques[c->clique_id]);
//    for (const auto &s_p : c->set_neighbours_ptr) {
//      seps_that_cliques_connect_to[c->clique_id][s_p->clique_id] = 1; // Record the connections.
//    }
//  }
//  for (const auto &s : jt->set_separator_ptr_container) {
//    map_separators[s->clique_id] = s->CopyWithoutPtr();
//    this->set_separator_ptr_container.insert(map_separators[s->clique_id]);
//    for (const auto &c_p : s->set_neighbours_ptr) {
//      cliques_that_seps_connect_to[s->clique_id][c_p->clique_id] = 1; // Record the connections
//    }
//  }
//  // --------------------------------------------------------------------------
//
//
//  // The following block is to restore the connections.
//  // --------------------------------------------------------------------------
////  #pragma omp parallel for collapse(2)
//  for (int i=0; i<jt->set_clique_ptr_container.size(); ++i) {
//    for (int j=0; j<jt->set_separator_ptr_container.size(); ++j) {
//      if (seps_that_cliques_connect_to[i][j]==1) {
//        map_cliques[i]->set_neighbours_ptr.insert(map_separators[j]);
//      }
//    }
//  }
//
//  for (int i=0; i<jt->set_separator_ptr_container.size(); ++i) {
//    for (int j=0; j<jt->set_clique_ptr_container.size(); ++j) {
//      if (cliques_that_seps_connect_to[i][j]==1) {
//        map_separators[i]->set_neighbours_ptr.insert(map_cliques[j]);
//      }
//    }
//  }
//  // --------------------------------------------------------------------------
//
//  this->BackUpJunctionTree();
//
//  for (int i=0; i<jt->set_separator_ptr_container.size(); ++i) {
//    delete[] cliques_that_seps_connect_to[i];
//  }
//  delete[] cliques_that_seps_connect_to;
//
//  for (int i=0; i<jt->set_clique_ptr_container.size(); ++i) {
//    delete[] seps_that_cliques_connect_to[i];
//  }
//  delete[] seps_that_cliques_connect_to;
//}


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
                               vector<int> elim_ord) { //checked
  if (elim_ord.size() == 0) {
    return;
  }

    int num_nodes = net->num_nodes;

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
    Clique* clique = new Clique(set_node_ptrs_to_form_a_clique);
    bool to_be_inserted = true;
    for (auto &ptr_clq : set_clique_ptr_container) {
        set<int> intersection;
        set_intersection(clique->clique_variables.begin(), clique->clique_variables.end(),
                         ptr_clq->clique_variables.begin(), ptr_clq->clique_variables.end(),
                         std::inserter(intersection, intersection.begin()));
        if (intersection == clique->clique_variables) {
            to_be_inserted = false;
            break;
        }
    }

    if (to_be_inserted) {
        set_clique_ptr_container.insert(clique);
//        cout << "clique " << set_clique_ptr_container.size() << ": ";
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

    Triangulate(net, adjac_matrix, elim_ord);
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
//      if (this_clq->table.related_variables.find(elimination_ordering.at(j))!=this_clq->table.related_variables.end()) {
//        next_clq = map_elim_var_to_clique[elimination_ordering.at(j)];
//        break;
//      }
//    }
//    set<int> common_related_variables;
//    set_intersection(this_clq->table.related_variables.begin(),this_clq->table.related_variables.end(),
//                     next_clq->table.related_variables.begin(),next_clq->table.related_variables.end(),
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
void JunctionTree::FormJunctionTree() {
  // First, generate all possible separators.
  set<pair<Clique*,Clique*>> mark;
  set<Separator*> all_possible_seps;
  // TODO: can we just traverse i from 0 to cliques.size and j from i+1 to cliques.size?
  // TODO: then we dont need to use "mark" or frequently find pair from "mark"
  for (auto &clique_ptr : set_clique_ptr_container) {
    for (auto &clique_ptr_2 : set_clique_ptr_container) {

      if (clique_ptr == clique_ptr_2) {
        continue; // The same cliques do not need a separator
      } else if (mark.find(pair<Clique*,Clique*>(clique_ptr,clique_ptr_2)) != mark.end()) {
        continue; // The separator of this pair of cliques has been generated
      }

      // Mark this pair.
      mark.insert(pair<Clique*,Clique*>(clique_ptr,clique_ptr_2));
      mark.insert(pair<Clique*,Clique*>(clique_ptr_2,clique_ptr));

      set<int> common_variables;
      set_intersection(clique_ptr->clique_variables.begin(),clique_ptr->clique_variables.end(),
                       clique_ptr_2->clique_variables.begin(),clique_ptr_2->clique_variables.end(),
                       std::inserter(common_variables,common_variables.begin()));

      // If they have no common variables, then they will not be connected by separator.
      if (common_variables.empty()) {
          continue;
      }

      set<Node*> common_related_node_ptrs;
      for (auto &v : common_variables) {
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
  tree_so_far.insert(*set_clique_ptr_container.begin()); // randomly insert a clique in tree, as the start of the Prim algorithm

  while (tree_so_far.size()<set_clique_ptr_container.size()) {
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
            sep_ptr->is_in_jt = true;
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

//      cout << "insert clique ";
//      for (auto &v: clq1->clique_variables) {
//          cout << v << " ";
//      }
//      cout << endl;
//      cout << "insert clique ";
//      for (auto &v: clq2->clique_variables) {
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

    for (auto it = all_possible_seps.begin(); it != all_possible_seps.end(); ) { // traverse all separators
        if (!(*it)->is_in_jt) { // if this sep is not used
            auto tmp = *it;
            all_possible_seps.erase(it++);
            delete tmp;
        } else {
            it++;
        }
    }
}

void JunctionTree::CliqueMerging(int threshold) {
    int num_clique_old = 0;
    int num_clique_new = set_clique_ptr_container.size();

    while (num_clique_old != num_clique_new) { // repeat until no reduction in cliques
        for (auto it = set_separator_ptr_container.begin(); it != set_separator_ptr_container.end(); ) { // traverse all seps
            auto iter = (*it)->set_neighbours_ptr.begin();
            Clique *clq1 = *iter, *clq2 = *(++iter);
            if (clq1->clique_size < threshold || clq2->clique_size < threshold) {
                // 1. remove this sep from the neighbors of clq1 and clq2
                clq1->set_neighbours_ptr.erase(*it);
                clq2->set_neighbours_ptr.erase(*it);
                // 2. remove and delete the sep
                auto tmp = *it;
                set_separator_ptr_container.erase(it++);
                delete tmp;
                // 3. create a new big clique
                set<int> set_index_to_form_a_clique;
                set<Node*> set_node_ptrs_to_form_a_clique;
                set_index_to_form_a_clique.insert(clq1->clique_variables.begin(), clq1->clique_variables.end());
                set_index_to_form_a_clique.insert(clq2->clique_variables.begin(), clq2->clique_variables.end());
                for (auto index: set_index_to_form_a_clique) {
                    set_node_ptrs_to_form_a_clique.insert(network->FindNodePtrByIndex(index));
                }
                Clique* clique = new Clique(set_node_ptrs_to_form_a_clique);
                set_clique_ptr_container.insert(clique);
                // 4. all neighbors of clq1 and clq2 become neighbors of the new big clique, and vice versa,
                //    and remove clq1 and clq2 from the neighbors of their neighbors
                // note that we don't remove their neighbors from the neighbors of clq1 and clq2
                // because to do this may cause problem of erasing and traversing at the same time
                // and we will erase and delete clq1 and clq2 so their neighbors don't matter
                for (auto &sep_ptr: clq1->set_neighbours_ptr) {
                    clique->set_neighbours_ptr.insert(sep_ptr);
                    sep_ptr->set_neighbours_ptr.insert(clique);
                    sep_ptr->set_neighbours_ptr.erase(clq1);
                }
                for (auto &sep_ptr: clq2->set_neighbours_ptr) {
                    clique->set_neighbours_ptr.insert(sep_ptr);
                    sep_ptr->set_neighbours_ptr.insert(clique);
                    sep_ptr->set_neighbours_ptr.erase(clq2);
                }
                // 5. remove and delete clq1 and clq2
                set_clique_ptr_container.erase(clq1);
                set_clique_ptr_container.erase(clq2);
                delete clq1;
                delete clq2;
            } else {
                it++;
            }
        }
        num_clique_old = num_clique_new;
        num_clique_new = set_clique_ptr_container.size();
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
void JunctionTree::AssignPotentials(Timer *timer) { //checked
  // todo: test the correctness of the continuous part (discrete part works correctly)

  // For purely discrete cliques, the potentials have been initialized to 1 on creation,
  // so we need to assign the probabilities of the network nodes to these cliques.
  // For continuous cliques, the lp_potentials and post_bags are set to empty list,
  // so we need to assign the CG regressions to these cliques. The method is described in
  // [Local Propagation in Conditional Gaussian Bayesian Networks (Cowell, 2005)], section 5.2.

  // 1, extract the information of nodes.
  // generate all the factors of the network, which are the probabilities of discrete nodes given their parents
  // Extract the CG regressions of continuous nodes.
//    /************************* use factor ******************************/
//  vector<Factor> factors; // Can not use std::set, because Factor does not have definition on operator "<".
//    /************************* use factor ******************************/
//  vector<CGRegression> cgrs;
    /************************* use potential table ******************************/
  vector<PotentialTable> potential_tables;
    /************************* use potential table ******************************/

  for (auto &id_node_ptr : network->map_idx_node_ptr) { // for each node of the network
    auto node_ptr = id_node_ptr.second;
    if (node_ptr->is_discrete) {
      // add the factor that consists of this node and its parents
//        /************************* use factor ******************************/
//        factors.push_back(Factor(dynamic_cast<DiscreteNode*>(node_ptr), this->network)); // each node has one factor
//        /************************* use factor ******************************/

        /************************* use potential table ******************************/
        potential_tables.push_back(PotentialTable(dynamic_cast<DiscreteNode*>(node_ptr), this->network));
        /************************* use potential table ******************************/
    }
//    else {  // If the node is continuous.
//      cgrs.push_back(CGRegression(node_ptr, network->GetParentPtrsOfNode(node_ptr->GetNodeIndex())));
//    }
  }

//    cout << "in assign potentials, after constructing all factors: " << endl;
//    for (int i = 0; i < factors.size(); ++i) {
//        Factor f = factors.at(i);
//        cout << "factor " << i << ": ";
//        for (auto &v : f.related_variables) {
//            cout << v << " ";
//        }
//        cout << endl;
//        // set<DiscreteConfig> set_disc_configs; DiscreteConfig: set< pair<int, int> >
//        // map<DiscreteConfig, double> map_potentials
//        for (auto &config: f.set_disc_configs) {
//            cout << "config: ";
//            for (auto &varval: config) {
//                cout << varval.first << "=" << varval.second << " ";
//            }
//            cout << ": " << f.map_potentials[config] << endl;
//        }
//    }
//    cout << "in assign potentials, after constructing all potential tables: " << endl;
//    for (int i = 0; i < potential_tables.size(); ++i) {
//        PotentialTable pt = potential_tables.at(i);
//        // set<int> related_variables
//        cout << "table " << i << ": ";
//        for (auto &v : pt.related_variables) {
//            cout << v << " ";
//        }
//        cout << endl;
//        // int num_variables
//        // int table_size
//        cout << "num variables = " << pt.num_variables << ", table size = " << pt.table_size << endl;
//        // vector<int> var_dims
//        cout << "var dims: ";
//        for (int j = 0; j < pt.var_dims.size(); ++j) {
//            cout << pt.var_dims[j] << " ";
//        }
//        // vector<int> cum_levels
//        cout << "cum_levels: ";
//        for (int j = 0; j < pt.cum_levels.size(); ++j) {
//            cout << pt.cum_levels[j] << " ";
//        }
//        // vector<double> potentials
//        cout << "table: " << endl;
//        for (int j = 0; j < pt.potentials.size(); ++j) {
//            cout << pt.potentials[j] << endl;
//        }
//    }


  // 2, (part of) BP algorithm
  //    2.1 assign each factors and CG regressions to a clique
  //    each factor and CG regression should be use only once

//    /************************* use factor ******************************/
//  // For potentials from discrete nodes, they should be assigned to purely discrete cliques.
//  for (auto &f : factors) { // for each factor of the network
//    for (auto &clique_ptr : set_clique_ptr_container) { // for each clique of the graph
//
//      if (f.related_variables.empty() || clique_ptr->clique_variables.empty()) {
//        break;
//      }
//      if (!clique_ptr->pure_discrete) {
//        continue;
//      }
//
//      // get the variables that in the factor but not in the clique
//      set<int> diff;
//      set_difference(f.related_variables.begin(), f.related_variables.end(),
//                     clique_ptr->clique_variables.begin(), clique_ptr->clique_variables.end(),
//                     inserter(diff, diff.begin()));
//      // If "diff" is empty, i.e., all the variables in the factor are in the clique,
//      // which means that the clique can accommodate the scope of the factor - satisfy the family preservation property of cluster graph
//      // (clique tree is a special case of cluster graph)
//      // so we can assign this factor to this clique
//      if (diff.empty()) {
//        // 2.2 construct the initial potential of this clique,
//        // which is the product of factors that assigned to it
////          cout << "assign factor ";
////          for (auto &v: f.related_variables) {
////              cout << v << " ";
////          }
////          cout << "to clique ";
////          for (auto &v: clique_ptr->clique_variables) {
////              cout << v << " ";
////          }
////          cout << endl;
//        clique_ptr->MultiplyWithFactorSumOverExternalVars(f, timer);
//        break;  // Ensure that each factor is used only once.
//      }
//    }
//  }
//    /************************* use factor ******************************/

    /************************* use potential table ******************************/
    // For potentials from discrete nodes, they should be assigned to purely discrete cliques.
    for (auto &pt : potential_tables) { // for each factor of the network
        for (auto &clique_ptr : set_clique_ptr_container) { // for each clique of the graph
            if (pt.related_variables.empty() || clique_ptr->clique_variables.empty()) {
                break;
            }
            if (!clique_ptr->pure_discrete) {
                continue;
            }

            // get the variables that in the factor but not in the clique
            set<int> diff;
            set_difference(pt.related_variables.begin(), pt.related_variables.end(),
                           clique_ptr->clique_variables.begin(), clique_ptr->clique_variables.end(),
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
//          for (auto &v: clique_ptr->clique_variables) {
//              cout << v << " ";
//          }
//          cout << endl;
                clique_ptr->MultiplyWithFactorSumOverExternalVars(pt, timer);
                break;  // Ensure that each factor is used only once.
            }
        }
    }
    /************************* use potential table ******************************/
//  for (auto &cgr : cgrs) {
//    for (auto &clique_ptr : set_clique_ptr_container) {
//
//      if (clique_ptr->pure_discrete) { continue; }
//
//      set<int> cgr_related_vars = cgr.set_all_tail_index;
//      cgr_related_vars.insert(cgr.head_var_index);
//      set<int> diff;
//      set_difference(cgr_related_vars.begin(), cgr_related_vars.end(),
//                     clique_ptr->table.related_variables.begin(), clique_ptr->table.related_variables.end(),
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

/**
 * @brief: when inferring, an evidence is given. The evidence needs to be loaded and propagate in the network.
 */
void JunctionTree::LoadDiscreteEvidence(const DiscreteConfig &E) {
//    /*********************** load evidence on all cliques and seps **************************/
//    // cannot just erase "comb" inside the inner most loop,
//    // because it will cause problem in factor division...
//    for (auto &e: E) { // for each observation of variable
//        for (auto &clique_ptr : set_clique_ptr_container) { // for each clique
//            // if this factor is related to the observation
//            if (clique_ptr->table.related_variables.find(e.first) != clique_ptr->table.related_variables.end()) {
//                clique_ptr->table.related_variables.erase(e.first);
//
//                set <DiscreteConfig> set_reduced_disc_configs;
//                map<DiscreteConfig, double> map_reduced_potentials;
//                // for each discrete config of this factor
//                for (auto &comb: clique_ptr->table.set_disc_configs) {
//                    // if this config and the evidence have different values on common variables,
//                    // which means that they conflict, then this config will be removed
//                    // so otherwise, it will be kept
//                    if (comb.find(e) != comb.end()) {
//                        DiscreteConfig reduced_config; // we need to reduce the config, remove the unrelated variables
//                        for (auto &p: comb) { // for all pairs in the config (we select all related variables from them)
//                            if (p.first != e.first) {
//                                // this variable is not unrelated, so this pair should be kept
//                                reduced_config.insert(p);
//                            }
//                        }
//                        // add new
//                        set_reduced_disc_configs.insert(reduced_config);
//                        map_reduced_potentials[reduced_config] = clique_ptr->table.map_potentials[comb];
//                    }
//                }
//                clique_ptr->table.set_disc_configs = set_reduced_disc_configs;
//                clique_ptr->table.map_potentials = map_reduced_potentials;
//            }
//        }
//    }
//
//    for (auto &e: E) { // for each observation of variable
//        for (auto &sep_ptr : set_separator_ptr_container) { // for each sep
//            // if this factor is related to the observation
//            if (sep_ptr->table.related_variables.find(e.first) != sep_ptr->table.related_variables.end()) {
//                sep_ptr->table.related_variables.erase(e.first);
//
//                set<DiscreteConfig> set_reduced_disc_configs;
//                map<DiscreteConfig, double> map_reduced_potentials;
//                // for each discrete config of this factor
//                for (auto &comb: sep_ptr->table.set_disc_configs) {
//                    // if this config and the evidence have different values on common variables,
//                    // which means that they conflict, then this config will be removed
//                    // so otherwise, it will be kept
//                    if (comb.find(e) != comb.end()) {
//                        auto tmp_potential = sep_ptr->table.map_potentials[comb]; // save the potential of this config
//
//                        DiscreteConfig reduced_config; // we need to reduce the config, remove the unrelated variables
//                        for (auto &p: comb) { // for all pairs in the config (we select all related variables from them)
//                            if (p.first != e.first) {
//                                // this variable is not unrelated, so this pair should be kept
//                                reduced_config.insert(p);
//                            }
//                        }
//
//                        // add new
//                        set_reduced_disc_configs.insert(reduced_config);
//                        map_reduced_potentials[reduced_config] = tmp_potential;
//                    }
//                }
//
//                sep_ptr->table.set_disc_configs = set_reduced_disc_configs;
//                sep_ptr->table.map_potentials = map_reduced_potentials;
//            }
//        }
//    }
//    /*********************** load evidence on all cliques and seps **************************/

//    /*********************** load evidence on only one clique **************************/
//    for (auto &e : E) {  // For each node's observation in E
//        for (auto &clique_ptr : set_clique_ptr_container) { // for each clique
//            // If this clique is related to this node
//            if (clique_ptr->table.related_variables.find(e.first) != clique_ptr->table.related_variables.end()) {
//                for (auto &comb : clique_ptr->table.set_disc_configs) {
//                    if (comb.find(e) == comb.end()) {
//                        clique_ptr->table.map_potentials[comb] = 0;
//                    }
//                }
//                break;
//            }
//        }
//    }
//    /*********************** load evidence on only one clique **************************/

    /************************* use potential table ******************************/
    for (auto &e: E) { // for each observation of variable
        // we need the index of the the evidence and the value index
        int index = e.first;
        int value = e.second;
        if (index >= network->num_nodes) {
            continue; // todo: this is because the testing set has more features than the training set
        }

        int value_index;
        auto dn = dynamic_cast<DiscreteNode*>(network->FindNodePtrByIndex(index));
        for (int i = 0; i < dn->GetDomainSize(); ++i) {
            if (value == dn->vec_potential_vals.at(i)) {
                value_index = i;
                break;
            }
        }

        for (auto &clique_ptr : set_clique_ptr_container) { // for each clique
//            cout << "clique ";
//            for (auto &v: clique_ptr->p_table.related_variables) {
//                cout << v << " ";
//            }
//            cout << endl;
            // if this factor is related to the observation
            if (clique_ptr->p_table.related_variables.find(index) != clique_ptr->p_table.related_variables.end()) {
//                cout << "this clique is related to the observation" << endl;
                clique_ptr->p_table.TableReduction(index, value_index);
            }
        }
        for (auto &sep_ptr : set_separator_ptr_container) { // for each sep
//            cout << "sep ";
//            for (auto &v: sep_ptr->p_table.related_variables) {
//                cout << v << " ";
//            }
//            cout << endl;
            // if this factor is related to the observation
            if (sep_ptr->p_table.related_variables.find(index) != sep_ptr->p_table.related_variables.end()) {
//                cout << "this sep is related to the observation" << endl;
                sep_ptr->p_table.TableReduction(index, value_index);
            }
        }
    }
    /************************* use potential table ******************************/
}

///**
// * @brief: when inferring, an evidence is given. The evidence needs to be loaded and propagate in the network.
// */
//void JunctionTree::LoadDiscreteEvidence(const DiscreteConfig &E) {
//  if (E.empty()) {
//    return;
//  }
//  for (auto &e : E) {  // For each node's observation in E
//    if (network->FindNodePtrByIndex(e.first)->is_discrete) {
//      Clique *clique_ptr = map_elim_var_to_clique[e.first];
//      for (auto &comb : clique_ptr->table.set_disc_configs) {  // Update each row of map_potentials
//        if (comb.find(e) == comb.end()) {
//          clique_ptr->table.map_potentials[comb] = 0;//conflict with the evidence; set the potential to 0.
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
void JunctionTree::MessagePassingUpdateJT(Timer *timer) {
  // Arbitrarily select a clique as the root.
  auto iter = set_clique_ptr_container.begin();
  Clique *arb_root = *iter;
//    /************************* use factor ******************************/
//  arb_root->Collect(timer);
//  arb_root->Distribute(timer);
//    /************************* use factor ******************************/

    /************************* use potential table ******************************/
    arb_root->Collect2(timer);
    arb_root->Distribute2(timer);
    /************************* use potential table ******************************/
}

//void JunctionTree::PrintAllCliquesPotentials() const {
//  cout << "Cliques" << '\n';
//  for (auto &c : set_clique_ptr_container) {
//    c->PrintPotentials();
//  }
//  cout << "==================================================" << endl;
//}

//void JunctionTree::PrintAllSeparatorsPotentials() const {
//  cout << "Separators" << '\n';
//  for (auto &s : set_separator_ptr_container) {
//    s->PrintPotentials();
//  }
//  cout << "==================================================" << endl;
//}

/**
 * @brief: compute the marginal distribution for a query variable
 **/
Factor JunctionTree::BeliefPropagationCalcuDiscreteVarMarginal(int query_index) {

  // The input is a set of query_indexes of variables.
  // The output is a factor representing the joint marginal of these variables.
  // TODO: here only support one query variable

  int min_size = INT32_MAX;
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
    if (c->table.related_variables.find(query_index) == c->table.related_variables.end()) { // cannot find the query variable
        continue;
    }
    if (c->table.related_variables.size() >= min_size) {
        continue;
    }
    min_size = c->table.related_variables.size();
    selected_clique = c;
  }

  if (selected_clique == nullptr) {
    fprintf(stderr, "Error in function [%s]\n"
                    "Variable [%d] does not appear in any clique!", __FUNCTION__, query_index);
    exit(1);
  }

  set<int> other_vars = selected_clique->table.related_variables;
  other_vars.erase(query_index);

  Factor f(selected_clique->table.related_variables, selected_clique->table.set_disc_configs, selected_clique->table.map_potentials);

  for (auto &index : other_vars) {
    f = f.SumOverVar(index);
  }

  f.Normalize(); // todo: no need to do normalization
  return f;

}

/**
 * @brief: compute the marginal distribution for a query variable
 **/
PotentialTable JunctionTree::BeliefPropagationCalcuDiscreteVarMarginal2(int query_index) {

    // The input is a set of query_indexes of variables.
    // The output is a factor representing the joint marginal of these variables.
    // TODO: here only support one query variable

    int min_size = INT32_MAX;
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
        if (c->p_table.related_variables.find(query_index) == c->p_table.related_variables.end()) { // cannot find the query variable
            continue;
        }
        if (c->p_table.related_variables.size() >= min_size) {
            continue;
        }
        min_size = c->p_table.related_variables.size();
        selected_clique = c;
    }

    if (selected_clique == nullptr) {
        fprintf(stderr, "Error in function [%s]\n"
                        "Variable [%d] does not appear in any clique!", __FUNCTION__, query_index);
        exit(1);
    }

    set<int> other_vars = selected_clique->p_table.related_variables;
    other_vars.erase(query_index);

    PotentialTable pt = selected_clique->p_table;

    for (auto &index : other_vars) {
        pt.TableMarginalization(index);
    }

    pt.Normalize(); // todo: no need to do normalization
    return pt;

}

/**
 * @brief: predict the label for a given variable.
 */
int JunctionTree::InferenceUsingBeliefPropagation(int &query_index) {
//    /************************* use factor ******************************/
//  Factor f = BeliefPropagationCalcuDiscreteVarMarginal(query_index);
//  double max_prob = 0;
//  DiscreteConfig comb_predict; // set< pair<int, int> >
//  for (auto &comb : f.set_disc_configs) {
//    if (f.map_potentials[comb] > max_prob) {
//      max_prob = f.map_potentials[comb];
//      comb_predict = comb;
//    }
//  }
//  int label_predict = comb_predict.begin()->second; // TODO: use "query_index"
//    /************************* use factor ******************************/

    /************************* use potential table ******************************/
    PotentialTable pt = BeliefPropagationCalcuDiscreteVarMarginal2(query_index);
    double max_prob = 0;
    int max_index;
    for (int i = 0; i < pt.table_size; ++i) { // traverse the potential table
        if (pt.potentials[i] > max_prob) {
            max_prob = pt.potentials[i];
            max_index = i;
        }
    }
    // "pt" has only one related variable, which is exactly the query variable,
    // so the "max_index" exactly means which value of the query variable gets the max probability
    auto dn = dynamic_cast<DiscreteNode*>(network->FindNodePtrByIndex(query_index));
    int label_predict = dn->vec_potential_vals.at(max_index);
    /************************* use potential table ******************************/

  return label_predict;
}

/**
 * @brief: test the Junction Tree given a data set
 */
double JunctionTree::EvaluateAccuracy(Dataset *dts, int num_samp, string alg, bool is_dense) {

  cout << "==================================================" << '\n'
       << "Begin testing the trained network." << endl;

    Timer *timer = new Timer();
    // record time
    timer->Start("jt");

    int m = dts->num_instance;

    int class_var_index = dts->class_var_index;

    vector<DiscreteConfig> evidences;
    evidences.reserve(m);
    vector<int> ground_truths;
    ground_truths.reserve(m);

    for (int i = 0; i < m; ++i) {  // For each sample in test set
        vector<VarVal> vec_instance = dts->vector_dataset_all_vars.at(i);

        // construct an evidence by removing the class variable
        DiscreteConfig e;
        pair<int, int> p;
        for (int j = 0; j < vec_instance.size(); ++j) {
            if (j == class_var_index) { // skip the class variable
                continue;
            }
            p.first = vec_instance.at(j).first;
            p.second = vec_instance.at(j).second.GetInt();
            e.insert(p);
        }

        if (is_dense) {
            e = Sparse2Dense(e, network->num_nodes, class_var_index);
        }
        evidences.push_back(e);

        // construct the ground truth
        int g = vec_instance.at(class_var_index).second.GetInt();
        ground_truths.push_back(g);

//      cout << "testing sample " << i << endl << "before load evidences: " << endl << "cliques: " << endl;
//      for (auto &c : set_clique_ptr_container) {
//          cout << c->clique_id << ": ";
//          for (auto &v : c->clique_variables) {
//              cout << v << " ";
//          }
//          cout << endl;
//          // set<DiscreteConfig> set_disc_configs; DiscreteConfig: set< pair<int, int> >
//          // map<DiscreteConfig, double> map_potentials
//          for (auto &config: c->table.set_disc_configs) {
//              cout << "config: ";
//              for (auto &varval: config) {
//                  cout << varval.first << "=" << varval.second << " ";
//              }
//              cout << ": " << c->table.map_potentials[config] << endl;
//          }
//      }
    }

    // predict the labels of the test instances
    vector<int> predictions = PredictUseJTInfer(evidences, class_var_index, timer);

    double accuracy = Accuracy(ground_truths, predictions);

    timer->Stop("jt");
    setlocale(LC_NUMERIC, "");

//    double accuracy = num_of_correct / (double)(num_of_correct+num_of_wrong);
    cout << '\n' << "Accuracy: " << accuracy << endl;
    cout << "==================================================";
    timer->Print("jt");
    timer->Print("load evidence"); cout << " (" << timer->time["load evidence"] / timer->time["jt"] * 100 << "%)";
    timer->Print("msg passing"); cout << " (" << timer->time["msg passing"] / timer->time["jt"] * 100 << "%)";
    timer->Print("predict"); cout << " (" << timer->time["predict"] / timer->time["jt"] * 100 << "%)";
    timer->Print("reset"); cout << " (" << timer->time["reset"] / timer->time["jt"] * 100 << "%)" << endl;
    timer->Print("factor marginalization"); cout << " (" << timer->time["factor marginalization"] / timer->time["msg passing"] * 100 << "%)";
    timer->Print("factor multiplication"); cout << " (" << timer->time["factor multiplication"] / timer->time["msg passing"] * 100 << "%)";
    timer->Print("factor division"); cout << " (" << timer->time["factor division"] / timer->time["msg passing"] * 100 << "%)";
    timer->Print("set_difference"); cout << " (" << timer->time["set_difference"] / timer->time["msg passing"] * 100 << "%)" << endl;
//    timer->Print("construct clique"); cout << " (" << timer->time["construct clique"] / timer->time["msg passing"] * 100 << "%)";
//    timer->Print("update clique"); cout << " (" << timer->time["update clique"] / timer->time["msg passing"] * 100 << "%)";
//    timer->Print("construct sep"); cout << " (" << timer->time["construct sep"] / timer->time["msg passing"] * 100 << "%)";
//    timer->Print("update sep"); cout << " (" << timer->time["update sep"] / timer->time["msg passing"] * 100 << "%)" << endl;
    delete timer;
    timer = nullptr;

    return accuracy;
}

/**
 * @brief: predict label given evidence E and target variable id Y_index
 * @return label of the target variable
 */
int JunctionTree::PredictUseJTInfer(const DiscreteConfig &E, int Y_index, Timer *timer) {
    timer->Start("load evidence");
    //update a clique using the evidence
    LoadDiscreteEvidence(E);
    timer->Stop("load evidence");

//    cout << "cliques: " << endl;
//    for (auto &c : set_clique_ptr_container) {
//        cout << c->clique_id << ": ";
//        // set<int> related_variables
//        for (auto &v : c->p_table.related_variables) {
//            cout << v << " ";
//        }
//        cout << endl;
//        // int num_variables
//        // int table_size
//        cout << "num variables = " << c->p_table.num_variables << ", table size = " << c->p_table.table_size << endl;
//        // vector<int> var_dims
//        cout << "var dims: ";
//        for (int j = 0; j < c->p_table.var_dims.size(); ++j) {
//            cout << c->p_table.var_dims[j] << " ";
//        }
//        // vector<int> cum_levels
//        cout << "cum_levels: ";
//        for (int j = 0; j < c->p_table.cum_levels.size(); ++j) {
//            cout << c->p_table.cum_levels[j] << " ";
//        }
//        // vector<double> potentials
//        cout << "table: " << endl;
//        for (int j = 0; j < c->p_table.potentials.size(); ++j) {
//            cout << c->p_table.potentials[j] << endl;
//        }
//    }
//    cout << "separators: " << endl;
//    for (auto &s : set_separator_ptr_container) {
//        cout << s->clique_id << ": ";
//        // set<int> related_variables
//        for (auto &v : s->p_table.related_variables) {
//            cout << v << " ";
//        }
//        cout << endl;
//        // int num_variables
//        // int table_size
//        cout << "num variables = " << s->p_table.num_variables << ", table size = " << s->p_table.table_size << endl;
//        // vector<int> var_dims
//        cout << "var dims: ";
//        for (int j = 0; j < s->p_table.var_dims.size(); ++j) {
//            cout << s->p_table.var_dims[j] << " ";
//        }
//        // vector<int> cum_levels
//        cout << "cum_levels: ";
//        for (int j = 0; j < s->p_table.cum_levels.size(); ++j) {
//            cout << s->p_table.cum_levels[j] << " ";
//        }
//        // vector<double> potentials
//        cout << "table: " << endl;
//        for (int j = 0; j < s->p_table.potentials.size(); ++j) {
//            cout << s->p_table.potentials[j] << endl;
//        }
//    }

    timer->Start("msg passing");
    //update the whole Junction Tree
    MessagePassingUpdateJT(timer);
    timer->Stop("msg passing");

    timer->Start("predict");
    int label_predict = InferenceUsingBeliefPropagation(Y_index);
    timer->Stop("predict");

    timer->Start("reset");
    ResetJunctionTree();
    timer->Stop("reset");
    return label_predict;
}

/**
 * @brief: predict the labels given different evidences
 * it just repeats the function above multiple times, and print the progress at the meantime
 * @param elim_orders: elimination order which may be different given different evidences due to the simplification of elimination order
 */
vector<int> JunctionTree::PredictUseJTInfer(const vector<DiscreteConfig> &evidences, int target_node_idx, Timer *timer) {
    int size = evidences.size();

    cout << "Progress indicator: ";
    int every_1_of_20 = size / 20; // used to print, print 20 times in total
    int progress = 0;

    vector<int> results(size, 0);
//#pragma omp parallel for
    for (int i = 0; i < size; ++i) {
//#pragma omp critical
        ++progress;

        if (progress % every_1_of_20 == 0) {
            string progress_percentage = to_string((double)progress/size * 100) + "%...\n";
            fprintf(stdout, "%s\n", progress_percentage.c_str());
//            double acc_so_far = num_of_correct / (double)(num_of_correct+num_of_wrong);
//            fprintf(stdout, "Accuracy so far: %f\n", acc_so_far);
            fflush(stdout);
        }

        int label_predict = PredictUseJTInfer(evidences.at(i), target_node_idx, timer);
        results.at(i) = label_predict;
    }
    return results;
}