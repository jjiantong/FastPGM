//
// Created by llj on 3/12/19.
//

#include "ScoreFunction.h"

ScoreFunction::ScoreFunction(Network *net, Trainer *trn) {
  this->net = net;
  this->trn = trn;
  num_network_params = 0;
  for (const auto &node_ptr : net->set_node_ptr_container) {
    num_network_params += node_ptr->num_potential_vals * node_ptr->set_discrete_parents_combinations.size();
  }
}


double ScoreFunction::LogLikelihoodForNode(Node *node_ptr) {
  return LogLikelihoodForNode(node_ptr, net, trn);
}



double ScoreFunction::LogLikelihoodForNode(Node *node_ptr, Network *net, Trainer *trn) {
  // todo: check the correctness

  // Use the notation like the papers (e.g. r_i, q_i, N_ij, N_ijk).

  const int &r_i = node_ptr->num_potential_vals;
  double log_likelihood = 0;
  for (const auto &par_comb : node_ptr->set_discrete_parents_combinations) {
    int index = node_ptr->GetNodeIndex();

    set<int> set_instances_parent_compatible;
    int n_ij = 0;

    for (int s=0; s<trn->num_instance; ++s) {

      // Check parents.
      bool parents_compatible = true;
      for (const auto &p : par_comb) {
        if (!parents_compatible) {break;}
        parents_compatible = (trn->train_set_y_X[s][p.first]==p.second);
      }

      if (parents_compatible) {
        set_instances_parent_compatible.insert(s);
        ++n_ij;
      }

    }

    for (int k=0; k<r_i; ++k){
      int val = node_ptr->potential_vals[k];

      int n_ijk = 0;

      for (const auto &s : set_instances_parent_compatible) {
        // Check this node.
        n_ijk += (trn->train_set_y_X[s][index] == val) ? 1 : 0;
      }

      double tmp = n_ijk==0 ? 0 : log((double)n_ijk/n_ij);

      log_likelihood += n_ijk * tmp;
    }
  }
  return log_likelihood;
}


double ScoreFunction::LogLikelihood() {
  return LogLikelihood(net,trn);
}


double ScoreFunction::LogLikelihood(Network *net, Trainer *trn) {
  // todo: check the correctness
  double log_likelihood = 0;
  for (auto &node_ptr : net->set_node_ptr_container) {
    log_likelihood += LogLikelihoodForNode(node_ptr, net, trn);
  }
  return log_likelihood;
}

double ScoreFunction::K2() {
  return LogK2(net,trn);
}

double ScoreFunction::K2(Network *net, Trainer *trn) {
  // IMPORTANT:
  //   This implementation is according to the original equation in the paper.
  //   But, it may need to calculate the factorial of a large number (e.g. 2000!)
  //   causing overflow. So, I need to find another implementable equation.
  fprintf(stderr, "Function %s! is not allow to use! It will cause overflow.", __FUNCTION__);
  exit(1);

  // todo: check the correctness

  // Use the notation like the paper (Cooper, 1992) (e.g. r_i, q_i, N_ij, N_ijk).

  // Assume equal prior on every possible structure.
  // Since it is equal for all structures, I will just ignore it.
  // (Or, I can say that I set it to be 1.)

  double multiply_over_i = 1;

  for (const auto &node_ptr : net->set_node_ptr_container) {
    const int &node_index = node_ptr->GetNodeIndex();
    const int &r_i = node_ptr->num_potential_vals;

    double multiply_over_j = 1;

    for (const auto &par_comb : node_ptr->set_discrete_parents_combinations) {

      set<int> set_instances_parent_compatible;
      int n_ij = 0;

      for (int s=0; s<trn->num_instance; ++s) {

        // Check parents.
        bool parents_compatible = true;
        for (const auto &p : par_comb) {
          if (!parents_compatible) {break;}
          parents_compatible = (trn->train_set_y_X[s][p.first]==p.second);
        }

        if (parents_compatible) {
          set_instances_parent_compatible.insert(s);
          ++n_ij;
        }

      }

      int multiply_over_k = 1;
      for (int k=0; k<r_i; ++k){
        int n_ijk = 0;
        for (const auto &s : set_instances_parent_compatible) {
          int val = node_ptr->potential_vals[k];
          // Check this node.
          n_ijk += (trn->train_set_y_X[s][node_index] == val) ? 1 : 0;
        }
        int tmp = FactorialForSmallInteger(n_ijk); // todo: Delete this line.
        multiply_over_k *= FactorialForSmallInteger(n_ijk);
      }

      multiply_over_j *=
              (FactorialForSmallInteger(r_i-1)
              *
              multiply_over_k
              /
              (double)FactorialForSmallInteger(n_ij+r_i-1));
    }

    multiply_over_i *= multiply_over_j;
  }

  return multiply_over_i;
}

