#include "JunctionTree.h"

//JunctionTree::JunctionTree(Network *net)
//  : JunctionTree(net, "min-nei") {}
//
//JunctionTree::JunctionTree(Network *net, string elim_ord_strategy)
//  : JunctionTree(net, elim_ord_strategy, vector<int>()) {}

/**
 * the optimized constructor
 * see the comments in the former one (the original implementation)
 */
JunctionTree::JunctionTree(Network *net) {

    cout << "begin construction function of JunctionTree..." << endl;

    Timer *timer = new Timer();
    // record time
    timer->Start("construct jt");

    network = net;

    vector_clique_ptr_container.reserve(network->num_nodes);
    vector_separator_ptr_container.reserve(network->num_nodes - 1);

    int **direc_adjac_matrix = network->ConvertDAGNetworkToAdjacencyMatrix();
    cout << "finish ConvertDAGNetworkToAdjacencyMatrix" << endl;

    Moralize(direc_adjac_matrix, network->num_nodes);
    int **moral_graph_adjac_matrix = direc_adjac_matrix;

    vector<bool> has_processed(network->num_nodes);
    //construct a clique for each node in the network
    Triangulate(network, moral_graph_adjac_matrix, has_processed);
    cout << "finish Triangulate, number of cliques = " << vector_clique_ptr_container.size() << endl;

    for (int i = 0; i < network->num_nodes; ++i) {
        delete[] direc_adjac_matrix[i];
    }
    delete[] direc_adjac_matrix;

    //construct map from main variable to a clique
//  GenMapElimVarToClique();
//  cout << "finish GenMapElimVarToClique" << endl;

    FormJunctionTree();//for discrete nodes
//  FormListShapeJunctionTree(set_clique_ptr_container);//for continuous nodes
    cout << "finish FormJunctionTree, number of cliques = " << vector_clique_ptr_container.size()
         << ", number of separators = " << vector_separator_ptr_container.size() << endl;

//    CliqueMerging(8, 12);
//    cout << "finish CliqueMerging, number of cliques = " << vector_clique_ptr_container.size()
//         << ", number of separators = " << vector_separator_ptr_container.size() << endl;

    //assign id to each clique
    NumberTheCliquesAndSeparators();
//  cout << "finish NumberTheCliquesAndSeparators" << endl;

//    cout << "cliques: " << endl;
//    for (auto &c : vector_clique_ptr_container) {
//        cout << c->clique_id << ": ";
//        // set<int> related_variables
//        for (auto &v : c->p_table.related_variables) {
//            cout << v << " ";
//        }
//        cout << endl;
////        cout << "neighbors: ";
////        for (auto &s: c->set_neighbours_ptr) {
////            cout << s->clique_id << " ";
////        }
////        cout << endl;
////        // int num_variables
////        // int table_size
////        cout << "num variables = " << c->p_table.num_variables << ", table size = " << c->p_table.table_size << endl;
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
//        cout << "table size = " << c->p_table.table_size << endl;
////        // vector<double> potentials
////        cout << "table: " << endl;
////        for (int j = 0; j < c->p_table.potentials.size(); ++j) {
////            cout << c->p_table.potentials[j] << endl;
////        }
//    }

    AssignPotentials();
    cout << "finish AssignPotentials" << endl;

//    cout << "cliques: " << endl;
//    for (auto &c : vector_clique_ptr_container) {
//        cout << c->clique_id << ": ";
//        // set<int> related_variables
//        for (auto &v : c->p_table.related_variables) {
//            cout << v << " ";
//        }
//        cout << endl;
////        // int num_variables
////        // int table_size
////        cout << "num variables = " << c->p_table.num_variables << ", table size = " << c->p_table.table_size << endl;
////        // vector<int> var_dims
////        cout << "var dims: ";
////        for (int j = 0; j < c->p_table.var_dims.size(); ++j) {
////            cout << c->p_table.var_dims[j] << " ";
////        }
////        // vector<int> cum_levels
////        cout << "cum_levels: ";
////        for (int j = 0; j < c->p_table.cum_levels.size(); ++j) {
////            cout << c->p_table.cum_levels[j] << " ";
////        }
//        // vector<double> potentials
//        cout << "table: " << endl;
//        for (int j = 0; j < c->p_table.potentials.size(); ++j) {
//            cout << c->p_table.potentials[j] << endl;
//        }
//    }

    // Arbitrarily select a clique as the root.
    auto iter = vector_clique_ptr_container.begin();
    arb_root = *iter;
    MarkLevel();
//    cout << "finish MarkLevel" << endl;

    BackUpJunctionTree();
//  cout << "finish BackUpJunctionTree" << endl;

    cout << "==================================================";
    timer->Stop("construct jt");
    timer->Print("construct jt"); cout << endl;
    delete timer;
    timer = nullptr;
}

/**
 * this is the original implementation
 * because we optimize triangulation, some parameters, thus some functions, are unnecessary,
 * so simply reduce these unnecessary things and lead to the former optimized constructor
 */
//JunctionTree::JunctionTree(Network *net, string elim_ord_strategy, vector<int> custom_elim_ord) {
//
//    cout << "begin construction function of JunctionTree..." << endl;
//
//    Timer *timer = new Timer();
//    // record time
//    timer->Start("construct jt");
//
//    network = net;
//
//    vector_clique_ptr_container.reserve(network->num_nodes);
//    vector_separator_ptr_container.reserve(network->num_nodes - 1);
//
//  int **direc_adjac_matrix = network->ConvertDAGNetworkToAdjacencyMatrix();
//  cout << "finish ConvertDAGNetworkToAdjacencyMatrix" << endl;
////    for (int i = 0; i < network->num_nodes; ++i) {
////        for (int j = 0; j < network->num_nodes; ++j) {
////            cout << direc_adjac_matrix[i][j] << " ";
////        }
////        cout << endl;
////    }
//
//  Moralize(direc_adjac_matrix, network->num_nodes);
//  int **moral_graph_adjac_matrix = direc_adjac_matrix;
//  cout << "finish Moralize" << endl;
////    for (int i = 0; i < network->num_nodes; ++i) {
////        for (int j = 0; j < network->num_nodes; ++j) {
////            cout << moral_graph_adjac_matrix[i][j] << " ";
////        }
////        cout << endl;
////    }
//
////  cout << "elimination order = " << elim_ord_strategy << endl;
//  // There are different ways of determining elimination ordering.
//  if (elim_ord_strategy == "min-nei") {
//    elimination_ordering = MinNeighbourElimOrd(moral_graph_adjac_matrix, network->num_nodes);
//  }
//  else if (elim_ord_strategy == "rev-topo") {
//    elimination_ordering = network->GetReverseTopoOrd();
//  }
//  else if (elim_ord_strategy == "custom") {
//    if (custom_elim_ord.size() != net->num_nodes) {
//      fprintf(stderr, "Error in function [%s]\nSize of custom elimination"
//                      "ordering and size of the network is not the same!", __FUNCTION__);
//      exit(1);
//    }
//    elimination_ordering = custom_elim_ord;
//  }
//  else {
//    fprintf(stderr, "The elimination ordering strategy should be one of the following:\n"
//                    "{ min-nei, rev-topo, custom }.");
//    exit(1);
//  }
////    cout << "finish order, order: ";
////    for (int i = 0; i < network->num_nodes; ++i) {
////        cout << elimination_ordering[i] << " ";
////    }
////    cout << endl;
//
//    vector<bool> has_processed(network->num_nodes);
//
//  //construct a clique for each node in the network
////  Triangulate(network, moral_graph_adjac_matrix, elimination_ordering);
//    Triangulate(network, moral_graph_adjac_matrix, has_processed);
//  cout << "finish Triangulate, number of cliques = " << vector_clique_ptr_container.size() << endl;
//
//    for (int i = 0; i < network->num_nodes; ++i) {
//        delete[] direc_adjac_matrix[i];
//    }
//    delete[] direc_adjac_matrix;
//
//  //construct map from main variable to a clique
////  GenMapElimVarToClique();
////  cout << "finish GenMapElimVarToClique" << endl;
//
//  FormJunctionTree();//for discrete nodes
////  FormListShapeJunctionTree(set_clique_ptr_container);//for continuous nodes
//    cout << "finish FormJunctionTree, number of cliques = " << vector_clique_ptr_container.size()
//         << ", number of separators = " << vector_separator_ptr_container.size() << endl;
//
////    CliqueMerging(8, 12);
////    cout << "finish CliqueMerging, number of cliques = " << vector_clique_ptr_container.size()
////         << ", number of separators = " << vector_separator_ptr_container.size() << endl;
//
//  //assign id to each clique
//  NumberTheCliquesAndSeparators();
////  cout << "finish NumberTheCliquesAndSeparators" << endl;
//
////    cout << "cliques: " << endl;
////    for (auto &c : vector_clique_ptr_container) {
////        cout << c->clique_id << ": ";
////        // set<int> related_variables
////        for (auto &v : c->p_table.related_variables) {
////            cout << v << " ";
////        }
////        cout << endl;
//////        cout << "neighbors: ";
//////        for (auto &s: c->set_neighbours_ptr) {
//////            cout << s->clique_id << " ";
//////        }
//////        cout << endl;
//////        // int num_variables
//////        // int table_size
//////        cout << "num variables = " << c->p_table.num_variables << ", table size = " << c->p_table.table_size << endl;
////        // vector<int> var_dims
////        cout << "var dims: ";
////        for (int j = 0; j < c->p_table.var_dims.size(); ++j) {
////            cout << c->p_table.var_dims[j] << " ";
////        }
////        // vector<int> cum_levels
////        cout << "cum_levels: ";
////        for (int j = 0; j < c->p_table.cum_levels.size(); ++j) {
////            cout << c->p_table.cum_levels[j] << " ";
////        }
////        cout << "table size = " << c->p_table.table_size << endl;
//////        // vector<double> potentials
//////        cout << "table: " << endl;
//////        for (int j = 0; j < c->p_table.potentials.size(); ++j) {
//////            cout << c->p_table.potentials[j] << endl;
//////        }
////    }
//
//  AssignPotentials(timer);
//  cout << "finish AssignPotentials" << endl;
//
////    cout << "cliques: " << endl;
////    for (auto &c : vector_clique_ptr_container) {
////        cout << c->clique_id << ": ";
////        // set<int> related_variables
////        for (auto &v : c->p_table.related_variables) {
////            cout << v << " ";
////        }
////        cout << endl;
//////        // int num_variables
//////        // int table_size
//////        cout << "num variables = " << c->p_table.num_variables << ", table size = " << c->p_table.table_size << endl;
//////        // vector<int> var_dims
//////        cout << "var dims: ";
//////        for (int j = 0; j < c->p_table.var_dims.size(); ++j) {
//////            cout << c->p_table.var_dims[j] << " ";
//////        }
//////        // vector<int> cum_levels
//////        cout << "cum_levels: ";
//////        for (int j = 0; j < c->p_table.cum_levels.size(); ++j) {
//////            cout << c->p_table.cum_levels[j] << " ";
//////        }
////        // vector<double> potentials
////        cout << "table: " << endl;
////        for (int j = 0; j < c->p_table.potentials.size(); ++j) {
////            cout << c->p_table.potentials[j] << endl;
////        }
////    }
//
//    // Arbitrarily select a clique as the root.
//    auto iter = vector_clique_ptr_container.begin();
//    arb_root = *iter;
//    MarkLevel();
//
//  BackUpJunctionTree();
////  cout << "finish BackUpJunctionTree" << endl;
//
//    cout << "==================================================";
//    timer->Stop("construct jt");
//    timer->Print("construct jt"); cout << endl;
//    delete timer;
//    timer = nullptr;
//}


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

