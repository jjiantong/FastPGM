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
#include "Timer.h"
#include "omp.h"

typedef set< pair<int, int> > DiscreteConfig;

class Clique {

 public:

  bool is_separator;//a clique can separate two other cliques.
  int clique_id;
  int clique_size;//the number of nodes in this clique
  bool pure_discrete;

  set <int> clique_variables;
  // the following three members are the same with the class "Factor"
  set<int> related_variables; //the variables involved in this clique
  set<DiscreteConfig> set_disc_configs; //all the configurations of the related variables
  map<DiscreteConfig, double> map_potentials; //the potential of each discrete config

  Factor *table;

  set<Clique*> set_neighbours_ptr; // neighbor cliques

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
//  int elimination_variable_index; //a clique is associated to a main variable (cf. Cowell, 2005);
//                                  // the main variable can be eliminated using its neighbours?
  //the following three variables are only used in regression problems.
  bool activeflag;
  vector<CGRegression> lp_potential;
  vector<CGRegression> post_bag;


  Clique();
//  Clique(set<Node*> set_node_ptrs, int elim_var_index);
    Clique(set<Node*> set_node_ptrs);
  ~Clique();

  Clique* CopyWithoutPtr();
  void MultiplyWithFactorSumOverExternalVars(Factor f, Timer *timer);
  Factor Collect(Timer *timer);
  void Distribute(Timer *timer);

  virtual void UpdateUseMessage(Factor f, Timer *timer);
  virtual Factor ConstructMessage(Timer *timer);
  void PrintPotentials() const;
//  void PrintRelatedVars() const;

 protected:
  Clique(const Clique&) = default;
  Factor SumOutExternalVars(Factor f, Timer *timer);
  void Distribute(Factor f, Timer *timer);
  void PreInitializePotentials();


};


#endif //BAYESIANNETWORK_CLIQUE_H
