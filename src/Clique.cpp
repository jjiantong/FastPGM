//
// Created by LinjianLi on 2019/2/16.
//

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
        //initialize related variables
        clique_variables.insert(n->GetNodeIndex());
        if (n->is_discrete) {
            auto dn = dynamic_cast<DiscreteNode*>(n);
            p_table.var_dims.push_back(dn->GetDomainSize());
        }
    }

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

/*!
 * @brief: a step of msg passing
 * msg passes from downstream to upstream
 * first collect the msgs from its downstream neighbors;
 * then update the msg by multiplying its initial potential with all msgs received from its downstream neighbors
 * (initial potential of a cluster/node is constructed via the product of factors that assigned to it)
 * @return a msg, which is a factor
 */
Factor Clique::Collect(Timer *timer) {

  for (auto &ptr_separator : set_neighbours_ptr) {

    /** when it reaches a leaf, the only neighbour is the upstream,
     * which can be viewed as the base case of recursive function.
     */
    // all neighbor cliques contain the upstream clique and downstream clique(s)
    // if the current neighbor "ptr_separator" is the upstream clique, not collect from it
    // otherwise, collect the msg from "ptr_separator" and update the msg
    if (ptr_separator == ptr_upstream_clique) {
      continue;
    }

    // the current neighbor "ptr_separator" is a downstream clique
    ptr_separator->ptr_upstream_clique = this;  // Let the callee know the caller.

//    /** This is for continuous nodes TODO: double-check
//     * If the next clique connected by this separator is a continuous clique,
//     * then the program should not collect from it. All information needed from
//     * the continuous clique has been pushed to the boundary when entering the evidence.
//     */
//    bool reach_boundary = false;
//    for (const auto &next_clq_ptr : ptr_separator->set_neighbours_ptr) {
//        reach_boundary = (next_clq_ptr->ptr_upstream_clique!=ptr_separator && !next_clq_ptr->pure_discrete);
//    }
//    if (reach_boundary) { continue; }

    // collect the msg f from downstream
    Factor f = ptr_separator->Collect(timer);
    // update the msg by multiplying the current factor with f
    // the current factor is the initial potential, or
    // the product of the initial potential and factors received from other downstream neighbors

    UpdateUseMessage(f, timer);  // Update itself.
  }

  // Prepare message for the upstream.
  ConstructMessage(timer);
  return table;
}

/*!
 * @brief: a step of msg passing
 * msg passes from downstream to upstream
 * first collect the msgs from its downstream neighbors;
 * then update the msg by multiplying its initial potential with all msgs received from its downstream neighbors
 * (initial potential of a cluster/node is constructed via the product of factors that assigned to it)
 * @return a msg, which is a factor
 */
PotentialTable Clique::Collect2(Timer *timer) {
    for (auto &ptr_separator : set_neighbours_ptr) {
        /** when it reaches a leaf, the only neighbour is the upstream,
         * which can be viewed as the base case of recursive function.
         */
        // all neighbor cliques contain the upstream clique and downstream clique(s)
        // if the current neighbor "ptr_separator" is the upstream clique, not collect from it
        // otherwise, collect the msg from "ptr_separator" and update the msg
        if (ptr_separator == ptr_upstream_clique) {
            continue;
        }

        // the current neighbor "ptr_separator" is a downstream clique
        ptr_separator->ptr_upstream_clique = this;  // Let the callee know the caller.

        // collect the msg f from downstream
        PotentialTable pt = ptr_separator->Collect2(timer);
        // update the msg by multiplying the current factor with f
        // the current factor is the initial potential, or
        // the product of the initial potential and factors received from other downstream neighbors

        UpdateUseMessage2(pt, timer);  // Update itself.
    }

    // Prepare message for the upstream.
    ConstructMessage2(timer);
    return p_table;
}

/**
 * Distribute the information it knows to the downstream cliques.
 * The reload version without parameter. Called on the selected root.
 */
void Clique::Distribute(Timer *timer) {
    ConstructMessage(timer);
    for (auto &sep : set_neighbours_ptr) {
        sep->Distribute(table, timer);
    }
}

/**
 * Distribute the information it knows to the downstream cliques.
 * The reload version without parameter. Called on the selected root.
 */