JunctionTree::~JunctionTree() {
    // delete cliques and separators
    for (auto it = vector_clique_ptr_container.begin(); it != vector_clique_ptr_container.end(); it++) {
        if (*it != nullptr) {
            delete *it;
            *it = nullptr;
        }
    }
    for (auto it = vector_separator_ptr_container.begin(); it != vector_separator_ptr_container.end(); it++) {
        if (*it != nullptr) {
            delete *it;
            *it = nullptr;
        }
    }

    delete [] clique_backup;
    delete [] separator_backup;
}


/**
 * @brief: moralization -> moral graph
 * 1. connect all the parents of each node (connect all the v-structure)
 * 2. directed graph -> undirected graph
 */
void JunctionTree::Moralize(int **direc_adjac_matrix, int &num_nodes) {
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

///**
// * @brief: Generate the elimination order by the number of neighbours.
// * The node with the minimum number of neighbours is eliminated first.
// */
//vector<int> JunctionTree::MinNeighbourElimOrd(int **adjac_matrix, int &num_nodes) { //checked
//    vector< pair<int,int> > to_be_sorted;
//
//    // get the number of neighbors for each node
//    for (int i = 0; i < num_nodes; ++i) { // for each node
//        pair<int,int> p; // key: node id; value: number of neighbors
//        p.first = i;
//        p.second = 0;
//        for (int j = 0; j < num_nodes; ++j) {
//            if (adjac_matrix[i][j]==1) { // j is i's neighbor
//                ++p.second;
//            }
//        }
//        to_be_sorted.push_back(p);
//    }
//
//    // sort by the number of neighbors from smallest to largest
//    sort(to_be_sorted.begin(), to_be_sorted.end(), [](pair<int,int> a, pair<int,int> b){return a.second < b.second;});  // Using lambda expression.
//    vector< pair<int,int> > &sorted = to_be_sorted;
//
//    vector<int> result;
//    result.reserve(sorted.size());
//    for (auto &p : sorted) {
//        result.push_back(p.first);
//    }
//    return result;
//}


/**
 * @brief: triangulation -> induced graph
 * definition: no loops of length > 3 without a "bridge"/"chord"
 * one way to construct a induced graph: select a node based on the "elim_ord", connect all its neighbors, and remove this node;
 * the selected node should try to minimize the added edges, so the "elim_ord" may according to the number or neighbors
 * (if we connect two nodes if they appeared in the same factor in a run of the VE algorithm, we can exactly get a induced graph)
 * in this function, the purpose is to generate a clique while constructing the induced graph
 *
 * @note: this is an improved implementation - each time find the node with fewest neighbors to process
 *        the original implementation is following it - use a fixed order generated before, according to the number of neighbors
 */
void JunctionTree::Triangulate(Network *net, int **adjac_matrix, vector<bool> &has_processed) {
    /**
     * terminating condition: all nodes have been processed
     */
    bool terminated = true;
    int num_nodes = net->num_nodes;
    for (int i = 0; i < num_nodes; ++i) {
        if (!has_processed[i]) {
            terminated = false;
            break;
        }
    }
    if (terminated) {
        return;
    }

    /**
     * find the node that has minimum number of neighbors in the current graph
     */
    int min_nei = INT_MAX;
    int min_nei_node;
    for (int i = 0; i < num_nodes; ++i) { // for each node
        if (has_processed[i]) {
            continue;
        }
        // for each unprocessed node
        int num_nei = 0;
        for (int j = 0; j < num_nodes; ++j) {
            if (adjac_matrix[i][j] == 1) { // j is i's neighbor
                ++num_nei;
            }
        }
        if (num_nei < min_nei) {
            min_nei_node = i;
            min_nei = num_nei;
        }
    }

    /**
     * construct a clique
     */
//    cout << "processing node " << min_nei_node << ": " << endl;
    vector<int> vec_neighbors;
    set<int> set_node_indexes_to_form_a_clique;

    // insert the node into "set_node_indexes_to_form_a_clique"
    set_node_indexes_to_form_a_clique.insert(min_nei_node);

    // insert all its neighbors into "vec_neighbors"
    for (int j = 0; j < num_nodes; ++j) {
        if (adjac_matrix[min_nei_node][j] == 1) {
            vec_neighbors.push_back(j);
        }
    }

//    cout << "neis: ";
    // Form a clique that contains
    for (int neighbor = 0; neighbor < vec_neighbors.size(); ++neighbor) {
        for (int neighbor2 = neighbor + 1; neighbor2 < vec_neighbors.size(); ++neighbor2) {
//            if (adjac_matrix[vec_neighbors.at(neighbor)][vec_neighbors.at(neighbor2)] == 0) {
//                cout << "add" << endl;
//            }
            adjac_matrix[vec_neighbors.at(neighbor)][vec_neighbors.at(neighbor2)] = 1;
            adjac_matrix[vec_neighbors.at(neighbor2)][vec_neighbors.at(neighbor)] = 1;
        }
        set_node_indexes_to_form_a_clique.insert(vec_neighbors.at(neighbor));
//        cout << vec_neighbors.at(neighbor) << ", ";
    }
//    cout << endl;

    // before adding a clique, we need to check whether the clique is redundant
    // if a clique is fully contained by another (existing/previous) clique, then the clique is no need to be inserted.
    bool to_be_inserted = true;
    for (auto &ptr_clq : vector_clique_ptr_container) {
        set<int> intersection;
        set_intersection(set_node_indexes_to_form_a_clique.begin(), set_node_indexes_to_form_a_clique.end(),
                         ptr_clq->clique_variables.begin(), ptr_clq->clique_variables.end(),
                         std::inserter(intersection, intersection.begin()));
        if (intersection == set_node_indexes_to_form_a_clique) {
            to_be_inserted = false;
            break;
        }
    }

    if (to_be_inserted) {
        Clique* clique = new Clique(set_node_indexes_to_form_a_clique, net);
        vector_clique_ptr_container.push_back(clique);
//        cout << "insert a clique: ";
//        for (auto &v : clique->p_table.related_variables) {
//            cout << v << " ";
//        }
//        cout << endl;
//        // vector<int> var_dims
//        cout << "var dims: ";
//        for (int j = 0; j < clique->p_table.var_dims.size(); ++j) {
//            cout << clique->p_table.var_dims[j] << " ";
//        }
//        // vector<int> cum_levels
//        cout << "cum_levels: ";
//        for (int j = 0; j < clique->p_table.cum_levels.size(); ++j) {
//            cout << clique->p_table.cum_levels[j] << " ";
//        }
//        // vector<double> potentials
//        cout << "table size = " << clique->p_table.potentials.size() << endl;
//        cout << "table size = " << clique->p_table.table_size << endl;
//        cout << "table: " << endl;
//        for (int j = 0; j < clique->p_table.potentials.size(); ++j) {
//            cout << clique->p_table.potentials[j] << endl;
//        }
    }

    /**
     * remove the node and its edges
     */
    // mark the node because it has already form a clique
    has_processed[min_nei_node] = 1;
    // the edges connected to it should be removed
    for (int neighbor = 0; neighbor < vec_neighbors.size(); ++neighbor) {
        adjac_matrix[min_nei_node][vec_neighbors.at(neighbor)] = 0;
        adjac_matrix[vec_neighbors.at(neighbor)][min_nei_node] = 0;
    }
//    cout << "post-processing" << endl;

    Triangulate(net, adjac_matrix, has_processed);
}

//void JunctionTree::Triangulate(Network *net,
//                               int **adjac_matrix,
//                               vector<int> elim_ord) { //checked
//    if (elim_ord.size() == 0) { // terminating condition for recursive procedure
//        return;
//    }
//
//    int num_nodes = net->num_nodes;
//
//    vector<int> vec_neighbors;
////    set<Node*> set_node_ptrs_to_form_a_clique;
//    set<int> set_node_indexes_to_form_a_clique; // use node index instead
//    int first_node_in_elim_ord = elim_ord.front();
//
//    // insert the first node in the elimination order into "set_node_ptrs_to_form_a_clique"
////    set_node_ptrs_to_form_a_clique.insert(net->FindNodePtrByIndex(first_node_in_elim_ord));
//    set_node_indexes_to_form_a_clique.insert(first_node_in_elim_ord);
//
//    // insert all its neighbors into "vec_neighbors"
//    for (int j = 0; j < num_nodes; ++j) {
//        if (adjac_matrix[first_node_in_elim_ord][j] == 1) {
//            vec_neighbors.push_back(j);
//        }
//    }
//
//    // Form a clique that contains
//    for (int neighbor = 0; neighbor < vec_neighbors.size(); ++neighbor) {
//        for (int neighbor2 = neighbor + 1; neighbor2 < vec_neighbors.size(); ++neighbor2) {
////            if (adjac_matrix[vec_neighbors.at(neighbor)][vec_neighbors.at(neighbor2)] == 0) {
////                cout << "add" << endl;
////            }
//            adjac_matrix[vec_neighbors.at(neighbor)][vec_neighbors.at(neighbor2)] = 1;
//            adjac_matrix[vec_neighbors.at(neighbor2)][vec_neighbors.at(neighbor)] = 1;
//        }
//        set_node_indexes_to_form_a_clique.insert(vec_neighbors.at(neighbor));
//    }
//
//    // before adding a clique, we need to check whether the clique is redundant
//    // if a clique is fully contained by another (existing/previous) clique, then the clique is no need to be inserted.
//    bool to_be_inserted = true;
//    for (auto &ptr_clq : vector_clique_ptr_container) {
//        set<int> intersection;
//        set_intersection(set_node_indexes_to_form_a_clique.begin(), set_node_indexes_to_form_a_clique.end(),
//                         ptr_clq->clique_variables.begin(), ptr_clq->clique_variables.end(),
//                         std::inserter(intersection, intersection.begin()));
//        if (intersection == set_node_indexes_to_form_a_clique) {
//            to_be_inserted = false;
//            break;
//        }
//    }
//
//    if (to_be_inserted) {
//        Clique* clique = new Clique(set_node_indexes_to_form_a_clique, net);
//        vector_clique_ptr_container.push_back(clique);
//    }
//
//    // Remove the first node in elimination ordering, which has already form a clique.
//    elim_ord.erase(elim_ord.begin());
//    // The node has been removed, so the edges connected to it should be removed too.
//    for (int neighbor = 0; neighbor < vec_neighbors.size(); ++neighbor) {
//        adjac_matrix[first_node_in_elim_ord][vec_neighbors.at(neighbor)] = 0;
//        adjac_matrix[vec_neighbors.at(neighbor)][first_node_in_elim_ord] = 0;
//    }
//
//    Triangulate(net, adjac_matrix, elim_ord);
//}

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

int JunctionTree::GetIndexByCliquePtr(Clique* clq) {
    for (int i = 0; i < vector_clique_ptr_container.size(); ++i) {
        if (vector_clique_ptr_container[i] == clq) {
            return i;
        }
    }
    return -1;
}

/**
 * @brief: construct a tree where each node is a clique and each edge is a separator.
 * use Prim algorithm; the weights of edges is represented by the weights of the separators
 */
void JunctionTree::FormJunctionTree() {
    // First, generate all possible separators.
    set<Separator*> all_possible_seps;
    for (int i = 0; i < vector_clique_ptr_container.size(); ++i) {
        for (int j = i + 1; j < vector_clique_ptr_container.size(); ++j) {
            auto clique_ptr = vector_clique_ptr_container[i];
            auto clique_ptr_2 = vector_clique_ptr_container[j];

            set<int> common_variables;
            set_intersection(clique_ptr->clique_variables.begin(), clique_ptr->clique_variables.end(),
                             clique_ptr_2->clique_variables.begin(), clique_ptr_2->clique_variables.end(),
                             std::inserter(common_variables,common_variables.begin()));

            // If they have no common variables, then they will not be connected by separator.
            if (common_variables.empty()) {
                continue;
            }

            Separator *sep = new Separator(common_variables, network);
            // Let separator know the two cliques that it connects to.
            sep->set_neighbours_ptr.insert(clique_ptr);
            sep->set_neighbours_ptr.insert(clique_ptr_2);

            all_possible_seps.insert(sep);
        }
    }

//  for (auto &sep_ptr : all_possible_seps) {
//      cout << "sep neis: ";
//      for (auto &nei :sep_ptr->set_neighbours_ptr) {
//          cout << GetIndexByCliquePtr(nei) << " ";
//      }
//      cout << endl;
//  }

  // Second, use Prim's algorithm to form a maximum spanning tree.
  // If we construct a maximum spanning tree by the weights of the separators,
  // then the tree will satisfy running intersection property.
  set<Clique*> tree_so_far;
  tree_so_far.insert(vector_clique_ptr_container[0]); // randomly insert a clique in tree, as the start of the Prim algorithm

  while (tree_so_far.size() < vector_clique_ptr_container.size()) {
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

      max_weight_sep->is_in_jt = true;
    vector_separator_ptr_container.push_back(max_weight_sep);

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
  for (auto &sep_ptr : vector_separator_ptr_container) {
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

//void JunctionTree::CliqueMerging(int low, int high) {
//    int num_clique_old = 0;
//    int num_clique_new = vector_clique_ptr_container.size();
//
//    while (num_clique_old != num_clique_new) { // repeat until no reduction in cliques
//        for (auto it = vector_separator_ptr_container.begin(); it != vector_separator_ptr_container.end(); ) { // traverse all seps
////            cout << "sep ";
////            for (auto v: (*it)->clique_variables) {
////                cout << v << " ";
////            }
////            cout << endl;
//            auto iter = (*it)->set_neighbours_ptr.begin();
//            Clique *clq1 = *iter, *clq2 = *(++iter);
////            cout << "clq1 ";
////            for (auto v: clq1->clique_variables) {
////                cout << v << " ";
////            }
////            cout << endl;
////            cout << "clq2 ";
////            for (auto v: clq2->clique_variables) {
////                cout << v << " ";
////            }
////            cout << endl;
//            if ((clq1->clique_size < low || clq2->clique_size < low) &&
//                (clq1->clique_size + clq2->clique_size < high)) {
//                // 1. remove this sep from the neighbors of clq1 and clq2
//                clq1->set_neighbours_ptr.erase(*it);
//                clq2->set_neighbours_ptr.erase(*it);
//                // 2. remove and delete the sep
//                auto tmp = *it;
//                it = vector_separator_ptr_container.erase(it);
//                delete tmp;
////                cout << "remove sep and neighbors...." << endl;
//                // 3. create a new big clique
//                set<int> set_indexes_to_form_a_clique;
////                set<Node*> set_node_ptrs_to_form_a_clique;
//                set_indexes_to_form_a_clique.insert(clq1->clique_variables.begin(), clq1->clique_variables.end());
//                set_indexes_to_form_a_clique.insert(clq2->clique_variables.begin(), clq2->clique_variables.end());
////                for (auto index: set_indexes_to_form_a_clique) {
////                    set_node_ptrs_to_form_a_clique.insert(network->FindNodePtrByIndex(index));
////                }
////                Clique* clique = new Clique(set_node_ptrs_to_form_a_clique);
//                Clique* clique = new Clique(set_indexes_to_form_a_clique, network);
//                vector_clique_ptr_container.push_back(clique);
////                cout << "create new clique ";
////                for (auto v: clique->clique_variables) {
////                    cout << v << " ";
////                }
////                cout << endl;
//                // 4. all neighbors of clq1 and clq2 become neighbors of the new big clique, and vice versa,
//                //    and remove clq1 and clq2 from the neighbors of their neighbors
//                // note that we don't remove their neighbors from the neighbors of clq1 and clq2
//                // because to do this may cause problem of erasing and traversing at the same time
//                // and we will erase and delete clq1 and clq2 so their neighbors don't matter
//                for (auto &sep_ptr: clq1->set_neighbours_ptr) {
//                    clique->set_neighbours_ptr.insert(sep_ptr);
//                    sep_ptr->set_neighbours_ptr.insert(clique);
//                    sep_ptr->set_neighbours_ptr.erase(clq1);
//                }
//                for (auto &sep_ptr: clq2->set_neighbours_ptr) {
//                    clique->set_neighbours_ptr.insert(sep_ptr);
//                    sep_ptr->set_neighbours_ptr.insert(clique);
//                    sep_ptr->set_neighbours_ptr.erase(clq2);
//                }
//                // 5. remove and delete clq1 and clq2
//                for (auto iter = vector_clique_ptr_container.begin(); iter != vector_clique_ptr_container.end();) {
//                    if (*iter == clq1) {
//                        iter = vector_clique_ptr_container.erase(iter);
//                    } else if (*iter == clq2) {
//                        iter = vector_clique_ptr_container.erase(iter);
//                    } else {
//                        ++iter;
//                    }
//                }
//                delete clq1;
//                delete clq2;
////                cout << "update neighbor and remove clq1 clq2... num clique = " << vector_clique_ptr_container.size() << endl;
//            } else {
//                ++it;
//            }
//        }
//        num_clique_old = num_clique_new;
//        num_clique_new = vector_clique_ptr_container.size();
//    }
//}

/**
 * @brief: assign an id to each clique and separator
 */
void JunctionTree::NumberTheCliquesAndSeparators() {//checked
  int i = 0;
  for (auto c : vector_clique_ptr_container) {
    c->clique_id = i++;
  }
  int j = 0;
  for (auto s : vector_separator_ptr_container) {
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
    for (auto &pt : potential_tables) { // for each potential table of the network
        for (auto &clique_ptr : vector_clique_ptr_container) { // for each clique of the graph
            if (pt.related_variables.empty() || clique_ptr->clique_variables.empty()) {
                break;
            }
            if (!clique_ptr->pure_discrete) {
                continue;
            }

            // get the variables that in the potential table but not in the clique
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
                clique_ptr->p_table.TableMultiplication(pt); // multiply two factors
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
 * @brief: do level traverse of the tree, at the same time:
 *      1 add all the cliques & separators in "nodes_by_level" by level
 *      2 add all the separators in "seps_by_level" by level
 *      3 mark both "ptr_upstream_clique" and "ptr_downstream_cliques"
 */
void JunctionTree::MarkLevel() {
    vector<Clique*> vec; // a set of cliques/seps in one level
    vec.push_back(arb_root); // push the root into vec
    nodes_by_level.push_back(vec); // the first level only has the root clique

    while (!vec.empty()) {
        vector<Clique*> vec2;
        vector<Separator*> vsep2;
        /*
         * think of nodes...: clq, sep, clq, sep, ... clq
         * if nodes.size % 2 == 0, "vec" is sep, then vec's downstream neighbors are clqs
         */
        bool is_sep = nodes_by_level.size() % 2 == 0 ? false : true;

        for (int i = 0; i < vec.size(); ++i) { // for each clique/sep in the current level
            Clique *clique = vec[i];
            for (auto &ptr_neighbor : clique->set_neighbours_ptr) {
                // all neighbor cliques of "clique" contain the upstream clique and downstream clique(s)
                // if the current neighbor "ptr_separator" is the upstream clique, do nothing
                if (ptr_neighbor == clique->ptr_upstream_clique) {
                    continue;
                }
                // the current neighbor "ptr_separator" is a downstream clique of "clique"
                clique->ptr_downstream_cliques.push_back(ptr_neighbor);
                ptr_neighbor->ptr_upstream_clique = clique;  // Let the callee know the caller.
                vec2.push_back(ptr_neighbor);
                if (is_sep) { // cast and push to the separator vector
                    Separator* ptr_sep = dynamic_cast<Separator*>(ptr_neighbor);
                    vsep2.push_back(ptr_sep);
                }
            }
        }

        nodes_by_level.push_back(vec2);
        if (is_sep) {
            separators_by_level.push_back(vsep2);
        }
        vec = vec2;
    }

    nodes_by_level.pop_back();
    separators_by_level.pop_back();
    max_level = nodes_by_level.size();
}

/**
 * The inference process will modify the junction tree itself.
 * So, we need to backup the tree and restore it after an inference.
 * Otherwise, we need to re-construct the tree each time we want to make inference.
 */
//void JunctionTree::BackUpJunctionTree() {
//    for (const auto &c : vector_clique_ptr_container) {
//        map_cliques_backup[c] = *c;
//    }
//    for (const auto &s : vector_separator_ptr_container) {
//        map_separators_backup[s] = *s;
//    }
//}

void JunctionTree::BackUpJunctionTree() {
    clique_backup = new Clique[vector_clique_ptr_container.size()];
    separator_backup = new Separator[vector_separator_ptr_container.size()];
    int i = 0;
    for (const auto &c : vector_clique_ptr_container) {
        clique_backup[i++] = *c;
    }
    i = 0;
    for (const auto &s : vector_separator_ptr_container) {
        separator_backup[i++] = *s;
    }
}

/**
 * The inference process will modify the junction tree itself.
 * So, we need to backup the tree and restore it after an inference.
 * Otherwise, we need to re-construct the tree each time we want to make inference.
 */
//void JunctionTree::ResetJunctionTree() {
//  for (auto &c : vector_clique_ptr_container) {
//    *c = map_cliques_backup[c];
//  }
//  for (auto &s : vector_separator_ptr_container) {
//    *s = map_separators_backup[s];
//  }
//}

void JunctionTree::ResetJunctionTree() {
    int i = 0;
    for (auto &c : vector_clique_ptr_container) {
        *c = clique_backup[i++];
    }
    i = 0;
    for (auto &s : vector_separator_ptr_container) {
        *s = separator_backup[i++];
    }
}

/**
 * @brief: when inferring, an evidence is given. The evidence needs to be loaded and propagate in the network.
 */
void JunctionTree::LoadDiscreteEvidence(const DiscreteConfig &E, int num_threads, Timer *timer) {
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
        timer->Start("pre-evi");
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

        vector<Clique*> vector_all_node_ptr;
        for (auto n: vector_clique_ptr_container) {
            vector_all_node_ptr.push_back(n);
        }
        for (auto n: vector_separator_ptr_container) {
            vector_all_node_ptr.push_back(n);
        }

        // find all cliques that related to the observation and push them to "vector_red_clq"
        int size = vector_all_node_ptr.size();
        vector<int> vector_red_clq;
        vector_red_clq.reserve(size);
        for (int i = 0; i < size; ++i) {
            auto clique_ptr = vector_all_node_ptr[i];
            if (clique_ptr->p_table.related_variables.find(index) != clique_ptr->p_table.related_variables.end()) {
                vector_red_clq.push_back(i);
            }
        }

        int red_size = vector_red_clq.size();

        int *e_loc = new int[red_size];
        int **full_config = new int*[red_size];
        int **v_index = new int*[red_size];

        int *cum_sum = new int[size];
        int final_sum = 0;

        /**
         * pre-computing
         */
        omp_set_num_threads(num_threads);
#pragma omp parallel for
        for (int k = 0; k < red_size; ++k) {
            auto clique_ptr = vector_all_node_ptr[vector_red_clq[k]];

            e_loc[k] = clique_ptr->p_table.GetVariableIndex(index);
            full_config[k] = new int[clique_ptr->p_table.table_size * clique_ptr->p_table.num_variables];
            v_index[k] = new int[clique_ptr->p_table.table_size];
        }

        for (int k = 0; k < red_size; ++k) {
            auto clique_ptr = vector_all_node_ptr[vector_red_clq[k]];

            // update sum
            cum_sum[k] = final_sum;
            final_sum += clique_ptr->p_table.table_size;
        }

        timer->Stop("pre-evi");

        timer->Start("main-evi");
        // the main loop
        omp_set_num_threads(num_threads);
#pragma omp parallel for
        for (int s = 0; s < final_sum; ++s) {
            // compute k and i
            int k = -1;
            for (int m = red_size - 1; m >= 0; --m) {
                if (s >= cum_sum[m]) {
                    k = m;
                    break;
                }
            }
            int i = s - cum_sum[k];

            auto clique_ptr = vector_all_node_ptr[vector_red_clq[k]];
            // 1. get the full config value of old table
            clique_ptr->p_table.GetConfigValueByTableIndex(i, full_config[k] + i * clique_ptr->p_table.num_variables);
            // 2. get the value of the evidence variable from the new table
            v_index[k][i] = full_config[k][i * clique_ptr->p_table.num_variables + e_loc[k]];
        }
        timer->Stop("main-evi");

        timer->Start("post-evi");
        /**
         * post-computing
         */
        omp_set_num_threads(num_threads);
#pragma omp parallel for
        for (int k = 0; k < red_size; ++k) {
            auto clique_ptr = vector_all_node_ptr[vector_red_clq[k]];

            int new_size = clique_ptr->p_table.table_size / clique_ptr->p_table.var_dims[e_loc[k]];
            vector<double> new_potentials;
            new_potentials.resize(new_size);

            delete[] full_config[k];

            for (int i = 0, j = 0; i < clique_ptr->p_table.table_size; ++i) {
                // 3. whether it is consistent with the evidence
                if (v_index[k][i] == value_index) {
                    new_potentials[j++] = clique_ptr->p_table.potentials[i];
                }
            }
            clique_ptr->p_table.potentials = new_potentials;
            delete[] v_index[k];

            clique_ptr->p_table.related_variables.erase(index);
            clique_ptr->p_table.num_variables -= 1;

            if (clique_ptr->p_table.num_variables > 0) {
                vector<int> dims;
                dims.reserve(clique_ptr->p_table.num_variables);
                for (int i = 0; i < clique_ptr->p_table.num_variables + 1; ++i) {
                    if (i != e_loc[k]) {
                        dims.push_back(clique_ptr->p_table.var_dims[i]);
                    }
                }
                clique_ptr->p_table.var_dims = dims;

                clique_ptr->p_table.ConstructCumLevels();
                // table size -- number of possible configurations
                clique_ptr->p_table.table_size = new_size;
            } else {
                clique_ptr->p_table.var_dims = vector<int>();
                clique_ptr->p_table.cum_levels = vector<int>();
                clique_ptr->p_table.table_size = 1;
            }
        }

        delete[] e_loc;
        delete[] full_config;
        delete[] v_index;
        delete[] cum_sum;
        timer->Stop("post-evi");
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
void JunctionTree::MessagePassingUpdateJT(int num_threads, Timer *timer) {
//    /************************* use factor ******************************/
//  arb_root->Collect(timer);
//  arb_root->Distribute(timer);
//    /************************* use factor ******************************/

    /************************* use potential table ******************************/
    /**
     * 1. omp task
     */
//    timer->Start("upstream");
//#pragma omp parallel num_threads(num_threads)
//    {
//#pragma omp single
//        {
//            arb_root->Collect2();
////            arb_root->Collect3(nodes_by_level, max_level);
//        }
//    }
//    timer->Stop("upstream");
//
//    timer->Start("downstream");
//#pragma omp parallel num_threads(num_threads)
//    {
//#pragma omp single
//        {
//            arb_root->Distribute2();
////            arb_root->Distribute3(nodes_by_level, max_level);
//        }
//    }
//    timer->Stop("downstream");

    /**
     * 2. omp parallel for
     */
    timer->Start("upstream");
//    arb_root->Collect3(nodes_by_level, max_level, num_threads);
    Collect(num_threads, timer);
    timer->Stop("upstream");

    timer->Start("downstream");
//    arb_root->Distribute3(nodes_by_level, max_level, num_threads);
    Distribute(num_threads, timer);
    timer->Stop("downstream");
    /************************* use potential table ******************************/
}

void JunctionTree::Collect(int num_threads, Timer *timer) {
    for (int i = max_level - 2; i >= 0 ; --i) { // for each level

        if (i % 2) {
            /**
             * case 1: levels 1, 3, 5, ... are separator levels
             * collect msg from its child (a clique) to it (a separator)
             * do marginalization + division for separator levels
             */
            timer->Start("pre-up-sep");
            int size = separators_by_level[i/2].size();
            vector<PotentialTable> tmp_pt; // store all tmp pt used for table marginalization
            tmp_pt.resize(size);

            // store number_variables and cum_levels of the original table
            // rather than storing the whole potential table
            int *nv_old = new int[size];
            vector<vector<int>> cl_old;
            cl_old.resize(size);

            int *cum_sum = new int[size];
            int final_sum = 0;

            // set of arrays, showing the locations of the variables of the new table in the old table
            int **loc_in_old = new int*[size];
            int **full_config = new int*[size];
            int **partial_config = new int*[size];
            int **table_index = new int*[size];

            /**
             * pre computing
             */
            omp_set_num_threads(num_threads);
#pragma omp parallel for
            for (int j = 0; j < size; ++j) { // for each separator in this level
                auto separator = separators_by_level[i/2][j];
                auto child = separator->ptr_downstream_cliques[0]; // there is only one child for each separator

                separator->old_ptable = separator->p_table; // used for division

                // find the variables to be marginalized
                set<int> set_external_vars;
                set_difference(child->p_table.related_variables.begin(), child->p_table.related_variables.end(),
                               separator->clique_variables.begin(), separator->clique_variables.end(),
                               inserter(set_external_vars, set_external_vars.begin()));

                // store the parent's table, used for update the child's table
                nv_old[j] = child->p_table.num_variables;
                cl_old[j] = child->p_table.cum_levels;

                child->p_table.MarginalizationPre(set_external_vars, tmp_pt[j]);

                // generate an array showing the locations of the variables of the new table in the old table
                loc_in_old[j] = new int[tmp_pt[j].num_variables];
                int k = 0;
                for (auto &v: tmp_pt[j].related_variables) {
                    loc_in_old[j][k++] = child->p_table.GetVariableIndex(v);
                }
                table_index[j] = new int[child->p_table.table_size];

                // malloc in pre-, not to parallelize
                full_config[j] = new int[child->p_table.table_size * child->p_table.num_variables];
                partial_config[j] = new int[child->p_table.table_size * tmp_pt[j].num_variables];
            }

            for (int j = 0; j < size; ++j) {
                auto separator = separators_by_level[i/2][j];
                auto child = separator->ptr_downstream_cliques[0]; // there is only one child for each separator
                // update sum
                cum_sum[j] = final_sum;
                final_sum += child->p_table.table_size;
            }

            timer->Stop("pre-up-sep");

            timer->Start("main-up-sep");
            // the main loop
            omp_set_num_threads(num_threads);
#pragma omp parallel for
            for (int s = 0; s < final_sum; ++s) {
                // compute j and k
                int j = -1;
                for (int m = size - 1; m >= 0; --m) {
                    if (s >= cum_sum[m]) {
                        j = m;
                        break;
                    }
                }
                int k = s - cum_sum[j];

                // 1. get the full config value of old table
                tmp_pt[j].GetConfigValueByTableIndex(k, full_config[j] + k * nv_old[j], nv_old[j], cl_old[j]);
                // 2. get the partial config value from the old table
                for (int l = 0; l < tmp_pt[j].num_variables; ++l) {
                    partial_config[j][k * tmp_pt[j].num_variables + l] = full_config[j][k * nv_old[j] + loc_in_old[j][l]];
                }
                // 3. obtain the potential index
                table_index[j][k] = tmp_pt[j].GetTableIndexByConfigValue(partial_config[j] + k * tmp_pt[j].num_variables);
            }

            timer->Stop("main-up-sep");

            timer->Start("post-up-sep");
            // post-computing
            omp_set_num_threads(num_threads);
#pragma omp parallel for
            for (int j = 0; j < size; ++j) { // for each separator in this level
                delete[] loc_in_old[j];
                delete[] full_config[j];
                delete[] partial_config[j];

                auto separator = separators_by_level[i/2][j];
                auto child = separator->ptr_downstream_cliques[0]; // there is only one child for each separator

                for (int k = 0; k < child->p_table.table_size; ++k) {
                    // 4. potential[table_index]
                    tmp_pt[j].potentials[table_index[j][k]] += child->p_table.potentials[k];
                }
                delete[] table_index[j];

                tmp_pt[j].TableDivision(separator->old_ptable);

                separator->p_table = tmp_pt[j];
            }
            delete[] loc_in_old;
            delete[] full_config;
            delete[] partial_config;
            delete[] table_index;
            delete[] cum_sum;
            delete[] nv_old;
            timer->Stop("post-up-sep");
        }
        else {
            /**
             * case 2: levels 0, 2, 4, ... are clique levels
             * collect msg from its children (separators) to it (a clique)
             * do extension + multiplication for clique levels
             */
            timer->Start("pre-up-clq");
            int size = nodes_by_level[i].size();

            int max_num_children = 0;
            for (int j = 0; j < size; ++j) { // for each clique of this level
                /**
                 * there may be multiple children for a clique
                 * first process the first child of each clique in this level,
                 * then the second child of each clique (if has), ...
                 * until all the children of all the cliques in this level have been processed
                 */
                auto clique = nodes_by_level[i][j];
                // first, find the max number of children for this level
                if (clique->ptr_downstream_cliques.size() > max_num_children) {
                    max_num_children = clique->ptr_downstream_cliques.size();
                }
            }
            timer->Stop("pre-up-clq");

            for (int k = 0; k < max_num_children; ++k) { // process the k-th child
                timer->Start("pre-up-clq");
                // use a vector to mark which clique(s) has the k-th children
                vector<int> has_kth_child;
                has_kth_child.reserve(size);
                for (int j = 0; j < size; ++j) { // of each clique
                    auto clique = nodes_by_level[i][j];
                    if (clique->ptr_downstream_cliques.size() > k) {
                        // this clique has the k-th child
                        has_kth_child.push_back(j);
                    }
                }

                /**
                 * then, inside this loop (k), the following operations are similar to before:
                 * before: process "size" cliques in parallel, each update once
                 * now: process "process_size" cliques in parallel, each update once
                 */
                int process_size = has_kth_child.size();

                vector<PotentialTable> tmp_pt;
                tmp_pt.reserve(2 * process_size);

                vector<PotentialTable> multi_pt;
                multi_pt.resize(process_size);

                // store number_variables and cum_levels of the original table
                // rather than storing the whole potential table
                int *nv_old = new int[2 * process_size];
                vector<vector<int>> cl_old;
                cl_old.reserve(2 * process_size);

                int *cum_sum = new int[2 * process_size];
                int final_sum = 0;
                int sum_index = 0;

                // not all tables need to do the extension
                // there are "2 * process_size" tables in total
                // use a vector to show which tables need to do the extension
                vector<int> vector_extension;
                vector_extension.reserve(process_size);

                // set of arrays, showing the locations of the variables of the new table in the old table
                int **loc_in_new = new int*[2 * process_size];
                int **full_config = new int*[2 * process_size];
                int **partial_config = new int*[2 * process_size];
                int **table_index = new int*[2 * process_size];

                /**
                 * pre computing
                 */
                for (int j = 0; j < process_size; ++j) { // for each clique (has the k-th child) in this level
                    auto clique = nodes_by_level[i][has_kth_child[j]];
                    auto child = clique->ptr_downstream_cliques[k];

                    multi_pt[j] = child->p_table;

                    // pre processing for extension
                    set<int> all_related_variables;
                    set<int> diff1, diff2;
                    clique->p_table.MultiplicationPre(child->p_table, all_related_variables, diff1, diff2);

                    if (diff1.empty() && diff2.empty()) { // if both table1 and table2 should not be extended
                        // do nothing
                    } else if (!diff1.empty() && diff2.empty()) { // if table1 should be extended and table2 not
                        // record the index (that requires to do the extension)
                        vector_extension.push_back(j * 2 + 0);
                        nv_old[sum_index] = clique->p_table.num_variables;
                        cl_old.push_back(clique->p_table.cum_levels);

                        PotentialTable pt;
                        pt.ExtensionPre(all_related_variables, child->p_table.var_dims);

                        // get this table's location -- it is currently the last one
                        int last = vector_extension.size() - 1;
                        // generate an array showing the locations of the variables of the new table in the old table
                        loc_in_new[last] = new int[clique->p_table.num_variables];
                        int k = 0;
                        for (auto &v: clique->p_table.related_variables) {
                            loc_in_new[last][k++] = pt.GetVariableIndex(v);
                        }
                        table_index[last] = new int[pt.table_size];

                        tmp_pt.push_back(pt);

                        // malloc in pre-, not to parallelize
                        full_config[last] = new int[pt.table_size * pt.num_variables];
                        partial_config[last] = new int[pt.table_size * clique->p_table.num_variables];

                        // update sum
                        cum_sum[sum_index++] = final_sum;
                        final_sum += pt.table_size;
                    } else if (diff1.empty() && !diff2.empty()) { // if table2 should be extended and table1 not
                        // record the index (that requires to do the extension)
                        vector_extension.push_back(j * 2 + 1);
                        nv_old[sum_index] = child->p_table.num_variables;
                        cl_old.push_back(child->p_table.cum_levels);

                        PotentialTable pt;
                        pt.ExtensionPre(all_related_variables, clique->p_table.var_dims);

                        // get this table's location -- it is currently the last one
                        int last = vector_extension.size() - 1;
                        // generate an array showing the locations of the variables of the new table in the old table
                        loc_in_new[last] = new int[child->p_table.num_variables];
                        int k = 0;
                        for (auto &v: child->p_table.related_variables) {
                            loc_in_new[last][k++] = pt.GetVariableIndex(v);
                        }
                        table_index[last] = new int[pt.table_size];

                        tmp_pt.push_back(pt);

                        // malloc in pre-, not to parallelize
                        full_config[last] = new int[pt.table_size * pt.num_variables];
                        partial_config[last] = new int[pt.table_size * child->p_table.num_variables];

                        // update sum
                        cum_sum[sum_index++] = final_sum;
                        final_sum += pt.table_size;
                    } else { // if both table1 and table2 should be extended
                        // record the index (that requires to do the extension)
                        vector_extension.push_back(j * 2 + 0);
                        vector_extension.push_back(j * 2 + 1);
                        nv_old[sum_index] = clique->p_table.num_variables;
                        cl_old.push_back(clique->p_table.cum_levels);
                        nv_old[sum_index + 1] = child->p_table.num_variables;
                        cl_old.push_back(child->p_table.cum_levels);

                        PotentialTable tmp_pta, tmp_ptb;

                        vector<int> dims; // to save dims of the new related variables
                        dims.reserve(all_related_variables.size());
                        // to find the location of each new related variable
                        for (auto &v: all_related_variables) {
                            int loc = clique->p_table.GetVariableIndex(v);
                            if (loc < clique->p_table.related_variables.size()) { // find it in table1
                                dims.push_back(clique->p_table.var_dims[loc]);
                            } else { // cannot find in table1, we need to find it in table2
                                loc = child->p_table.GetVariableIndex(v);
                                dims.push_back(child->p_table.var_dims[loc]);
                            }
                        }

                        tmp_pta.ExtensionPre(all_related_variables, dims);
                        tmp_ptb.ExtensionPre(all_related_variables, dims);

                        // get this table's location -- it is currently the last one
                        int last = vector_extension.size() - 1;
                        // generate an array showing the locations of the variables of the new table in the old table
                        loc_in_new[last - 1] = new int[clique->p_table.num_variables];
                        int k = 0;
                        for (auto &v: clique->p_table.related_variables) {
                            loc_in_new[last][k++] = tmp_pta.GetVariableIndex(v);
                        }
                        table_index[last] = new int[tmp_pta.table_size];

                        tmp_pt.push_back(tmp_pta);

                        // malloc in pre-, not to parallelize
                        full_config[last - 1] = new int[tmp_pta.table_size * tmp_pta.num_variables];
                        partial_config[last] = new int[tmp_pta.table_size * clique->p_table.num_variables];

                        // generate an array showing the locations of the variables of the new table in the old table
                        loc_in_new[last] = new int[child->p_table.num_variables];
                        k = 0;
                        for (auto &v: child->p_table.related_variables) {
                            loc_in_new[last][k++] = tmp_ptb.GetVariableIndex(v);
                        }
                        table_index[last] = new int[tmp_ptb.table_size];

                        tmp_pt.push_back(tmp_ptb);

                        // malloc in pre-, not to parallelize
                        full_config[last] = new int[tmp_ptb.table_size * tmp_ptb.num_variables];
                        partial_config[last] = new int[tmp_ptb.table_size * child->p_table.num_variables];

                        // update sum
                        cum_sum[sum_index++] = final_sum;
                        final_sum += tmp_pta.table_size;
                        cum_sum[sum_index++] = final_sum;
                        final_sum += tmp_ptb.table_size;
                    }
                }
                timer->Stop("pre-up-clq");

                timer->Start("main-up-clq");
                int size_e = vector_extension.size(); // the number of variables to be extended
                // the main loop
                omp_set_num_threads(num_threads);
#pragma omp parallel for
                for (int s = 0; s < final_sum; ++s) {
                    // compute j and k
                    int j = -1;
                    for (int m = size_e - 1; m >= 0; --m) {
                        if (s >= cum_sum[m]) {
                            j = m;
                            break;
                        }
                    }
                    int k = s - cum_sum[j];

                    // 1. get the full config value of new table
                    tmp_pt[j].GetConfigValueByTableIndex(k, full_config[j] + k * tmp_pt[j].num_variables);
                    // 2. get the partial config value from the new table
                    for (int l = 0; l < nv_old[j]; ++l) {
                        partial_config[j][k * nv_old[j] + l] = full_config[j][k * tmp_pt[j].num_variables + loc_in_new[j][l]];
                    }
                    // 3. obtain the potential index
                    table_index[j][k] = tmp_pt[j].GetTableIndexByConfigValue(partial_config[j] + k * nv_old[j], nv_old[j], cl_old[j]);
                }
                timer->Stop("main-up-clq");

                timer->Start("post-up-clq");
                // post-computing
                int l = 0;
                for (int j = 0; j < process_size; ++j) {
                    auto clique = nodes_by_level[i][has_kth_child[j]];
                    auto child = clique->ptr_downstream_cliques[k];

                    int m = j * 2 + 0;
                    if (l < size_e && m == vector_extension[l]) { // index k have done the extension
                        delete[] loc_in_new[l];
                        delete[] full_config[l];
                        delete[] partial_config[l];

                        for (int k = 0; k < tmp_pt[l].table_size; ++k) {
                            // 4. potential[table_index]
                            tmp_pt[l].potentials[k] = clique->p_table.potentials[table_index[l][k]];
                        }
                        delete[] table_index[l];

                        clique->p_table = tmp_pt[l];

                        l++;
                    }

                    m = j * 2 + 1;
                    if (l < size_e && m == vector_extension[l]) { // index j have done the extension
                        delete[] loc_in_new[l];
                        delete[] full_config[l];
                        delete[] partial_config[l];

                        for (int k = 0; k < tmp_pt[l].table_size; ++k) {
                            // 4. potential[table_index]
                            tmp_pt[l].potentials[k] = child->p_table.potentials[table_index[l][k]];
                        }
                        delete[] table_index[l];

                        multi_pt[j] = tmp_pt[l];

                        l++;
                    }
                }

                delete[] loc_in_new;
                delete[] full_config;
                delete[] partial_config;
                delete[] table_index;
                delete[] cum_sum;
                delete[] nv_old;

                for (int j = 0; j < process_size; ++j) {
                    for (int k = 0; k < multi_pt[j].table_size; ++k) {
                        nodes_by_level[i][has_kth_child[j]]->p_table.potentials[k] *= multi_pt[j].potentials[k];
                    }
                }
                timer->Stop("post-up-clq");
            }
        }
    }
}

void JunctionTree::Distribute(int num_threads, Timer *timer) {
    for (int i = 1; i < max_level; ++i) { // for each level

        if (i % 2) {
            /**
             * case 1: levels 1, 3, 5, ... are separator levels
             * distribute msg from its parent (a clique) to it (a separator)
             * do marginalization + division for separator levels
             */
            timer->Start("pre-down-sep");
            int size = separators_by_level[i/2].size();
            vector<PotentialTable> tmp_pt; // store all tmp pt used for table marginalization
            tmp_pt.resize(size);

            // store number_variables and cum_levels of the original table
            // rather than storing the whole potential table
            int *nv_old = new int[size];
            vector<vector<int>> cl_old;
            cl_old.resize(size);

            int *cum_sum = new int[size];
            int final_sum = 0;

            // set of arrays, showing the locations of the variables of the new table in the old table
            int **loc_in_old = new int*[size];
            int **full_config = new int*[size];
            int **partial_config = new int*[size];
            int **table_index = new int*[size];

            /**
             * pre computing
             */
            omp_set_num_threads(num_threads);
#pragma omp parallel for
            for (int j = 0; j < size; ++j) { // for each separator in this level
                auto separator = separators_by_level[i/2][j];
                auto par = separator->ptr_upstream_clique;

                separator->old_ptable = separator->p_table; // used for division

                // find the variables to be marginalized
                set<int> set_external_vars;
                set_difference(par->p_table.related_variables.begin(), par->p_table.related_variables.end(),
                               separator->clique_variables.begin(), separator->clique_variables.end(),
                               inserter(set_external_vars, set_external_vars.begin()));

                // store the parent's table, used for update the child's table
                nv_old[j] = par->p_table.num_variables;
                cl_old[j] = par->p_table.cum_levels;

                par->p_table.MarginalizationPre(set_external_vars, tmp_pt[j]);

                // generate an array showing the locations of the variables of the new table in the old table
                loc_in_old[j] = new int[tmp_pt[j].num_variables];
                int k = 0;
                for (auto &v: tmp_pt[j].related_variables) {
                    loc_in_old[j][k++] = par->p_table.GetVariableIndex(v);
                }
                table_index[j] = new int[par->p_table.table_size];

                // malloc in pre-, not to parallelize
                full_config[j] = new int[par->p_table.table_size * par->p_table.num_variables];
                partial_config[j] = new int[par->p_table.table_size * tmp_pt[j].num_variables];
            }

            for (int j = 0; j < size; ++j) {
                auto separator = separators_by_level[i/2][j];
                auto par = separator->ptr_upstream_clique;
                // update sum
                cum_sum[j] = final_sum;
                final_sum += par->p_table.table_size;
            }

            timer->Stop("pre-down-sep");

            timer->Start("main-down-sep");
            // the main loop
            omp_set_num_threads(num_threads);
#pragma omp parallel for
            for (int s = 0; s < final_sum; ++s) {
                // compute j and k
                int j = -1;
                for (int m = size - 1; m >= 0; --m) {
                    if (s >= cum_sum[m]) {
                        j = m;
                        break;
                    }
                }
                int k = s - cum_sum[j];

                // 1. get the full config value of old table
                tmp_pt[j].GetConfigValueByTableIndex(k, full_config[j] + k * nv_old[j], nv_old[j], cl_old[j]);
                // 2. get the partial config value from the old table
                for (int l = 0; l < tmp_pt[j].num_variables; ++l) {
                    partial_config[j][k * tmp_pt[j].num_variables + l] = full_config[j][k * nv_old[j] + loc_in_old[j][l]];
                }
                // 3. obtain the potential index
                table_index[j][k] = tmp_pt[j].GetTableIndexByConfigValue(partial_config[j] + k * tmp_pt[j].num_variables);
            }

            timer->Stop("main-down-sep");

            timer->Start("post-down-sep");
            // post-computing
            omp_set_num_threads(num_threads);
#pragma omp parallel for
            for (int j = 0; j < size; ++j) { // for each separator in this level
                timer->Start("post-down-sep-mem");
                auto separator = separators_by_level[i/2][j];
                auto par = separator->ptr_upstream_clique;

                for (int k = 0; k < par->p_table.table_size; ++k) {
                    // 4. potential[table_index]
                    tmp_pt[j].potentials[table_index[j][k]] += par->p_table.potentials[k];
                }
                timer->Stop("post-down-sep-mem");

                timer->Start("post-down-sep-del");
                delete[] loc_in_old[j];
                delete[] full_config[j];
                delete[] partial_config[j];
                delete[] table_index[j];
                timer->Stop("post-down-sep-del");

                timer->Start("post-down-sep-div");
                tmp_pt[j].TableDivision(separator->old_ptable);
                separator->p_table = tmp_pt[j];
                timer->Stop("post-down-sep-div");
            }
            timer->Start("post-down-sep-del");
            delete[] loc_in_old;
            delete[] full_config;
            delete[] partial_config;
            delete[] table_index;
            delete[] cum_sum;
            delete[] nv_old;
            timer->Stop("post-down-sep-del");
            timer->Stop("post-down-sep");
        }
        else {
            /**
             * case 2: levels 0, 2, 4,... are clique levels
             * distribute msg from its parent (a separator) to it (a clique)
             * do extension + multiplication for clique levels
             */
            timer->Start("pre-down-clq");
            int size = nodes_by_level[i].size();

            vector<PotentialTable> tmp_pt;
            tmp_pt.reserve(2 * size);

            vector<PotentialTable> multi_pt;
            multi_pt.resize(size);

            // store number_variables and cum_levels of the original table
            // rather than storing the whole potential table
            int *nv_old = new int[2 * size];
            vector<vector<int>> cl_old;
            cl_old.reserve(2 * size);

            int *cum_sum = new int[2 * size];
            int final_sum = 0;
            int sum_index = 0;

            // not all tables need to do the extension
            // there are "2 * size" tables in total
            // use a vector to show which tables need to do the extension
            vector<int> vector_extension;
            vector_extension.reserve(size);

            // set of arrays, showing the locations of the variables of the new table in the old table
            int **loc_in_new = new int*[2 * size];
            int **full_config = new int*[2 * size];
            int **partial_config = new int*[2 * size];
            int **table_index = new int*[2 * size];

            /**
             * pre computing
             */
            for (int j = 0; j < size; ++j) { // for each clique in this level
                auto clique = nodes_by_level[i][j];
                auto par = clique->ptr_upstream_clique;

                multi_pt[j] = par->p_table;

                // pre processing for extension
                set<int> all_related_variables;
                set<int> diff1, diff2;
                clique->p_table.MultiplicationPre(par->p_table, all_related_variables, diff1, diff2);

                if (diff1.empty() && diff2.empty()) { // if both table1 and table2 should not be extended
                    // do nothing
                } else if (!diff1.empty() && diff2.empty()) { // if table1 should be extended and table2 not
                    // record the index (that requires to do the extension)
                    vector_extension.push_back(j * 2 + 0);
                    nv_old[sum_index] = clique->p_table.num_variables;
                    cl_old.push_back(clique->p_table.cum_levels);

                    PotentialTable pt;
                    pt.ExtensionPre(all_related_variables, par->p_table.var_dims);

                    // get this table's location -- it is currently the last one
                    int last = vector_extension.size() - 1;
                    // generate an array showing the locations of the variables of the new table in the old table
                    loc_in_new[last] = new int[clique->p_table.num_variables];
                    int k = 0;
                    for (auto &v: clique->p_table.related_variables) {
                        loc_in_new[last][k++] = pt.GetVariableIndex(v);
                    }
                    table_index[last] = new int[pt.table_size];

                    tmp_pt.push_back(pt);

                    // malloc in pre-, not to parallelize
                    full_config[last] = new int[pt.table_size * pt.num_variables];
                    partial_config[last] = new int[pt.table_size * clique->p_table.num_variables];

                    // update sum
                    cum_sum[sum_index++] = final_sum;
                    final_sum += pt.table_size;
                } else if (diff1.empty() && !diff2.empty()) { // if table2 should be extended and table1 not
                    // record the index (that requires to do the extension)
                    vector_extension.push_back(j * 2 + 1);
                    nv_old[sum_index] = par->p_table.num_variables;
                    cl_old.push_back(par->p_table.cum_levels);

                    PotentialTable pt;
                    pt.ExtensionPre(all_related_variables, clique->p_table.var_dims);

                    // get this table's location -- it is currently the last one
                    int last = vector_extension.size() - 1;
                    // generate an array showing the locations of the variables of the new table in the old table
                    loc_in_new[last] = new int[par->p_table.num_variables];
                    int k = 0;
                    for (auto &v: par->p_table.related_variables) {
                        loc_in_new[last][k++] = pt.GetVariableIndex(v);
                    }
                    table_index[last] = new int[pt.table_size];

                    tmp_pt.push_back(pt);

                    // malloc in pre-, not to parallelize
                    full_config[last] = new int[pt.table_size * pt.num_variables];
                    partial_config[last] = new int[pt.table_size * par->p_table.num_variables];

                    // update sum
                    cum_sum[sum_index++] = final_sum;
                    final_sum += pt.table_size;
                } else { // if both table1 and table2 should be extended
                    // record the index (that requires to do the extension)
                    vector_extension.push_back(j * 2 + 0);
                    vector_extension.push_back(j * 2 + 1);
                    nv_old[sum_index] = clique->p_table.num_variables;
                    cl_old.push_back(clique->p_table.cum_levels);
                    nv_old[sum_index + 1] = par->p_table.num_variables;
                    cl_old.push_back(par->p_table.cum_levels);

                    PotentialTable tmp_pta, tmp_ptb;

                    vector<int> dims; // to save dims of the new related variables
                    dims.reserve(all_related_variables.size());
                    // to find the location of each new related variable
                    for (auto &v: all_related_variables) {
                        int loc = clique->p_table.GetVariableIndex(v);
                        if (loc < clique->p_table.related_variables.size()) { // find it in table1
                            dims.push_back(clique->p_table.var_dims[loc]);
                        } else { // cannot find in table1, we need to find it in table2
                            loc = par->p_table.GetVariableIndex(v);
                            dims.push_back(par->p_table.var_dims[loc]);
                        }
                    }

                    tmp_pta.ExtensionPre(all_related_variables, dims);
                    tmp_ptb.ExtensionPre(all_related_variables, dims);

                    // get this table's location -- it is currently the last one
                    int last = vector_extension.size() - 1;
                    // generate an array showing the locations of the variables of the new table in the old table
                    loc_in_new[last - 1] = new int[clique->p_table.num_variables];
                    int k = 0;
                    for (auto &v: clique->p_table.related_variables) {
                        loc_in_new[last][k++] = tmp_pta.GetVariableIndex(v);
                    }
                    table_index[last] = new int[tmp_pta.table_size];

                    tmp_pt.push_back(tmp_pta);

                    // malloc in pre-, not to parallelize
                    full_config[last - 1] = new int[tmp_pta.table_size * tmp_pta.num_variables];
                    partial_config[last] = new int[tmp_pta.table_size * clique->p_table.num_variables];

                    // generate an array showing the locations of the variables of the new table in the old table
                    loc_in_new[last] = new int[par->p_table.num_variables];
                    k = 0;
                    for (auto &v: par->p_table.related_variables) {
                        loc_in_new[last][k++] = tmp_ptb.GetVariableIndex(v);
                    }
                    table_index[last] = new int[tmp_ptb.table_size];

                    tmp_pt.push_back(tmp_ptb);

                    // malloc in pre-, not to parallelize
                    full_config[last] = new int[tmp_ptb.table_size * tmp_ptb.num_variables];
                    partial_config[last] = new int[tmp_ptb.table_size * par->p_table.num_variables];

                    // update sum
                    cum_sum[sum_index++] = final_sum;
                    final_sum += tmp_pta.table_size;
                    cum_sum[sum_index++] = final_sum;
                    final_sum += tmp_ptb.table_size;
                }
            }
            timer->Stop("pre-down-clq");

            timer->Start("main-down-clq");
            int size_e = vector_extension.size(); // the number of variables to be extended
            // the main loop
            omp_set_num_threads(num_threads);
#pragma omp parallel for
            for (int s = 0; s < final_sum; ++s) {
                // compute j and k
                int j = -1;
                for (int m = size_e - 1; m >= 0; --m) {
                    if (s >= cum_sum[m]) {
                        j = m;
                        break;
                    }
                }
                int k = s - cum_sum[j];

                // 1. get the full config value of new table
                tmp_pt[j].GetConfigValueByTableIndex(k, full_config[j] + k * tmp_pt[j].num_variables);
                // 2. get the partial config value from the new table
                for (int l = 0; l < nv_old[j]; ++l) {
                    partial_config[j][k * nv_old[j] + l] = full_config[j][k * tmp_pt[j].num_variables + loc_in_new[j][l]];
                }
                // 3. obtain the potential index
                table_index[j][k] = tmp_pt[j].GetTableIndexByConfigValue(partial_config[j] + k * nv_old[j], nv_old[j], cl_old[j]);
            }
            timer->Stop("main-down-clq");

            timer->Start("post-down-clq");
            // post-computing
            timer->Start("post-down-clq-mem");
            int l = 0;
            for (int j = 0; j < size; ++j) {
                auto clique = nodes_by_level[i][j];
                auto par = clique->ptr_upstream_clique;

                int m = j * 2 + 0;
                if (l < size_e && m == vector_extension[l]) { // index k have done the extension
                    for (int k = 0; k < tmp_pt[l].table_size; ++k) {
                        // 4. potential[table_index]
                        tmp_pt[l].potentials[k] = clique->p_table.potentials[table_index[l][k]];
                    }
                    clique->p_table = tmp_pt[l];
                    l++;
                }

                m = j * 2 + 1;
                if (l < size_e && m == vector_extension[l]) { // index j have done the extension
                    for (int k = 0; k < tmp_pt[l].table_size; ++k) {
                        // 4. potential[table_index]
                        tmp_pt[l].potentials[k] = par->p_table.potentials[table_index[l][k]];
                    }
                    multi_pt[j] = tmp_pt[l];
                    l++;
                }
            }
            timer->Stop("post-down-clq-mem");

            timer->Start("post-down-clq-del");
            for (int l = 0; l < size_e; ++l) {
                delete[] loc_in_new[l];
                delete[] full_config[l];
                delete[] partial_config[l];
                delete[] table_index[l];
            }

            delete[] loc_in_new;
            delete[] full_config;
            delete[] partial_config;
            delete[] table_index;
            delete[] cum_sum;
            delete[] nv_old;
            timer->Stop("post-down-clq-del");

            timer->Start("post-down-clq-mul");
//            omp_set_num_threads(num_threads);
//#pragma omp parallel for
            for (int j = 0; j < size; ++j) {
                for (int k = 0; k < multi_pt[j].table_size; ++k) {
                    nodes_by_level[i][j]->p_table.potentials[k] *= multi_pt[j].potentials[k];
                }
            }
            timer->Stop("post-down-clq-mul");

            timer->Stop("post-down-clq");
        }
    }
}

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
  for (auto &c : vector_clique_ptr_container) {
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
    for (auto &c : vector_clique_ptr_container) {
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

//    cout << "select clique: " << selected_clique->clique_id << ": ";
//    for (auto v: selected_clique->p_table.related_variables) {
//        cout << v << " ";
//    }
//    cout << endl;

    set<int> other_vars = selected_clique->p_table.related_variables;
    other_vars.erase(query_index);

//    cout << "other variables: ";
//    for (auto v: other_vars) {
//        cout << v << " ";
//    }
//    cout << endl;

    PotentialTable pt = selected_clique->p_table;
//    cout << "table: " << endl;
//    for (int j = 0; j < pt.potentials.size(); ++j) {
//        cout << pt.potentials[j] << endl;
//    }

//    for (auto &index : other_vars) {
//        pt.TableMarginalization(index);
//    }
    pt.TableMarginalization(other_vars);
//    cout << "table after marginalization: " << endl;
//    for (int j = 0; j < pt.potentials.size(); ++j) {
//        cout << pt.potentials[j] << endl;
//    }

    pt.Normalize(); // todo: no need to do normalization
//    cout << "table after norm: " << endl;
//    for (int j = 0; j < pt.potentials.size(); ++j) {
//        cout << pt.potentials[j] << endl;
//    }

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
//    cout << "max_prob = " << max_prob << ", max_index = " << max_index << endl;

    // "pt" has only one related variable, which is exactly the query variable,
    // so the "max_index" exactly means which value of the query variable gets the max probability
    auto dn = dynamic_cast<DiscreteNode*>(network->FindNodePtrByIndex(query_index));
    int label_predict = dn->vec_potential_vals.at(max_index);
//    cout << "vec potential vals: ";
//    for (int i = 0; i < dn->vec_potential_vals.size(); ++i) {
//        cout << dn->vec_potential_vals[i] << " ";
//    }
//    cout << endl;
//    cout << "return predict = " << label_predict << endl;
    /************************* use potential table ******************************/

  return label_predict;
}

/**
 * @brief: test the Junction Tree given a data set
 */
double JunctionTree::EvaluateAccuracy(Dataset *dts, int num_threads, int num_samp, string alg, bool is_dense) {

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
    vector<int> predictions = PredictUseJTInfer(evidences, class_var_index, num_threads, timer);

    double accuracy = Accuracy(ground_truths, predictions);
//    cout << "result: ";
//    for (int i = 0; i < predictions.size(); ++i) {
//        cout << predictions[i] << " ";
//    }
//    cout << endl;

    timer->Stop("jt");
    setlocale(LC_NUMERIC, "");

//    double accuracy = num_of_correct / (double)(num_of_correct+num_of_wrong);
    cout << '\n' << "Accuracy: " << accuracy << endl;
    cout << "==================================================";
    timer->Print("jt");
    timer->Print("load evidence"); cout << " (" << timer->time["load evidence"] / timer->time["jt"] * 100 << "%)";
    timer->Print("msg passing"); cout << " (" << timer->time["msg passing"] / timer->time["jt"] * 100 << "%)";
    timer->Print("upstream");
    timer->Print("downstream");
    timer->Print("predict"); cout << " (" << timer->time["predict"] / timer->time["jt"] * 100 << "%)";
    timer->Print("reset"); cout << " (" << timer->time["reset"] / timer->time["jt"] * 100 << "%)" << endl;
    timer->Print("pre-evi");
    timer->Print("main-evi");
    timer->Print("post-evi"); cout << endl;
    timer->Print("pre-down-sep");
    timer->Print("main-down-sep");
    timer->Print("post-down-sep");
    timer->Print("pre-down-clq");
    timer->Print("main-down-clq");
    timer->Print("post-down-clq"); cout << endl;
    timer->Print("pre-up-sep");
    timer->Print("main-up-sep");
    timer->Print("post-up-sep");
    timer->Print("pre-up-clq");
    timer->Print("main-up-clq");
    timer->Print("post-up-clq"); cout << endl;
    timer->Print("post-down-clq-mem");
    timer->Print("post-down-clq-del");
    timer->Print("post-down-clq-mul");
    timer->Print("post-down-sep-mem");
    timer->Print("post-down-sep-del");
    timer->Print("post-down-sep-div"); cout << endl;

    delete timer;
    timer = nullptr;

    return accuracy;
}

/**
 * @brief: predict label given evidence E and target variable id Y_index
 * @return label of the target variable
 */
int JunctionTree::PredictUseJTInfer(const DiscreteConfig &E, int Y_index, int num_threads, Timer *timer) {
    timer->Start("load evidence");
    //update a clique using the evidence
    LoadDiscreteEvidence(E, num_threads, timer);
    timer->Stop("load evidence");
//    cout << "finish load evidence" << endl;

//    cout << "cliques: " << endl;
//    for (auto &c : vector_clique_ptr_container) {
//        cout << c->clique_id << ": ";
//        // set<int> related_variables
//        for (auto &v : c->p_table.related_variables) {
//            cout << v << " ";
//        }
//        cout << endl;
//        // vector<double> potentials
//        for (int j = 0; j < c->p_table.potentials.size(); ++j) {
//            cout << c->p_table.potentials[j] << " ";
//        }
//        cout << endl;
//    }

    timer->Start("msg passing");
    //update the whole Junction Tree
    MessagePassingUpdateJT(num_threads, timer);
    timer->Stop("msg passing");
//    cout << "finish msg passing" << endl;

//    cout << "cliques: " << endl;
//    for (auto &c : vector_clique_ptr_container) {
//        cout << c->clique_id << ": ";
//        // set<int> related_variables
//        for (auto &v : c->p_table.related_variables) {
//            cout << v << " ";
//        }
//        cout << endl;
//        // vector<double> potentials
//        for (int j = 0; j < c->p_table.potentials.size(); ++j) {
//            cout << c->p_table.potentials[j] << " ";
//        }
//        cout << endl;
//    }

    timer->Start("predict");
    int label_predict = InferenceUsingBeliefPropagation(Y_index);
    timer->Stop("predict");
//    cout << "finish predict " << endl;

    timer->Start("reset");
    ResetJunctionTree();
    timer->Stop("reset");
//    cout << "finish reset" << endl;
    return label_predict;
}

/**
 * @brief: predict the labels given different evidences
 * it just repeats the function above multiple times, and print the progress at the meantime
 * @param elim_orders: elimination order which may be different given different evidences due to the simplification of elimination order
 */
vector<int> JunctionTree::PredictUseJTInfer(const vector<DiscreteConfig> &evidences, int target_node_idx,
                                            int num_threads, Timer *timer) {
    int size = evidences.size();

    cout << "Progress indicator: ";
    int every_1_of_20 = size / 20; // used to print, print 20 times in total
    int progress = 0;

    vector<int> results(size, 0);

    for (int i = 0; i < size; ++i) {
//        cout << i << endl;
        ++progress;

        if (progress % every_1_of_20 == 0) {
            string progress_percentage = to_string((double)progress/size * 100) + "%...\n";
            fprintf(stdout, "%s\n", progress_percentage.c_str());
//            double acc_so_far = num_of_correct / (double)(num_of_correct+num_of_wrong);
//            fprintf(stdout, "Accuracy so far: %f\n", acc_so_far);
            fflush(stdout);
        }

        int label_predict = PredictUseJTInfer(evidences.at(i), target_node_idx, num_threads, timer);
        results.at(i) = label_predict;
    }
    return results;
}