//
// Created by LinjianLi on 2019/2/16.
//

#include "Clique.h"

Clique::Clique() {
  is_separator = false;
  clique_id = -1;
  elimination_variable_index = -1;
  clique_size = -1;
  pure_discrete = true;
  ptr_upstream_clique = nullptr;
  activeflag = false;
}

Clique::Clique(set<Node*> set_node_ptr, int elim_var_index) {
  is_separator = false;
  elimination_variable_index = elim_var_index;
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

  // set_of_sets: set< set< pair<int, int> > >:
  //    one set is all possible values of one node(variable),
  //    set of sets is all possible values of all nodes.
  // c: set< pair<int, int> >
  set<DiscreteConfig> set_of_sets;
  for (auto &n : set_node_ptr) { // for each node
    //initialize related variables
    related_variables.insert(n->GetNodeIndex()); // insert into related_variables
    if (n->is_discrete) {
      auto dn = dynamic_cast<DiscreteNode*>(n);
      DiscreteConfig c; // multiple groups: [node id, all possible values of this node]
      for (int i = 0; i < dn->GetDomainSize(); ++i) {
        c.insert(pair<int, int>(n->GetNodeIndex(), dn->vec_potential_vals.at(i)));
      }
      set_of_sets.insert(c);
    }
  }

  set_disc_configs = GenAllCombinationsFromSets(&set_of_sets);

  PreInitializePotentials();

  ptr_upstream_clique = nullptr;
}

/**
 * @brief: potential is similar to weight, and can be used to compute probability.
 */
void Clique::PreInitializePotentials() {
  if (pure_discrete) {
    for (auto &c : set_disc_configs) {
      map_potentials[c] = 1;  // Initialize clique potential to be 1.
    }
  } else {
    // Initialize lp_potential and post_bag to be empty. That is, do NOTHING.
  }
}

/**
 * @brief: copy the clique by ignoring the pointer members
 */
Clique* Clique::CopyWithoutPtr() {
  auto c = new Clique(*this);
  c->set_neighbours_ptr.clear();
  c->ptr_upstream_clique = nullptr;
  return c;
}

/*!
 * @brief: a step of msg passing
 * msg passes from downstream to upstream
 * first collect the msgs from its downstream neighbors;
 * then update the msg by multiplying its initial potential with all msgs received from its downstream neighbors
 * (initial potential of a cluster/node is constructed via the product of factors that assigned to it)
 * @return a msg, which is a factor
 */
Factor Clique::Collect() {

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
    Factor f = ptr_separator->Collect();
    // update the msg by multiplying the current factor with f
    // the current factor is the initial potential, or
    // the product of the initial potential and factors received from other downstream neighbors
    UpdateUseMessage(f);  // Update itself.
  }

  // Prepare message for the upstream.
  return ConstructMessage();
}

/**
 * Distribute the information it knows to the downstream cliques. // TODO: to all neighbors include downstream and upstream cliques?
 * The reload version without parameter. Called on the selected root.
 */
 // TODO: double-check
void Clique::Distribute() {
  Factor f = ConstructMessage();
  for (auto &sep : set_neighbours_ptr) {
    sep->Distribute(f);
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
void Clique::Distribute(Factor f) {
  // If the next clique connected by this separator is a continuous clique,
  // then the program should not distribute information to it.// TODO: double-check
  bool reach_boundary = false;
  for (const auto &next_clq_ptr : set_neighbours_ptr) {
    reach_boundary = !next_clq_ptr->pure_discrete;
  }
  if (reach_boundary) { return; }

  // update the msg by multiplying the current factor with f
  UpdateUseMessage(f);  // Update itself.

  // Prepare message for the downstream.
  Factor distribute_factor = ConstructMessage();

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
    ptr_separator->Distribute(distribute_factor); // Distribute to downstream.

  }
}

/**
 * @brief: sum over external variables which are the results of factor multiplication.
 */
Factor Clique::SumOutExternalVars(Factor f) {
  Factor factor_of_this_clique(this->related_variables,
                                   this->set_disc_configs,
                                   this->map_potentials);

  // get the variables that in "f" but not in "factor_of_this_clique"
  set<int> set_external_vars;
  set_difference(f.related_variables.begin(), f.related_variables.end(),
                 factor_of_this_clique.related_variables.begin(), factor_of_this_clique.related_variables.end(),
                 inserter(set_external_vars, set_external_vars.begin()));

  // Sum over the variables that are not in the scope of this clique/separator, so as to eliminate them.
  for (auto &ex_vars : set_external_vars) {
    f = f.SumOverVar(ex_vars);
  }
  return f;
}

/**
 * @brief: multiply a clique with a factor
 */
void Clique::MultiplyWithFactorSumOverExternalVars(Factor f) {
    // TODO: check the usage of "MultiplyWithFactorSumOverExternalVars",
    //  for the usage in assigning factors, no need to "SumOutExternalVars",
    //  because the scope of clique can accommodate the scope of its factors
    // sum over the irrelevant variables of the clique
  f = SumOutExternalVars(f);

  Factor factor_of_this_clique(related_variables, set_disc_configs, map_potentials); // the factor of the clique
  // TODO: see the comments below: "related_variables" and "set_disc_configs" are not required to be changed,
  //  there are some differences between the standard multiplication of factors and the multiplication here
  //  1. "related_variables" is no need to be changed
  //  2. maybe not all the variables in the "related_variables" are in the factors, not like the standard factor
  factor_of_this_clique = factor_of_this_clique.MultiplyWithFactor(f); // multiply two factors
  // TODO: double-check: "related_variables" is not changed, "set_disc_configs" is also not changed?
  // checked: "related_variables" is all the variables in the clique,
  //          "set_disc_configs" is all the configs of the variables in the clique
  //          therefore, they are not required to be changed, the only thing changed is the potentials
  map_potentials = factor_of_this_clique.map_potentials;
}


void Clique::UpdateUseMessage(Factor f) {
  MultiplyWithFactorSumOverExternalVars(f);
}

/**
 * @brief: construct a factor of this clique and return
 */
Factor Clique::ConstructMessage() {
  Factor message_factor(related_variables, set_disc_configs, map_potentials);
  return message_factor;
}


void Clique::PrintPotentials() const {
  if (pure_discrete) {
    for (const auto &potentials_key_value : map_potentials) {
      for (const auto &vars_index_value : potentials_key_value.first) {
        cout << '(' << vars_index_value.first << ',' << vars_index_value.second << ") ";
      }
      cout << "\t: " << potentials_key_value.second << endl;
    }
  } else {
    fprintf(stderr, "%s not implemented for continuous clique yet!", __FUNCTION__);
    exit(1);
    // todo: implement
  }
  cout << "----------" << endl;
}

void Clique::PrintRelatedVars() const {
  string out = "{  ";
  for (const auto &v : related_variables) {
    if (v==elimination_variable_index) {
      out += "[" + to_string(v) + "]";
    } else {
      out += to_string(v);
    }
    out += "  ";
  }
  out += "}";
  cout << out << endl;
}