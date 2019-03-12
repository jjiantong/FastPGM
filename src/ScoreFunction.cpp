//
// Created by llj on 3/12/19.
//

#include "ScoreFunction.h"

ScoreFunction::ScoreFunction(Network *net, Trainer *trn) {
  this->net = net;
  this->trn = trn;
  num_network_params = 0;
  for (auto &node_ptr : net->set_node_ptr_container) {
    num_network_params += node_ptr->num_potential_vals * node_ptr->set_parents_combinations.size();
  }
}

double ScoreFunction::LogLikelihoodForNode(Node *node_ptr, Network *net, Trainer *trn) {
  // todo: check the correctness

  // Use the notation like the papers (e.g. r_i, q_i, N_ij, N_ijk).

  double log_likelihood = 0;
  for (int i=0; i<node_ptr->num_potential_vals; ++i) {
    int index = node_ptr->GetNodeIndex();
    int val = node_ptr->potential_vals[i];
    for (auto &par_comb : node_ptr->set_parents_combinations) {
      int n_ijk = 0;
      for (int s=0; s<trn->n_train_instance; ++s) {

        // Check this node.
        bool compatible = (trn->train_set_y_X[s][index]==val);

        // Check parents.
        for (auto &p : par_comb) {
          if (!compatible) {break;}
          compatible = (trn->train_set_y_X[s][p.first]==p.second);
        }

        n_ijk += compatible ? 1 : 0;
      }

      log_likelihood +=
              (n_ijk
               *
               log(node_ptr->map_cond_prob_table[val][par_comb]));
    }
  }
  return log_likelihood;
}

double ScoreFunction::LogLikelihood(Network *net, Trainer *trn) {
  // todo: check the correctness
  double log_likelihood = 0;
  for (auto &node_ptr : net->set_node_ptr_container) {
    log_likelihood += LogLikelihoodForNode(node_ptr, net, trn);
  }
  return log_likelihood;
}

double ScoreFunction::K2(Network *net, Trainer *trn) {
  // todo: check the correctness

  // Use the notation like the paper (Cooper, 1992) (e.g. r_i, q_i, N_ij, N_ijk).

  // Assume equal prior on every possible structure.
  // Since it is equal for all structures, I will just ignore it.
  // (Or, I can say that I set it to be 1.)

  double sum_over_i = 0;

  for (auto &node_ptr : net->set_node_ptr_container) {
    int node_index = node_ptr->GetNodeIndex();
    int r_i = node_ptr->num_potential_vals;

    double sum_over_j = 0;

    for (auto &par_comb : node_ptr->set_parents_combinations) {

      set<int> set_instances_parent_compatible;
      int n_ij = 0;

      for (int s=0; s<trn->n_train_instance; ++s) {

        // Check parents.
        bool parents_compatible = true;
        for (auto &p : par_comb) {
          if (!parents_compatible) {break;}
          parents_compatible = (trn->train_set_y_X[s][p.first]==p.second);
        }

        if (parents_compatible) {
          set_instances_parent_compatible.insert(s);
          ++n_ij;
        }

      }

      int sum_of_factorial_of_n_ijk = 0;
      for (int k=0; k<r_i; ++k){
        int n_ijk = 0;
        for (auto &s : set_instances_parent_compatible) {
          int val = node_ptr->potential_vals[k];
          // Check this node.
          n_ijk += (trn->train_set_y_X[s][node_index] == val) ? 1 : 0;
        }
        sum_of_factorial_of_n_ijk += FactorialForSmallInteger(n_ijk);
      }

      sum_over_j +=
              (FactorialForSmallInteger(r_i-1)
              *
              sum_of_factorial_of_n_ijk
              /
              (double)FactorialForSmallInteger(n_ij+r_i-1));
    }

    sum_over_i += sum_over_j;
  }

  return sum_over_i;
}

double ScoreFunction::BDe(Network *net, Trainer *trn) {
  fprintf(stderr, "Function %s! is not implemented!", __FUNCTION__);
  exit(1);
}

double ScoreFunction::BDeu(Network *net, Trainer *trn) {
  fprintf(stderr, "Function %s! is not implemented!", __FUNCTION__);
  exit(1);
}

double ScoreFunction::AIC(Network *net, Trainer *trn) {
  // todo: check the correctness
  double penalty = -num_network_params;

  double log_likelihood = LogLikelihood(net, trn);

  return (log_likelihood + penalty);
}

double ScoreFunction::BIC(Network *net, Trainer *trn) {
  return MDL(net,trn);
}

double ScoreFunction::MDL(Network *net, Trainer *trn) {
  // todo: check the correctness
  double penalty = -(0.5 * log(net->num_nodes) * num_network_params);

  double log_likelihood = LogLikelihood(net, trn);

  return (log_likelihood + penalty);
}

int ScoreFunction::GammaFunctionForInteger(int n) {
  return (int)tgamma(n);
}