double ScoreFunction::LogK2(Network *net, Trainer *trn) {
  // todo: check the correctness

  // Use the notation like the paper (Cooper, 1992) (e.g. r_i, q_i, N_ij, N_ijk).

  // Assume equal prior on every possible structure.
  // Since it is equal for all structures, I will just ignore it.
  // (Or, I can say that I set it to be 1.)

  double sum_over_i = 0;

  for (const auto &node_ptr : net->set_node_ptr_container) {
    const int &node_index = node_ptr->GetNodeIndex();
    const int &r_i = node_ptr->num_potential_vals;

    double sum_over_j = 0;

    for (const auto &par_comb : node_ptr->set_discrete_parents_combinations) {

      set<int> set_instances_parent_compatible;
      int n_ij = 0;

      for (int s=0; s<trn->num_instance; ++s) {

        // Check parents.
        bool parents_compatible = true;
        for (const auto &p : par_comb) {
          if (!parents_compatible) {break;}
          parents_compatible = (trn->train_set_y_X[s][p.first]==p.second);
        }

        if (parents_compatible) {
          set_instances_parent_compatible.insert(s);
          ++n_ij;
        }

      }

      double sum_over_k = 0;
      for (int k=0; k<r_i; ++k){
        int n_ijk = 0;
        for (const auto &s : set_instances_parent_compatible) {
          int val = node_ptr->potential_vals[k];
          // Check this node.
          n_ijk += (trn->train_set_y_X[s][node_index] == val) ? 1 : 0;
        }
        double tmp = LogOfFactorial(n_ijk); // todo: Delete this line.
        sum_over_k *= LogOfFactorial(n_ijk);
      }

      sum_over_j +=
              (LogOfFactorial(r_i-1)
               -
               LogOfFactorial(n_ij+r_i-1)
               +
               sum_over_k);
    }

    sum_over_i += sum_over_j;
  }

  return sum_over_i;

}

double ScoreFunction::BDeu(int equi_sample_size) {
  return LogBDeu(net,trn,equi_sample_size);
}

double ScoreFunction::BDeu(Network *net, Trainer *trn, int equi_sample_size) {
  // IMPORTANT:
  //   This implementation is according to the original equation in the paper.
  //   But, it may need to calculate the factorial of a large number (e.g. 2000!)
  //   causing overflow. So, I need to find another implementable equation.
  fprintf(stderr, "Function %s! is not allow to use! It will cause overflow.", __FUNCTION__);
  exit(1);

  // todo: check the correctness

  // Use the notation like the paper (e.g. r_i, q_i, N_ij, N_ijk).
  // In the paper, N' is the equivalent sample size.

  // Assume equal prior on every possible structure.
  // Since it is equal for all structures, I will just ignore it.
  // (Or, I can say that I set it to be 1.)

  double multiply_over_i = 1;

  for (const auto &node_ptr : net->set_node_ptr_container) {
    const int &node_index = node_ptr->GetNodeIndex();
    const int &r_i = node_ptr->num_potential_vals;
    const int &q_i = node_ptr->set_discrete_parents_combinations.size();
    double multiply_over_j = 1;

    for (const auto &par_comb : node_ptr->set_discrete_parents_combinations) {

      set<int> set_instances_parent_compatible;
      int n_ij = 0;

      for (int s=0; s<trn->num_instance; ++s) {

        // Check parents.
        bool parents_compatible = true;
        for (const auto &p : par_comb) {
          if (!parents_compatible) {break;}
          parents_compatible = (trn->train_set_y_X[s][p.first]==p.second);
        }

        if (parents_compatible) {
          set_instances_parent_compatible.insert(s);
          ++n_ij;
        }

      }

      double multiply_over_k = 1;
      for (int k=0; k<r_i; ++k){
        int n_ijk = 0;
        for (const auto &s : set_instances_parent_compatible) {
          int val = node_ptr->potential_vals[k];
          // Check this node.
          n_ijk += (trn->train_set_y_X[s][node_index] == val) ? 1 : 0;
        }

        double n_ijk_prime = (double)equi_sample_size/(r_i*q_i);
        double tmp = tgamma(n_ijk); // todo: Delete this line.
        multiply_over_k *= tgamma(n_ijk + n_ijk_prime)/tgamma(n_ijk_prime);
      }

      double n_ij_prime = (double)equi_sample_size/q_i;

      double gma_np = tgamma(n_ij_prime);
      double gma_n_plus_np = tgamma(n_ij + n_ij_prime);

      multiply_over_j *= gma_np / gma_n_plus_np * multiply_over_k;
    }

    multiply_over_i *= multiply_over_j;
  }

  return multiply_over_i;
}