void Clique::Distribute2(Timer *timer) {
    ConstructMessage2(timer);
    for (auto &sep : set_neighbours_ptr) {
        sep->Distribute2(p_table, timer);
    }
}

/**
 * @brief: a step of msg passing in clique trees
 * distribute the information it knows to the downstream cliques; the msg passes from upstream to downstream
 * first update the msg; then distribute the msgs to its downstream neighbors;
 * @param f is in fact a factor received from its upstream clique
 * @return a msg, which is a factor
 * The reload version with parameter. Called by recursion.
 */
void Clique::Distribute(Factor &f, Timer *timer) {
  // If the next clique connected by this separator is a continuous clique,
  // then the program should not distribute information to it.// TODO: double-check
//  bool reach_boundary = false;
//  for (const auto &next_clq_ptr : set_neighbours_ptr) {
//    reach_boundary = !next_clq_ptr->pure_discrete;
//  }
//  if (reach_boundary) { return; }

  // update the msg by multiplying the current factor with f
  UpdateUseMessage(f, timer);  // Update itself.

  // Prepare message for the downstream.
  ConstructMessage(timer);

  for (auto &ptr_separator : set_neighbours_ptr) {

    // all neighbor cliques contain the upstream clique and downstream clique(s)
    // if the current neighbor "ptr_separator" is the upstream clique, not distribute to it
    // otherwise, distribute the msg to "ptr_separator"
    if (ptr_separator == ptr_upstream_clique) {
      continue;
    }

    // the current neighbor "ptr_separator" is a downstream clique
    ptr_separator->ptr_upstream_clique = this;  // Let the callee know the caller.
    // distribute the msg to downstream
    ptr_separator->Distribute(table, timer); // Distribute to downstream.
  }
}

/**
 * @brief: a step of msg passing in clique trees
 * distribute the information it knows to the downstream cliques; the msg passes from upstream to downstream
 * first update the msg; then distribute the msgs to its downstream neighbors;
 * @param f is in fact a factor received from its upstream clique
 * @return a msg, which is a factor
 * The reload version with parameter. Called by recursion.
 */
void Clique::Distribute2(PotentialTable &pt, Timer *timer) {
    // If the next clique connected by this separator is a continuous clique,
    // then the program should not distribute information to it.// TODO: double-check
//  bool reach_boundary = false;
//  for (const auto &next_clq_ptr : set_neighbours_ptr) {
//    reach_boundary = !next_clq_ptr->pure_discrete;
//  }
//  if (reach_boundary) { return; }

    // update the msg by multiplying the current factor with f
    UpdateUseMessage2(pt, timer);  // Update itself.

    // Prepare message for the downstream.
    ConstructMessage2(timer);

    for (auto &ptr_separator : set_neighbours_ptr) {

        // all neighbor cliques contain the upstream clique and downstream clique(s)
        // if the current neighbor "ptr_separator" is the upstream clique, not distribute to it
        // otherwise, distribute the msg to "ptr_separator"
        if (ptr_separator == ptr_upstream_clique) {
            continue;
        }

        // the current neighbor "ptr_separator" is a downstream clique
        ptr_separator->ptr_upstream_clique = this;  // Let the callee know the caller.
        // distribute the msg to downstream
        ptr_separator->Distribute2(p_table, timer); // Distribute to downstream.
    }
}

/**
 * @brief: sum over external variables which are the results of factor multiplication.
 */
void Clique::SumOutExternalVars(Factor &f, Timer *timer) {
//    timer->Start("set_difference");
    // get the variables that in "f" but not in "factor_of_this_clique"
    set<int> set_external_vars;
    set_difference(f.related_variables.begin(), f.related_variables.end(),
                   this->clique_variables.begin(), this->clique_variables.end(),
                   inserter(set_external_vars, set_external_vars.begin()));
//    timer->Stop("set_difference");

//    timer->Start("factor marginalization");
    // Sum over the variables that are not in the scope of this clique/separator, so as to eliminate them.
    for (auto &ex_vars : set_external_vars) {
        f = f.SumOverVar(ex_vars);
    }
//    timer->Stop("factor marginalization");
}

/**
 * @brief: sum over external variables which are the results of factor multiplication.
 */
