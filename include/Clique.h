//
// Created by LinjianLi on 2019/2/16.
//

#ifndef BAYESIANNETWORK_CLIQUE_H
#define BAYESIANNETWORK_CLIQUE_H

#include <set>
#include <map>
#include <utility>
#include <string>
#include "gadget.h"
#include "Node.h"
#include "Factor.h"
#include "CGRegression.h"

typedef set< pair<int, int> > Combination;

class Clique {

 public:

  bool is_separator;
  int clique_id;
  int clique_size;
  bool pure_discrete;
  set<int> related_variables;
  set<Combination> set_disc_combinations;
  map<Combination, double> map_potentials;
  set<Clique*> set_neighbours_ptr;

  // In junction tree algorithm,
  // the "Collect" force messages to flow from downstream to upstream,
  // and the "Distribute" force messages flow from upstream to downstream.
  // So, we need a member to record the upstream of this clique (node).
  Clique *ptr_upstream_clique;


  Clique();
  Clique(set<Node*> set_node_ptrs, int elim_var_index);
  virtual ~Clique() = default;

  Clique* CopyWithoutPtr();
  void MultiplyWithFactorSumOverExternalVars(Factor);
  Factor Collect();
  void Distribute();

  virtual void UpdateUseMessage(Factor);
  virtual Factor ConstructMessage();
  void PrintPotentials() const;

  // ==================================================
  // Data structures needed for elimination tree
  // (like junction tree) of gaussian Bayesian network.
  // Proposed in [Local Propagation in Conditional Gaussian Bayesian Networks (Cowell, 2005)]
  // Note that, separators between continuous cliques only retain post_bag but not lp_potential.

  int elimination_variable_index;   // Each clique is associated to a main variable according to the paper.
  bool activeflag;
  vector<CGRegression> lp_potential;
  vector<CGRegression> post_bag;
  // ==================================================

 protected:
  Clique(const Clique&) = default;
  Factor SumOutExternalVars(Factor);
  void Distribute(Factor);
  void PreInitializePotentials();


};


#endif //BAYESIANNETWORK_CLIQUE_H