double ScoreFunction::LogBDeu(Network *net, Trainer *trn, int equi_sample_size) {
  // todo: check the correctness

  // Use the notation like the paper (e.g. r_i, q_i, N_ij, N_ijk).
  // In the paper, N' is the equivalent sample size.

  // Assume equal prior on every possible structure.
  // Since it is equal for all structures, I will just ignore it.
  // (Or, I can say that I set it to be 1.)

  double sum_over_i = 0;

  for (const auto &node_ptr : net->set_node_ptr_container) {
    const int &node_index = node_ptr->GetNodeIndex();
    const int &r_i = node_ptr->num_potential_vals;
    const int &q_i = node_ptr->set_discrete_parents_combinations.size();
    double sum_over_j = 0;

    for (const auto &par_comb : node_ptr->set_discrete_parents_combinations) {

      set<int> set_instances_parent_compatible;
      int n_ij = 0;

      for (int s=0; s<trn->num_instance; ++s) {

        // Check parents.
        bool parents_compatible = true;
        for (const auto &p : par_comb) {
          if (!parents_compatible) {break;}
          parents_compatible = (trn->train_set_y_X[s][p.first]==p.second);
        }

        if (parents_compatible) {
          set_instances_parent_compatible.insert(s);
          ++n_ij;
        }

      }

      double sum_over_k = 1;
      for (int k=0; k<r_i; ++k){
        int n_ijk = 0;
        for (const auto &s : set_instances_parent_compatible) {
          int val = node_ptr->potential_vals[k];
          // Check this node.
          n_ijk += (trn->train_set_y_X[s][node_index] == val) ? 1 : 0;
        }

        double n_ijk_prime = (double)equi_sample_size/(r_i*q_i);
        int approx_n_ijk_prime = (int)n_ijk_prime + 1;
        sum_over_k +=
                LogOfFactorial(n_ijk+approx_n_ijk_prime-1)-LogOfFactorial(approx_n_ijk_prime-1);
      }

      double n_ij_prime = (double)equi_sample_size/q_i;
      int approx_n_ij_prime = (int)n_ij_prime + 1;

      double log_gma_np = LogOfFactorial(approx_n_ij_prime-1);
      double log_gma_n_plus_np = LogOfFactorial(n_ij + approx_n_ij_prime);

      sum_over_j += log_gma_np - log_gma_n_plus_np + sum_over_k;
    }

    sum_over_i += sum_over_j;
  }

  return sum_over_i;
}

double ScoreFunction::BDe(Network *net, Trainer *trn,  int equi_sample_size=10) {
  // todo: implement
  fprintf(stderr, "Function %s! is not implemented!", __FUNCTION__);
  exit(1);
}

double ScoreFunction::AIC() {
  return AIC(net,trn);
}

double ScoreFunction::AIC(Network *net, Trainer *trn) {
  // todo: check the correctness
  double penalty = -num_network_params;

  double log_likelihood = LogLikelihood(net, trn);

  return (log_likelihood + penalty);
}

double ScoreFunction::BIC() {
  return BIC(net,trn);
}

double ScoreFunction::BIC(Network *net, Trainer *trn) {
  return MDL(net,trn);
}

double ScoreFunction::MDL() {
  return MDL(net,trn);
}

double ScoreFunction::MDL(Network *net, Trainer *trn) {
  // todo: check the correctness
  double penalty = -(0.5 * log(net->num_nodes) * num_network_params);

  double log_likelihood = LogLikelihood(net, trn);

  return (log_likelihood + penalty);
}
