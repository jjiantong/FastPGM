#include "Clique.h"

Clique::Clique() {
    ptr_upstream_clique = nullptr;
}

/*!
 * @brief: constructor, given node indexes of the clique
 * in the original implementation, the order of "var_dims" is not right
 * because the node ptrs in the set ("set_node_ptr") is not ordered by the index of the nodes
 * at the beginning, we used data set "a1a" where every feature and label has the same number of possible values
 * so we did not find the bug at the beginning...
 * @current: we still need the node ptr, so we add parameter "net" to find node ptr by index
 * TODO: only support (pure) discrete cases now
 */
Clique::Clique(set<int> set_node_index, Network *net) : clique_variables(set_node_index) {
    /**
     * potential table
     * an object of class PotentialTable is a class variable of class Clique ("p_table")
     * it will call the default constructor of class PotentialTable in the construction of a Clique object
     * so I cannot let it call another defined PotentialTable constructor
     * so I use a method in class PotentialTable instead
     * TODO: maybe consider to use a pointer instead of an object?
     */
    p_table.ConstructEmptyPotentialTable(set_node_index, net);

    ptr_upstream_clique = nullptr;
}


/*!
 * @brief: a step of msg passing (alg1, use recursive functions)
 * msg passes from downstream to upstream
 * first collect the msgs from its downstream neighbors;
 * then update the msg by multiplying its initial potential with all msgs received from its downstream neighbors
 * (initial potential of a cluster/node is constructed via the product of factors that assigned to it)
 */
void Clique::Collect2() {
    for (auto &ptr_child : ptr_downstream_cliques) {
        /** when it reaches a leaf, the only neighbour is the upstream,
         * which can be viewed as the base case of recursive function.
         */
#pragma omp task shared(ptr_child)
        {
            // collect the msg f from downstream
            ptr_child->Collect2();
            PotentialTable pt = ptr_child->p_table;
            // update the msg by multiplying the current table with pt
            UpdateMessage(pt);
        }
    }
#pragma omp taskwait
}

/*!
 * @brief: a step of msg passing (alg2, avoid recursive functions)
 * msg passes from downstream to upstream
 * traverse the tree from leaves to root, by levels
 * for each level, update the cliques (separators) of the level according to their children
 * computations inside each level can be parallelized
 * @param cliques: all cliques (separators) in the junction tree, ordered by levels
 * @param max_level: the max level of the junction tree (cliques and separators are all included)
 */
void Clique::Collect3(vector<vector<Clique*>> &cliques, int max_level, int num_threads, Timer *timer) {
    for (int i = max_level - 2; i >= 0 ; --i) { // for each level
//        omp_set_num_threads(num_threads);
//#pragma omp parallel for
        for (int j = 0; j < cliques[i].size(); ++j) { // for each clique of this level
            for (auto &ptr_child : cliques[i][j]->ptr_downstream_cliques) {
                cliques[i][j]->UpdateMessage(ptr_child->p_table);
            }
        }
    }
}


/**
 * @brief: a step of msg passing in clique trees (alg1, use recursive functions)
 * distribute the information it knows to the downstream cliques; the msg passes from upstream to downstream
 * first update the msg; then distribute the msgs to its downstream neighbors;
 */

void Clique::Distribute2() {
    for (auto &sep : ptr_downstream_cliques) {
        sep->Distribute2(p_table);
    }
}

void Clique::Distribute2(PotentialTable &pt) {
    UpdateMessage(pt);

    for (auto &ptr_child : ptr_downstream_cliques) {
#pragma omp task shared(ptr_child)
        {
            // distribute the msg to downstream
            ptr_child->Distribute2(p_table); // Distribute to downstream.
        }
    }
}

/**
 * @brief: a step of msg passing in clique trees (alg2, avoid recursive functions)
 * msg passes from upstream to downstream
 * use "while" loop and a queue instead of using recursive function (originally)
 * first push the root to the queue; then consecutively pop a clique from the queue and handle it until the queue is empty (originally)
 * in the improved version, use a vector, not a queue, each time handle the whole vector,
 * because the handling order of the cliques in the same level does not matter
 * in the handling process, first update the msg; then construct the msg and push the clique to the queue
 */
void Clique::Distribute3(vector<vector<Clique*>> &cliques, int max_level, int num_threads) {
    for (int i = 1; i < max_level; ++i) { // for each level
//        omp_set_num_threads(num_threads);
//#pragma omp parallel for
        for (int j = 0; j < cliques[i].size(); ++j) { // for each clique in this level
            auto clique = cliques[i][j];
            auto par = clique->ptr_upstream_clique;
            clique->UpdateMessage(par->p_table);
        }
    }
}


/**
 * merging the above two methods
 */
void Clique::UpdateMessage(const PotentialTable &pt) {
    PotentialTable tmp_pt = pt;
    p_table.TableMultiplication(tmp_pt); // multiply two factors

    p_table.Normalize();

}
