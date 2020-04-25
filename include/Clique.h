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

typedef set< pair<int, int> > DiscreteConfig;

/**
 *
 */
class Clique {

 public:

  bool is_separator;//a clique can separate two other cliques.
  int clique_id;
  int clique_size;//the number of nodes in this clique
  bool pure_discrete;
  set<int> related_variables;
  set<DiscreteConfig> set_disc_configs;
  map<DiscreteConfig, double> map_potentials;
  set<Clique*> set_neighbours_ptr;

  /**
   * In junction tree algorithm,
   * the "Collect" force messages to flow from downstream to upstream,
   * and the "Distribute" force messages flow from upstream to downstream.
   * So, we need a member to record the upstream of this clique (node).
   */
  Clique *ptr_upstream_clique;//a clique only has one upstream, and all the other neighbours are downstreams.

  /**
   * Data structures needed for elimination tree
   * (like junction tree) of Gaussian Bayesian network.
   * Proposed in [Local Propagation in Conditional Gaussian Bayesian Networks (Cowell, 2005)]
   * Note that, separators between continuous cliques only retain post_bag but not lp_potential.
   */
  int elimination_variable_index; //a clique is associated to a main variable (cf. Cowell, 2005); the main variable can be eliminated using its neighbours?
  //the following three variables are only used in regression problems.
  bool activeflag;
  vector<CGRegression> lp_potential;
  vector<CGRegression> post_bag;


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
  void PrintRelatedVars() const;

 protected:
  Clique(const Clique&) = default;
  Factor SumOutExternalVars(Factor);
  void Distribute(Factor);
  void PreInitializePotentials();


};


#endif //BAYESIANNETWORK_CLIQUE_H
