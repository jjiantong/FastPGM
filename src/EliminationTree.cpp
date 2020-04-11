//
// Created by LinjianLi on 2019/9/1.
//

#include "EliminationTree.h"

EliminationTree::EliminationTree(Network *net)
  : EliminationTree(net, "rev-topo") {}

EliminationTree::EliminationTree(Network *net, string elim_ord_strategy)
  : JunctionTree(net, elim_ord_strategy, false) {
  int last_elem_in_rev_topo_ord = elimination_ordering.back();
  for (const auto &c : this->set_clique_ptr_container) {
    if (c->elimination_variable_index==last_elem_in_rev_topo_ord) {
      this->strong_root = c;
      break;
    }
  }
  CalcuEachToRoot();
  InitCGRegressions();
}

EliminationTree::EliminationTree(Network *net, vector<int> custom_elim_ord)
  : JunctionTree(net, "custom", false, custom_elim_ord) {
  int last_elem_in_rev_topo_ord = elimination_ordering.back();
  for (const auto &c : this->set_clique_ptr_container) {
    if (c->elimination_variable_index==last_elem_in_rev_topo_ord) {
      this->strong_root = c;
      break;
    }
  }
  CalcuEachToRoot();
  InitCGRegressions();
}


void EliminationTree::CalcuEachToRoot() {
  // todo: test correctness
  map_to_root[strong_root->elimination_variable_index] = -1;
  queue<Clique*> to_be_calcu;
  to_be_calcu.push(strong_root);
  while (!to_be_calcu.empty()) {
    Clique* this_clique_ptr = to_be_calcu.front();
    to_be_calcu.pop();
    for (const auto &sep : this_clique_ptr->set_neighbours_ptr) {
      for (const auto &clq : sep->set_neighbours_ptr) {
        if (map_to_root.find(clq->elimination_variable_index)==map_to_root.end()) {
          to_be_calcu.push(clq);
          map_to_root[clq->elimination_variable_index] = this_clique_ptr->elimination_variable_index;
        }
      }
    }
  }
}

void EliminationTree::InitCGRegressions() {
  // todo: test correctness
  // Described in
  // [Local Propagation in Conditional Gaussian Bayesian Networks (Cowell, 2005)]
  // section 5.5.
  vector<int> topo_ord = this->network->GetTopoOrd();
  for (const auto &v : elimination_ordering) {
    if (map_elim_var_to_clique[v]->pure_discrete) { break; }
    Clique *clq = map_elim_var_to_clique[v];
    cout << "Processing clique: ";
    clq->PrintRelatedVars();
    // Sort the CG regressions so that the head variables are compatible with topological ordering.
    sort(
      clq->post_bag.begin(),
      clq->post_bag.end(),
      [&topo_ord](CGRegression &a, CGRegression &b) {
        return OccurInCorrectOrder(a.head_var_index, b.head_var_index, topo_ord);
      }
    );
    while (!clq->post_bag.empty()) {
      CGRegression R = clq->post_bag.front();
      clq->post_bag.erase(clq->post_bag.begin());
      if (R.set_all_tail_index.find(clq->elimination_variable_index)!=R.set_all_tail_index.end()) {
        CGRegression::Exchange(R, clq->lp_potential.front());
      }
      Clique *c_ji = map_elim_var_to_clique[map_to_root[clq->elimination_variable_index]];
      if (c_ji->elimination_variable_index==R.head_var_index) {
        c_ji->lp_potential.push_back(R);
      } else {
        c_ji->post_bag.push_back(R);
      }
    }
  }
  // Check
  for (const auto &c : set_clique_ptr_container) {
    if (!c->pure_discrete) {
      if (c->lp_potential.size()!=1) {
        fprintf(stderr, "At the end of [%s], each lp-potential "
                        "contains more than 1 CG regressions.", __FUNCTION__);
        exit(1);
      }
      if (!c->post_bag.empty()) {
        fprintf(stderr, "At the end of [%s], "
                        "the postbags are not empty.", __FUNCTION__);
        exit(1);
      }
    }
  }
}


