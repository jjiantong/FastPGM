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

  set<DiscreteConfig> set_of_sets;

  for (auto &n : set_node_ptr) {
    related_variables.insert(n->GetNodeIndex());
    if (n->is_discrete) {
      DiscreteConfig c;
      for (int i = 0; i < n->num_potential_vals; ++i) {
        c.insert(pair<int, int>(n->GetNodeIndex(), n->potential_vals[i]));
      }
      set_of_sets.insert(c);
    }
  }

  set_disc_configs = GenAllCombinationsFromSets(&set_of_sets);

  PreInitializePotentials();

  ptr_upstream_clique = nullptr;
}


void Clique::PreInitializePotentials() {
  if (pure_discrete) {
    for (auto &c : set_disc_configs) {
      map_potentials[c] = 1;  // Initialize clique potential to be 1.
    }
  } else {
    // Initialize lp_potential and post_bag to be empty. That is, do NOTHING.
  }
}


Clique* Clique::CopyWithoutPtr() {
  auto c = new Clique(*this);
  c->set_neighbours_ptr.clear();
  c->ptr_upstream_clique = nullptr;
  return c;
}


Factor Clique::Collect() {
  // First collect from its downstream, then update itself.

  for (auto &ptr_separator : set_neighbours_ptr) {

    // The message passes from downstream to upstream.
    // Also, when it reaches a leaf, the only neighbour is the upstream,
    // which can be viewed as the base case of recursive function.
    if (ptr_separator==ptr_upstream_clique) {continue;}

    ptr_separator->ptr_upstream_clique = this;  // Let the callee know the caller.

    // If the next clique connected by this separator is a continuous clique,
    // then the program should not collect from it. All information needed from
    // the continuous clique has been pushed to the boundary when entering the evidence.
    bool reach_boundary = false;
    for (const auto &next_clq_ptr : ptr_separator->set_neighbours_ptr) {
        reach_boundary = (next_clq_ptr->ptr_upstream_clique!=ptr_separator && !next_clq_ptr->pure_discrete);
    }
    if (reach_boundary) { continue; }

    Factor f = ptr_separator->Collect();  // Collect from downstream.
    UpdateUseMessage(f);  // Update itself.
  }

  // Prepare message for the upstream.
  return ConstructMessage();
}


void Clique::Distribute() {
  Factor f = ConstructMessage();

  // Store the pointers in an array to make use of OpenMP.
//  int num_neighbours = set_neighbours_ptr.size();
//  Clique** arr_neighbours_ptr_container = new Clique*[num_neighbours];
//  auto iter_nei_ptr = set_neighbours_ptr.begin();
//  for (int i=0; i<num_neighbours; ++i) {
//    arr_neighbours_ptr_container[i] = *(iter_nei_ptr++);
//  }
//  #pragma omp parallel for
//  for (int i=0; i<num_neighbours; ++i) {
//    auto &separator = arr_neighbours_ptr_container[i];
//    separator->Distribute(f);
//  }

  for (auto &sep : set_neighbours_ptr) {
    sep->Distribute(f);
  }
}


void Clique::Distribute(Factor f) {
  // If the next clique connected by this separator is a continuous clique,
  // then the program should not distribute information to it.
  bool reach_boundary = false;
  for (const auto &next_clq_ptr : set_neighbours_ptr) {
    reach_boundary = !next_clq_ptr->pure_discrete;
  }
  if (reach_boundary) { return; }

  // First update itself, then distribute to its downstream.

  UpdateUseMessage(f);  // Update itself.

  // Prepare message for the downstream.
  Factor distribute_factor = ConstructMessage();

  for (auto &ptr_separator : set_neighbours_ptr) {

    // The message passes from upstream to downstream.
    // Also, when it reaches a leaf, the only neighbour is the upstream,
    // which can be viewed as the base case of recursive function.
    if (ptr_separator==ptr_upstream_clique) {continue;}

    ptr_separator->ptr_upstream_clique = this;  // Let the callee know the caller.
    ptr_separator->Distribute(distribute_factor); // Distribute to downstream.

  }
}

Factor Clique::SumOutExternalVars(Factor f) {
  Factor factor_of_this_clique;
  factor_of_this_clique.SetMembers(this->related_variables,
                                   this->set_disc_configs,
                                   this->map_potentials);

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


void Clique::MultiplyWithFactorSumOverExternalVars(Factor f) {
  Factor factor_of_this_clique;
  factor_of_this_clique.SetMembers(related_variables, set_disc_configs, map_potentials);

  f = SumOutExternalVars(f);

  factor_of_this_clique = factor_of_this_clique.MultiplyWithFactor(f);

  map_potentials = factor_of_this_clique.map_potentials;
}


void Clique::UpdateUseMessage(Factor f) {
  MultiplyWithFactorSumOverExternalVars(f);
}

Factor Clique::ConstructMessage() {
  Factor message_factor;
  message_factor.SetMembers(related_variables, set_disc_configs, map_potentials);
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