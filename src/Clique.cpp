#include "Clique.h"

Clique::Clique() {
  is_separator = false;
  clique_id = -1;
//  elimination_variable_index = -1;
  clique_size = -1;
  pure_discrete = true;
  ptr_upstream_clique = nullptr;
  activeflag = false;
}

/*!
 * @brief: constructor, given node ptrs of the clique
 * @bug: the node ptrs in the set ("set_node_ptr") is not ordered by the index of the nodes
 *       so the order of "var_dims" is not right using this constructor (TODO)
 *       at the beginning, we used data set "a1a" where every feature and label has the same number of possible values
 *       so we did not find the bug at the beginning...
 * use the next constructor, given node indexes of the clique
 */
Clique::Clique(set<Node*> set_node_ptr) {
//Clique::Clique(set<Node*> set_node_ptr, int elim_var_index) {
  is_separator = false;
//  elimination_variable_index = elim_var_index;
  clique_size = set_node_ptr.size();

  pure_discrete = true;
  for (const auto &n_p : set_node_ptr) {
    if (!n_p->is_discrete) {
      pure_discrete = false;
      break;
    }
  }

  // In the paper, all continuous cliques' activeflags are initially set to true.
  activeflag = !pure_discrete;

//    /************************* use factor ******************************/
//  // set_of_sets: set< set< pair<int, int> > >:
//  //    one set is all possible values of one node(variable),
//  //    set of sets is all possible values of all nodes.
//  // c: set< pair<int, int> >
//  set<DiscreteConfig> set_of_sets;
//  for (auto &n : set_node_ptr) { // for each node
//    //initialize related variables
//      clique_variables.insert(n->GetNodeIndex()); // insert into related_variables
//    if (n->is_discrete) {
//      auto dn = dynamic_cast<DiscreteNode*>(n);
//      DiscreteConfig c; // multiple groups: [node id, all possible values of this node]
//      for (int i = 0; i < dn->GetDomainSize(); ++i) {
//        c.insert(pair<int, int>(n->GetNodeIndex(), dn->vec_potential_vals.at(i)));
//      }
//      set_of_sets.insert(c);
//    }
//  }
//
//    table.related_variables = clique_variables;
//    table.set_disc_configs = GenAllCombinationsFromSets(&set_of_sets);
//    PreInitializePotentials();
//    /************************* use factor ******************************/

    /************************* use potential table ******************************/
    // potential table
    p_table.num_variables = clique_size;
    p_table.var_dims.reserve(clique_size);
    for (auto &n : set_node_ptr) { // for each node
        cout << "operate " << n << ", ";
        //initialize related variables
        clique_variables.insert(n->GetNodeIndex());
        cout << "insert to clique variables - " << n->GetNodeIndex() << ", ";
        if (n->is_discrete) {
            auto dn = dynamic_cast<DiscreteNode*>(n);
            p_table.var_dims.push_back(dn->GetDomainSize());
            cout << "insert to dims - " << dn->GetDomainSize() << endl;
        }
    }
    cout << "finally dims = ";
    for (int i = 0; i < p_table.var_dims.size(); ++i) {
        cout << p_table.var_dims[i] << " ";
    }
    cout << endl;

    p_table.cum_levels.resize(clique_size);
    // set the right-most one ...
    p_table.cum_levels[clique_size - 1] = 1;
    // ... then compute the left ones
    for (int i = clique_size - 2; i >= 0; --i) {
        p_table.cum_levels[i] = p_table.cum_levels[i + 1] * p_table.var_dims[i + 1];
    }
    // compute the table size -- number of possible configurations
    p_table.table_size = p_table.cum_levels[0] * p_table.var_dims[0];

    p_table.potentials.reserve(p_table.table_size);
    for (int i = 0; i < p_table.table_size; ++i) {
        p_table.potentials.push_back(1);
    }

    p_table.related_variables = clique_variables;
    /************************* use potential table ******************************/

    ptr_upstream_clique = nullptr;
}

/*!
 * @brief: constructor, given node indexes of the clique
 * use this constructor, the reason is shown in the last constructor
 * we still need the node ptr, so we add parameter "net" to find node ptr by index
 * TODO: only support (pure) discrete cases now
 */