void EliminationTree::EnterSingleContEvidence(pair<int,double> e) {
  // todo: test correctness
  // Described in
  // [Local Propagation in Conditional Gaussian Bayesian Networks (Cowell, 2005)]
  // section 5.6, algorithm 5.3.

  // Step 1: Enter evidence in all regressions which the corresponding variable is in tail.
  for (const auto &v : elimination_ordering) {
    if (v==e.first || map_elim_var_to_clique[v]->pure_discrete) { break; }
    if (map_elim_var_to_clique[v]->activeflag
        &&
        map_elim_var_to_clique[v]->related_variables.find(e.first)!=map_elim_var_to_clique[v]->related_variables.end()) {
      map_elim_var_to_clique[v]->lp_potential.front().Substitute(e);
    }
  }

  // Step 2: Initialize the loop for pushing evidence towards a boundary cluster.
  int i = e.first;
  Clique *c_i = map_elim_var_to_clique[i];
  c_i->post_bag.push_back(
    c_i->lp_potential.front()
  );
  c_i->lp_potential.erase(
    c_i->lp_potential.begin()
  );
  c_i->activeflag = false;

  // Step 3: Push evidence towards a boundary cluster.
  // In the paper, the algorithm is described as
  // "while toroot(i) is not a boundary cluster".
  // We enter the evidence starting at the continuous cluster.
  // And we move to the strong root.
  // So, the boundary cluster is the first cluster that is purely discrete.
  while (!map_elim_var_to_clique[map_to_root[i]]->pure_discrete) {
    Clique *c_ri = map_elim_var_to_clique[map_to_root[i]];
    c_ri->post_bag.push_back(c_i->post_bag.front());
    if (c_ri->activeflag) {
      CGRegression::Exchange(c_ri->post_bag.front(), c_ri->lp_potential.front());
      c_ri->lp_potential.front().Substitute(e);
    }
    c_i->post_bag.erase(c_i->post_bag.begin());
    i = c_ri->elimination_variable_index;
    c_i = map_elim_var_to_clique[i];
  }

  // Step 4: Update the discrete part of the tree.
  // When the program runs into this line,
  // the clique indicated by "i" is the continuous clique neighbouring the boundary clique.
  if (map_elim_var_to_clique[i]->pure_discrete
      ||
      !map_elim_var_to_clique[map_to_root[i]]->pure_discrete) {
    fprintf(stderr, "Have not reached boundary cluster.\n"
                    "Something is wrong in [%s] and I do not know why yet!", __FUNCTION__);
    exit(1);
  }

  c_i = map_elim_var_to_clique[i];
  Separator *sep_to_root = nullptr;
  for (const auto &sep : c_i->set_neighbours_ptr) {
    for (const auto &clq : sep->set_neighbours_ptr) {
      if (clq->elimination_variable_index == map_to_root[c_i->elimination_variable_index]) {
        sep_to_root = dynamic_cast<Separator*>(sep);
        break;
      }
    }
    if (sep_to_root!=nullptr) { break; }
  }
  if (sep_to_root==nullptr) {
    fprintf(stderr, "Could not find the separator to the boundary cluster in [%s]!", __FUNCTION__);
    exit(1);
  }
  // I am not sure that if I should consider the case of the CG regression with no discrete tails.
  // When reach boundary, it should be conditioned on some discrete variables, in my view.
  for (const auto &comb : c_i->set_disc_configs) {
    double weight = - pow((e.second-c_i->post_bag.front().map_mu[comb]), 2) / (2 * c_i->post_bag.front().map_variance[comb]);
    weight = exp(weight);
    weight /= pow((2*M_PI*c_i->post_bag.front().map_variance[comb]), 0.5);
    sep_to_root->map_potentials[comb] = weight;
  }

  Factor f = sep_to_root->Clique::ConstructMessage();  // todo: check the actually called function
  map_elim_var_to_clique[map_to_root[c_i->elimination_variable_index]]->MultiplyWithFactorSumOverExternalVars(f);

  c_i->post_bag.erase(
    c_i->post_bag.begin()
  );
}

void EliminationTree::MessagePassingUpdateDiscretePartJT() {
  MessagePassingUpdateJT();
}

void EliminationTree::MessagePassingUpdateJT() {
  strong_root->Collect();
  strong_root->Distribute();
}