void Clique::SumOutExternalVars(PotentialTable &pt, Timer *timer) {
//    timer->Start("set_difference");

//    cout << "  sum out from ";
//    for (auto &v: pt.related_variables) {
//        cout << v << " ";
//    }
//    cout << endl;

    // get the variables that in "f" but not in "factor_of_this_clique"
    set<int> set_external_vars;
    set_difference(pt.related_variables.begin(), pt.related_variables.end(),
                   this->clique_variables.begin(), this->clique_variables.end(),
                   inserter(set_external_vars, set_external_vars.begin()));
//    timer->Stop("set_difference");

//    cout << "  external vars: ";
//    for (auto &v: set_external_vars) {
//        cout << v << " ";
//    }
//    cout << endl;

//    timer->Start("factor marginalization");
    // Sum over the variables that are not in the scope of this clique/separator, so as to eliminate them.
    for (auto &ex_vars : set_external_vars) {
//        cout << "  sum out " << ex_vars << endl;
        pt.TableMarginalization(ex_vars);
    }
//    timer->Stop("factor marginalization");
}

/**
 * @brief: multiply a clique with a factor
 */
void Clique::MultiplyWithFactorSumOverExternalVars(Factor &f, Timer *timer) {
    // sum over the irrelevant variables of the clique
    SumOutExternalVars(f, timer);

    // in the original implementation, "related_variables" is always all the variables in the clique,
    // "set_disc_configs" is always all the configurations of the variables in the clique,
    // so they are not required to be changed, the only thing changed is the "map_potentials".
    // for the current implementation, all "related_variables", "set_disc_configs" and "map_potentials" are reduced if possible,
    // so they all need to be changed here.
    // at the same time, the original implementation copy a new factor of the clique, use the copy to compute,
    // and then copy back the "map_potentials", which is not efficient...
//    timer->Start("factor multiplication");
    table = table.MultiplyWithFactor(f); // multiply two factors
//    timer->Stop("factor multiplication");
}

/**
 * @brief: multiply a clique with a factor
 */
void Clique::MultiplyWithFactorSumOverExternalVars(PotentialTable &pt, Timer *timer) {
    // sum over the irrelevant variables of the clique
    SumOutExternalVars(pt, timer);

    // in the original implementation, "related_variables" is always all the variables in the clique,
    // "set_disc_configs" is always all the configurations of the variables in the clique,
    // so they are not required to be changed, the only thing changed is the "map_potentials".
    // for the current implementation, all "related_variables", "set_disc_configs" and "map_potentials" are reduced if possible,
    // so they all need to be changed here.
    // at the same time, the original implementation copy a new factor of the clique, use the copy to compute,
    // and then copy back the "map_potentials", which is not efficient...
//    timer->Start("factor multiplication");
    p_table.TableMultiplication(pt); // multiply two factors
//    timer->Stop("factor multiplication");
}

void Clique::UpdateUseMessage(Factor &f, Timer *timer) {
//    timer->Start("update clique");
    MultiplyWithFactorSumOverExternalVars(f, timer);
//    timer->Stop("update clique");
}

void Clique::UpdateUseMessage2(PotentialTable &pt, Timer *timer) {
//    timer->Start("update clique");
//    cout << "construct msg of clique ";
//    for (auto &v: this->p_table.related_variables) {
//        cout << v << " ";
//    }
//    cout << " use msg ";
//    for (auto &v: pt.related_variables) {
//        cout << v << " ";
//    }
//    cout << endl;
//    cout << "tables: " << endl;
//    for (int i = 0; i < this->p_table.table_size; ++i) {
//        cout << this->p_table.potentials[i] << " ";
//    }
//    cout << endl;
//    for (int i = 0; i < pt.table_size; ++i) {
//        cout << pt.potentials[i] << " ";
//    }
//    cout << endl;
    MultiplyWithFactorSumOverExternalVars(pt, timer);
//    timer->Stop("update clique");
}

/**
 * @brief: construct a factor of this clique and return
 */
void Clique::ConstructMessage(Timer *timer) {
    // do nothing
    return;
}

/**
 * @brief: construct a factor of this clique and return
 */
void Clique::ConstructMessage2(Timer *timer) {
    // do nothing
    return;
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