Clique::Clique(set<int> set_node_index, Network *net) {
    is_separator = false;
    clique_size = set_node_index.size();
    pure_discrete = true;

//    /************************* use factor ******************************/
//  // set_of_sets: set< set< pair<int, int> > >:
//  //    one set is all possible values of one node(variable),
//  //    set of sets is all possible values of all nodes.
//  // c: set< pair<int, int> >
//  set<DiscreteConfig> set_of_sets;
//  clique_variables = set_node_index;
//  for (auto &n : set_node_index) { // for each node
//      Node *node_ptr = net->FindNodePtrByIndex(n);
//      auto dn = dynamic_cast<DiscreteNode*>(node_ptr);
//      DiscreteConfig c; // multiple groups: [node id, all possible values of this node]
//      for (int i = 0; i < dn->GetDomainSize(); ++i) {
//        c.insert(pair<int, int>(n, dn->vec_potential_vals.at(i)));
//      set_of_sets.insert(c);
//    }
//  }
//
//    table.related_variables = set_node_index;
//    table.set_disc_configs = GenAllCombinationsFromSets(&set_of_sets);
//    PreInitializePotentials();
//    /************************* use factor ******************************/

    /************************* use potential table ******************************/
    clique_variables = set_node_index;

    /**
     * potential table
     * an object of class PotentialTable is a class variable of class Clique ("p_table")
     * it will call the default constructor of class PotentialTable in the construction of a Clique object
     * so I cannot let it call another defined PotentialTable constructor
     * so I use a method in class PotentialTable instead
     * TODO: maybe consider to use a pointer instead of an object?
     */
    p_table.ConstructEmptyPotentialTable(set_node_index, net);
    /************************* use potential table ******************************/

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

///**
// * @brief: copy the clique by ignoring the pointer members
// */
//Clique* Clique::CopyWithoutPtr() {
//  auto c = new Clique(*this);
//  c->set_neighbours_ptr.clear();
//  c->ptr_upstream_clique = nullptr;
//  return c;
//}


///************************* use factor ******************************/
///*!
// * @brief: a step of msg passing
// * msg passes from downstream to upstream
// * first collect the msgs from its downstream neighbors;
// * then update the msg by multiplying its initial potential with all msgs received from its downstream neighbors
// * (initial potential of a cluster/node is constructed via the product of factors that assigned to it)
// * @return a msg, which is a factor
// */
//void Clique::Collect(Timer *timer) {
//
//  for (auto &ptr_separator : set_neighbours_ptr) {
//
//    /** when it reaches a leaf, the only neighbour is the upstream,
//     * which can be viewed as the base case of recursive function.
//     */
//    // all neighbor cliques contain the upstream clique and downstream clique(s)
//    // if the current neighbor "ptr_separator" is the upstream clique, not collect from it
//    // otherwise, collect the msg from "ptr_separator" and update the msg
//    if (ptr_separator == ptr_upstream_clique) {
//      continue;
//    }
//
//    // the current neighbor "ptr_separator" is a downstream clique
//    ptr_separator->ptr_upstream_clique = this;  // Let the callee know the caller.
//
////    /** This is for continuous nodes TODO: double-check
////     * If the next clique connected by this separator is a continuous clique,
////     * then the program should not collect from it. All information needed from
////     * the continuous clique has been pushed to the boundary when entering the evidence.
////     */
////    bool reach_boundary = false;
////    for (const auto &next_clq_ptr : ptr_separator->set_neighbours_ptr) {
////        reach_boundary = (next_clq_ptr->ptr_upstream_clique!=ptr_separator && !next_clq_ptr->pure_discrete);
////    }
////    if (reach_boundary) { continue; }
//
//    // collect the msg f from downstream
//      ptr_separator->Collect(timer);
//      Factor f = ptr_separator->table;
//    // update the msg by multiplying the current factor with f
//    // the current factor is the initial potential, or
//    // the product of the initial potential and factors received from other downstream neighbors
//
//    UpdateUseMessage(f, timer);  // Update itself.
//  }
//
//  // Prepare message for the upstream.
//  ConstructMessage(timer);
//}
///************************* use factor ******************************/


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

///************************* use factor ******************************/
///**
// * Distribute the information it knows to the downstream cliques.
// * The reload version without parameter. Called on the selected root.
// */
//void Clique::Distribute(Timer *timer) {
//    ConstructMessage(timer);
//    for (auto &sep : set_neighbours_ptr) {
//        sep->Distribute(table, timer);
//    }
//}

//void Clique::Distribute(Factor &f, Timer *timer) {
//  // If the next clique connected by this separator is a continuous clique,
//  // then the program should not distribute information to it.// TODO: double-check
////  bool reach_boundary = false;
////  for (const auto &next_clq_ptr : set_neighbours_ptr) {
////    reach_boundary = !next_clq_ptr->pure_discrete;
////  }
////  if (reach_boundary) { return; }
//
//  // update the msg by multiplying the current factor with f
//  UpdateUseMessage(f, timer);  // Update itself.
//
//  // Prepare message for the downstream.
//  ConstructMessage(timer);
//
//  for (auto &ptr_separator : set_neighbours_ptr) {
//
//    // all neighbor cliques contain the upstream clique and downstream clique(s)
//    // if the current neighbor "ptr_separator" is the upstream clique, not distribute to it
//    // otherwise, distribute the msg to "ptr_separator"
//    if (ptr_separator == ptr_upstream_clique) {
//      continue;
//    }
//
//    // the current neighbor "ptr_separator" is a downstream clique
//    ptr_separator->ptr_upstream_clique = this;  // Let the callee know the caller.
//    // distribute the msg to downstream
//    ptr_separator->Distribute(table, timer); // Distribute to downstream.
//  }
//}
///************************* use factor ******************************/

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
 * @brief: do level traverse of the tree, add all the cliques in "cliques" by level at the same time
 * @param cliques: storing all the cliques by level; e.g. cliques[i][j] contains one clique in level i
 * @param max_level: total number of levels
 * @note1: separators also included
 * @note2: mark both "ptr_upstream_clique" and "ptr_downstream_cliques" at the same time
 */
void Clique::MarkLevel(vector<vector<Clique*>> &cliques, int &max_level) {
    vector<Clique*> vec; // a set of cliques in one level
    vec.push_back(this); // push the root into vec
    cliques.push_back(vec); // the first level only has the root clique

    while (!vec.empty()) {
        vector<Clique*> vec2;
        for (int i = 0; i < vec.size(); ++i) { // for each clique in the current level
            Clique *clique = vec[i];
            for (auto &ptr_separator : clique->set_neighbours_ptr) {
                // all neighbor cliques of "clique" contain the upstream clique and downstream clique(s)
                // if the current neighbor "ptr_separator" is the upstream clique, do nothing
                if (ptr_separator == clique->ptr_upstream_clique) {
                    continue;
                }
                // the current neighbor "ptr_separator" is a downstream clique of "clique"
                clique->ptr_downstream_cliques.push_back(ptr_separator);
                ptr_separator->ptr_upstream_clique = clique;  // Let the callee know the caller.
                vec2.push_back(ptr_separator);
            }
        }
        cliques.push_back(vec2);
        vec = vec2;
    }

    cliques.pop_back();
    max_level = cliques.size();
}

///************************* use factor ******************************/
///**
// * @brief: sum over external variables which are the results of factor multiplication.
// */
//void Clique::SumOutExternalVars(Factor &f, Timer *timer) {
//    // get the variables that in "f" but not in "factor_of_this_clique"
//    set<int> set_external_vars;
//    set_difference(f.related_variables.begin(), f.related_variables.end(),
//                   this->clique_variables.begin(), this->clique_variables.end(),
//                   inserter(set_external_vars, set_external_vars.begin()));
//
//    // Sum over the variables that are not in the scope of this clique/separator, so as to eliminate them.
//    for (auto &ex_vars : set_external_vars) {
//        f = f.SumOverVar(ex_vars);
//    }
//}
///************************* use factor ******************************/

/**
 * @brief: sum over external variables which are the results of factor multiplication.
 */
//void Clique::SumOutExternalVars(PotentialTable &pt) {
//    // get the variables that in "f" but not in "factor_of_this_clique"
//    set<int> set_external_vars;
//    set_difference(pt.related_variables.begin(), pt.related_variables.end(),
//                   this->clique_variables.begin(), this->clique_variables.end(),
//                   inserter(set_external_vars, set_external_vars.begin()));
//
//    // Sum over the variables that are not in the scope of this clique/separator, so as to eliminate them.
//    for (auto &ex_vars : set_external_vars) {
//        pt.TableMarginalization(ex_vars);
//    }
//}

///************************* use factor ******************************/
///**
// * @brief: multiply a clique with a factor
// */
//void Clique::MultiplyWithFactorSumOverExternalVars(Factor &f, Timer *timer) {
//    // sum over the irrelevant variables of the clique todo: no need to do sum out
//    SumOutExternalVars(f, timer);
//
//    // in the original implementation, "related_variables" is always all the variables in the clique,
//    // "set_disc_configs" is always all the configurations of the variables in the clique,
//    // so they are not required to be changed, the only thing changed is the "map_potentials".
//    // for the current implementation, all "related_variables", "set_disc_configs" and "map_potentials" are reduced if possible,
//    // so they all need to be changed here.
//    // at the same time, the original implementation copy a new factor of the clique, use the copy to compute,
//    // and then copy back the "map_potentials", which is not efficient...
//    table = table.MultiplyWithFactor(f); // multiply two factors
//}
//
//
//void Clique::UpdateUseMessage(const Factor &f, Timer *timer) {
//    Factor tmp_f = f;
//    MultiplyWithFactorSumOverExternalVars(tmp_f, timer);
//}
//
///**
// * @brief: construct a factor of this clique and return
// */
//void Clique::ConstructMessage(Timer *timer) {
//    // do nothing
//    return;
//}
///************************* use factor ******************************/

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