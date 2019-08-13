//
// Created by LinjianLi on 2019/2/16.
//

#include "Clique.h"

Clique::Clique(set<Node*> set_node_ptr) {
  InitializeClique(set_node_ptr);
}


void Clique::InitializeClique(set<Node*> set_node_ptr) {
  clique_size = set_node_ptr.size();

  set<Combination> set_of_sets;

  for (auto &n : set_node_ptr) {
    related_variables.insert(n->GetNodeIndex());
    Combination c;
    for (int i=0; i<n->num_potential_vals; ++i) {
      c.insert(pair<int,int>(n->GetNodeIndex(),n->potential_vals[i]));
    }
    set_of_sets.insert(c);
  }

  set_combinations = GenAllCombFromSets(&set_of_sets);

  for (auto &c : set_combinations) {
    map_potentials[c] = 1;  // Initialize clique potential to be 1.
  }

  ptr_upstream_clique = nullptr;
}

Factor Clique::Collect() {
  // First collect from its downstream, then update itself.

  for (auto &ptr_separator : set_neighbours_ptr) {

    // The message passes from downstream to upstream.
    // Also, when it reaches a leaf, the only neighbour is the upstream,
    // which can be viewed as the base case of recursive function.
    if (ptr_separator==ptr_upstream_clique) {continue;}

    ptr_separator->ptr_upstream_clique = this;  // Let the callee know the caller.
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
  factor_of_this_clique.SetMembers(related_variables,set_combinations,map_potentials);

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
  factor_of_this_clique.SetMembers(related_variables,set_combinations,map_potentials);

  f = SumOutExternalVars(f);

  factor_of_this_clique = factor_of_this_clique.MultiplyWithFactor(f);

  map_potentials = factor_of_this_clique.map_potentials;
}


void Clique::UpdateUseMessage(Factor f) {
  MultiplyWithFactorSumOverExternalVars(f);
}

Factor Clique::ConstructMessage() {
  Factor message_factor;
  message_factor.SetMembers(related_variables,set_combinations,map_potentials);
  return message_factor;
}


void Clique::PrintPotentials() const {
  for (auto &potentials_key_value : map_potentials) {
    for (auto &vars_index_value : potentials_key_value.first) {
      cout << '(' << vars_index_value.first << ',' << vars_index_value.second << ") ";
    }
    cout << "\t: " << potentials_key_value.second << endl;
  }
  cout << "----------" << endl;
}