#include "Clique.h"

Clique::Clique() {
  is_separator = false;
  clique_id = -1;
  clique_size = -1;
  pure_discrete = true;
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
    is_separator = false;
    clique_size = set_node_index.size();
    pure_discrete = true;

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


/**
 * @brief: potential is similar to weight, and can be used to compute probability.
 */
void Clique::PreInitializePotentials() {
  if (pure_discrete) {
    for (auto &c : table.set_disc_configs) {
      table.map_potentials[c] = 1;  // Initialize clique potential to be 1.
    }
  } else {
    // Initialize lp_potential and post_bag to be empty. That is, do NOTHING.
  }
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
/**
 * 1. omp task
 */
void Clique::Collect3(vector<vector<Clique*>> &cliques, int max_level) {
    for (int i = max_level - 2; i >= 0 ; --i) { // for each level
        for (int j = 0; j < cliques[i].size(); ++j) { // for each clique of this level
#pragma omp task shared(cliques)
            {
                for (auto &ptr_child : cliques[i][j]->ptr_downstream_cliques) {
//                    cliques[i][j]->UpdateUseMessage2(ptr_child->p_table);
//                    cliques[i][j]->ConstructMessage2();
                    cliques[i][j]->UpdateMessage(ptr_child->p_table);
                }
            }
        }
#pragma omp taskwait
    }
}

/**
 * 2. omp parallel for
 */
void Clique::Collect3(vector<vector<Clique*>> &cliques, int max_level, int num_threads, Timer *timer) {
    for (int i = max_level - 2; i >= 0 ; --i) { // for each level
//        omp_set_num_threads(num_threads);
//#pragma omp parallel for
        for (int j = 0; j < cliques[i].size(); ++j) { // for each clique of this level
            for (auto &ptr_child : cliques[i][j]->ptr_downstream_cliques) {
//                cliques[i][j]->UpdateUseMessage2(ptr_child->p_table);
//                cliques[i][j]->ConstructMessage2();
                cliques[i][j]->UpdateMessage(ptr_child->p_table);
            }
        }

        /**
        * there are some issues with datasets munin2, munin3, munin4
        * after debugging -- caused by table multiplication
        * don't have enough precision so it may cause 0 prob after multiplication
        * therefore, I add a normalization after collection of each level
        * we can remove this part for other datasets
        */
        timer->Start("norm");
        omp_set_num_threads(num_threads);
#pragma omp parallel for
        for (int j = 0; j < cliques[i].size(); ++j) {
            cliques[i][j]->p_table.Normalize();
        }
        timer->Stop("norm");
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
/**
 * 1. omp task
 */
void Clique::Distribute3(vector<vector<Clique*>> &cliques, int max_level) {
    for (int i = 1; i < max_level; ++i) { // for each level
        for (int j = 0; j < cliques[i].size(); ++j) { // for each clique in this level
                auto clique = cliques[i][j];
                auto par = clique->ptr_upstream_clique;
#pragma omp task // TODO: need test
            {
                clique->UpdateMessage(par->p_table);
            }
        }
#pragma omp taskwait
    }

//    for (int i = 0; i < max_level - 1; ++i) {
//        for (int j = 0; j < cliques[i].size(); ++j) {
//            Clique *clique = cliques[i][j];
//            for (auto &ptr_child : clique->ptr_downstream_cliques) {
//#pragma omp task shared(ptr_child)
//                {
//                    // Prepare message for the downstream.
//                    ptr_child->UpdateMessage(clique->p_table);
//                }
//            }
//        }
//#pragma omp taskwait
//    }
}

/**
 * 2. omp parallel for
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

//void Clique::UpdateUseMessage2(const PotentialTable &pt) {
//    PotentialTable tmp_pt = pt;
//    p_table.TableMultiplication(tmp_pt); // multiply two factors
//}
//
///**
// * @brief: construct a factor of this clique and return
// */
//void Clique::ConstructMessage2() {
//    // do nothing
//    return;
//}

/**
 * merging the above two methods
 */
void Clique::UpdateMessage(const PotentialTable &pt) {
    PotentialTable tmp_pt = pt;
    p_table.TableMultiplication(tmp_pt); // multiply two factors
}

//void Clique::PrintPotentials() const {
//  if (pure_discrete) {
//    for (const auto &potentials_key_value : table.map_potentials) {
//      for (const auto &vars_index_value : potentials_key_value.first) {
//        cout << '(' << vars_index_value.first << ',' << vars_index_value.second << ") ";
//      }
//      cout << "\t: " << potentials_key_value.second << endl;
//    }
//  } else {
//    fprintf(stderr, "%s not implemented for continuous clique yet!", __FUNCTION__);
//    exit(1);
//    // todo: implement
//  }
//  cout << "----------" << endl;
//}

//void Clique::PrintRelatedVars() const {
//  string out = "{  ";
//  for (const auto &v : table.related_variables) {
//    if (v==elimination_variable_index) {
//      out += "[" + to_string(v) + "]";
//    } else {
//      out += to_string(v);
//    }
//    out += "  ";
//  }
//  out += "}";
//  cout << out << endl;
//}