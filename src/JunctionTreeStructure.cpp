//
// Created by jjt on 12/09/22.
// checked on 16/10/23.
//

#include "JunctionTreeStructure.h"

/**
 * @brief: convert BN into a junction tree.
 * following the original paper of the junction tree algorithm:
 *      Local Computations with Probabilities on Graphical Structures and their Application to Expert Systems, 1988.
 * this is an optimized constructor; the original implementation is removed. because we optimize triangulation, some
 * parameters, thus some functions, are unnecessary
 */
JunctionTreeStructure::JunctionTreeStructure(Network *net) : network(net) {

    vector_clique_ptr_container.reserve(network->num_nodes);
    vector_separator_ptr_container.reserve(network->num_nodes - 1);

    int **direc_adjac_matrix = network->ConvertDAGToAdjacencyMatrix();
//    cout << "Finish ConvertDAGNetworkToAdjacencyMatrix" << endl;

    Moralize(direc_adjac_matrix, network->num_nodes);
    int **moral_graph_adjac_matrix = direc_adjac_matrix;
//    cout << "Finish Moralize" << endl;

    vector<bool> has_processed(network->num_nodes);
    Triangulate(network, moral_graph_adjac_matrix, has_processed);
    for (int i = 0; i < network->num_nodes; ++i) {
        SAFE_DELETE_ARRAY(direc_adjac_matrix[i]);
    }
    SAFE_DELETE_ARRAY(direc_adjac_matrix);
//    cout << "Finish Triangulate, number of cliques = " << vector_clique_ptr_container.size() << endl;
//    for (int i = 0; i < vector_clique_ptr_container.size(); ++i) {
//        cout << i << ": ";
//        Clique* clq = vector_clique_ptr_container[i];
//        for (const int &v: clq->clique_variables) {
//            cout << v << ", ";
//        }
//        cout << endl;
//    }

    FormJunctionTree();//for discrete nodes
//    cout << "Finish FormJunctionTree, number of cliques = " << vector_clique_ptr_container.size()
//         << ", number of separators = " << vector_separator_ptr_container.size() << endl;

    AssignPotentials();
//    cout << "Finish AssignPotentials" << endl;

    /**
     * it is only to compute mean of clique size
     */
    int max_size = 0;
    float mean_size = GetAveAndMaxCliqueSize(max_size);
//    cout << "Finish GetAveAndMaxCliqueSize, ave clique size = " << mean_size << ", max size = " << max_size << endl;
}

JunctionTreeStructure::~JunctionTreeStructure() {
    // delete cliques and separators
    for (auto it = vector_clique_ptr_container.begin(); it != vector_clique_ptr_container.end(); it++) {
        SAFE_DELETE(*it);
    }
    for (auto it = vector_separator_ptr_container.begin(); it != vector_separator_ptr_container.end(); it++) {
        SAFE_DELETE(*it);
    }
}

/**
 * @brief: moralization -> moral graph
 * 1. connect all the parents of each node (connect all the v-structure)
 * 2. directed graph -> undirected graph
 */
void JunctionTreeStructure::Moralize(int **direc_adjac_matrix, int &num_nodes) {
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
        SAFE_DELETE(cg);
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

/**
 * @brief: triangulation -> induced graph
 * definition: no loops of length > 3 without a "bridge"/"chord"
 * one way to construct a induced graph: select a node based on the "elim_ord", connect all its neighbors, and remove this node;
 * the selected node should try to minimize the added edges, so the "elim_ord" may according to the number or neighbors
 * (if we connect two nodes if they appeared in the same factor in a run of the VE algorithm, we can exactly get a induced graph)
 * in this function, the purpose is to generate a clique while constructing the induced graph
 *
 * @note: this is an improved implementation - each time find the node with fewest neighbors to process
 *        the original implementation (removed) uses a fixed order generated before, according to the number of neighbors
 */
void JunctionTreeStructure::Triangulate(Network *net, int **adjac_matrix, vector<bool> &has_processed) {
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

    // Form a clique that contains
    for (int neighbor = 0; neighbor < vec_neighbors.size(); ++neighbor) {
        for (int neighbor2 = neighbor + 1; neighbor2 < vec_neighbors.size(); ++neighbor2) {
            adjac_matrix[vec_neighbors.at(neighbor)][vec_neighbors.at(neighbor2)] = 1;
            adjac_matrix[vec_neighbors.at(neighbor2)][vec_neighbors.at(neighbor)] = 1;
        }
        set_node_indexes_to_form_a_clique.insert(vec_neighbors.at(neighbor));
    }

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

    Triangulate(net, adjac_matrix, has_processed);
}

/**
 * @brief: construct a tree where each node is a clique and each edge is a separator.
 * use Prim algorithm; the weights of edges is represented by the weights of the separators
 */
void JunctionTreeStructure::FormJunctionTree() {

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
                if (max_weight_sep==nullptr || max_weight_sep->p_table.num_variables < sep_ptr->p_table.num_variables) {
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
            SAFE_DELETE(tmp);
        } else {
            it++;
        }
    }
}

/**
 * @brief: each clique has a potential;
 * the potentials of continuous and discrete cliques are computed differently
 */
void JunctionTreeStructure::AssignPotentials() { //checked

    vector<PotentialTableBase> potential_tables;

    for (auto &id_node_ptr : network->map_idx_node_ptr) { // for each node of the network
        auto node_ptr = id_node_ptr.second;
        if (node_ptr->is_discrete) {
            // add the factor that consists of this node and its parents
            potential_tables.push_back(PotentialTableBase(dynamic_cast<DiscreteNode*>(node_ptr), this->network));
        }
    }

    // For potentials from discrete nodes, they should be assigned to purely discrete cliques.
    for (auto &pt : potential_tables) { // for each potential table of the network
        for (auto &clique_ptr : vector_clique_ptr_container) { // for each clique of the graph
            if (pt.vec_related_variables.empty() || clique_ptr->clique_variables.empty()) {
                break;
            }

            // get the variables that in the potential table but not in the clique
            vector<int> related_variables = pt.vec_related_variables;
            sort(related_variables.begin(), related_variables.end());
            set<int> diff;
            set_difference(related_variables.begin(), related_variables.end(),
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
}

/**
 * @brief: get the average and max of clique size
 * by traversing all cliques and get the their clique sizes
 */
float JunctionTreeStructure::GetAveAndMaxCliqueSize(int &max_size) {
    int total_size = 0;
    int num_clique = vector_clique_ptr_container.size();
    for (int i = 0; i < num_clique; ++i) {
        // for each clique
        total_size += vector_clique_ptr_container[i]->p_table.table_size;
        if (vector_clique_ptr_container[i]->p_table.table_size > max_size) {
            max_size = vector_clique_ptr_container[i]->p_table.table_size;
        }
    }
    return ((float)total_size/(float)num_clique);
}