CGRegression EliminationTree::CalMarginalOfContinuousVar(int var_index) {
  // Described in
  // [Local Propagation in Conditional Gaussian Bayesian Networks (Cowell, 2005)]
  // section 5.7, algorithm 5.4.

  // Step 1: Initialize the loop.
  int i = var_index;
  Clique *c_i = map_elim_var_to_clique[i];
  c_i->post_bag.push_back(
          c_i->lp_potential.front()
  );


  // Step 2: Push variable towards a boundary cluster.
  // In the paper, the algorithm is described as
  // "while toroot(i) is not a boundary cluster".
  // We enter the evidence starting at the continuous cluster.
  // And we move to the strong root.
  // So, the boundary cluster is the first cluster that is purely discrete.
  while (!map_elim_var_to_clique[map_to_root[i]]->pure_discrete) {
    Clique *c_ri = map_elim_var_to_clique[map_to_root[i]];
    c_ri->post_bag.push_back(c_i->post_bag.front());
    if (c_ri->activeflag) {
      CGRegression c_ri_lp_potential = c_ri->lp_potential.front();
      CGRegression::Exchange(c_ri->post_bag.front(), c_ri_lp_potential);
    }
    c_i->post_bag.erase(c_i->post_bag.begin());
    i = c_ri->elimination_variable_index;
    c_i = map_elim_var_to_clique[i];
  }

  // Step 3: Find the marginal density.
  // When the program runs into this line,
  // the clique indicated by "i" is the continuous clique neighbouring the boundary clique.
  if (map_elim_var_to_clique[i]->pure_discrete
      ||
      !map_elim_var_to_clique[map_to_root[i]]->pure_discrete) {
    fprintf(stderr, "Have not reached boundary cluster.\n"
                    "Something is wrong in [%s] and I do not know why yet!", __FUNCTION__);
    exit(1);
  }

  c_i = map_elim_var_to_clique[i];
  Separator *sep_to_root = nullptr;
  for (const auto &sep : c_i->set_neighbours_ptr) {
    for (const auto &clq : sep->set_neighbours_ptr) {
      if (clq->elimination_variable_index == map_to_root[c_i->elimination_variable_index]) {
        sep_to_root = (Separator*)sep;
        break;
      }
    }
    if (sep_to_root!=nullptr) { break; }
  }
  if (sep_to_root==nullptr) {
    fprintf(stderr, "Could not find the separator to the boundary cluster in [%s]!", __FUNCTION__);
    exit(1);
  }

  Clique *boundary = map_elim_var_to_clique[map_to_root[c_i->elimination_variable_index]];
  sep_to_root->UpdateUseMessage(boundary->ConstructMessage());


  CGRegression result = c_i->post_bag.front();
  c_i->post_bag.erase(
    c_i->post_bag.begin()
  );
  if (result.set_discrete_tails_combinations!=c_i->set_disc_configs) {
    fprintf(stderr, "Error in function [%s]\n"
                    "Two set of combinations is not the same!", __FUNCTION__);
    exit(1);
  }
  if (!result.vec_contin_tail_indexes.empty()) {
    fprintf(stderr, "Error in function [%s]\n"
                    "The result CG regression still depends on other continuous variables!", __FUNCTION__);
    exit(1);
  }

  // I am not sure that if I should consider the case of the CG regression with no discrete tails.
  // When reach boundary, it should be conditioned on some discrete variables, in my view.
  for (const auto &comb : c_i->set_disc_configs) {
    result.map_mu[comb] *= sep_to_root->map_potentials[comb];
    result.map_variance[comb] *= sep_to_root->map_potentials[comb];
    for (auto &coef : result.map_coefficients[comb]) {
      coef *= sep_to_root->map_potentials[comb];
    }
  }

  for (const auto &comb : c_i->set_disc_configs) {
    result.marginal_mu += result.map_mu[comb];
    result.marginal_variance += result.map_variance[comb];
    for (int i=0; i<result.map_coefficients[comb].size(); ++i) {
      result.marginal_coefficients.at(i) += result.map_coefficients[comb].at(i);
    }
  }
  result.set_discrete_tails_combinations.clear();
  result.map_coefficients.clear();
  result.map_mu.clear();
  result.map_variance.clear();

  for (const auto &coeff : result.marginal_coefficients) {
    if (coeff!=0) {
      fprintf(stderr, "Errer in function [%s]\n"
                      "At the end of the function, the result still depends on some continuous variable.",
                      __FUNCTION__);
      exit(1);
    }
  }

  return result;
}