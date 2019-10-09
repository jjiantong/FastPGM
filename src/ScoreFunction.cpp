//
// Created by llj on 3/12/19.
//

#include "ScoreFunction.h"

ScoreFunction::ScoreFunction(Network *net, Dataset *dts) {
  this->network = net;
  this->dataset = dts;
}


double ScoreFunction::ScoreForNode(Node *node_ptr, string metric) {
  if (metric=="log likelihood") {
    return LogLikelihoodForNode(node_ptr);
  } else if (metric=="log K2") {
    return LogK2ForNode(node_ptr);
  } else if (metric=="log BDeu") {
    return LogBDeuForNode(node_ptr, 10);
  } else if (metric=="aic") {
    return AICForNode(node_ptr);
  } else if (metric=="bic") {
    return BICForNode(node_ptr);
  } else if (metric=="mdl") {
    return MDLForNode(node_ptr);
  } else {
    return LogLikelihoodForNode(node_ptr);
  }
}


double ScoreFunction::LogLikelihoodForNode(Node *node_ptr) {
  // todo: check the correctness

  // Use the notation like the papers (e.g. r_i, q_i, N_ij, N_ijk).
  const int index = node_ptr->GetNodeIndex();
  auto d_node_ptr = dynamic_cast<DiscreteNode*>(node_ptr);
  const int &r_i = d_node_ptr->num_potential_vals;
  double log_likelihood = 0;
  if (!node_ptr->set_parents_ptrs.empty()) {


    for (const auto &par_comb : node_ptr->set_discrete_parents_combinations) {

      set<int> set_instances_parent_compatible;
      int n_ij = 0;

      for (int s=0; s < dataset->num_instance; ++s) {

        // Check parents.
        bool parents_compatible = true;
        for (const auto &p : par_comb) {
          if (!parents_compatible) {break;}
          parents_compatible = (dataset->dataset_all_vars[s][p.first] == p.second);
        }

        if (parents_compatible) {
          set_instances_parent_compatible.insert(s);
          ++n_ij;
        }

      }

      for (int k=0; k<r_i; ++k){
        int val = d_node_ptr->potential_vals[k];

        int n_ijk = 0;

        for (const auto &s : set_instances_parent_compatible) {
          // Check this node.
          if (dataset->dataset_all_vars[s][index] == val) { ++n_ijk; }
        }

        double tmp = ( n_ijk==0 ? 0 : log((double)n_ijk/n_ij) );

        log_likelihood += n_ijk * tmp;
      }
    }


  } else {  // else if the node has no parents


    // Since the node has no parents
    int n_ij = dataset->num_instance;

    for (int k=0; k<r_i; ++k){
      int val = d_node_ptr->potential_vals[k];

      int n_ijk = 0;

      for (int s=0; s < dataset->num_instance; ++s) {
        // Check this node.
        if (dataset->dataset_all_vars[s][index] == val) { ++n_ijk; }
      }

      double tmp = ( n_ijk==0 ? 0 : log((double)n_ijk/n_ij) );

      log_likelihood += n_ijk * tmp;
    }
  }

  return log_likelihood;
}


double ScoreFunction::LogLikelihood() {
  // todo: check the correctness
  double log_likelihood = 0;
  for (auto &node_ptr : network->set_node_ptr_container) {
    log_likelihood += LogLikelihoodForNode(node_ptr);
  }
  return log_likelihood;
}


double ScoreFunction::LogK2ForNode(Node *node_ptr) {
  const int &node_index = node_ptr->GetNodeIndex();
  auto d_node_ptr = dynamic_cast<DiscreteNode*>(node_ptr);
  const int &r_i = d_node_ptr->num_potential_vals;

  double sum_over_j = 0;

  if (!node_ptr->set_parents_ptrs.empty()) {


    for (const auto &par_comb : node_ptr->set_discrete_parents_combinations) {

      set<int> set_instances_parent_compatible;
      int n_ij = 0;

      for (int s = 0; s < dataset->num_instance; ++s) {

        // Check parents.
        bool parents_compatible = true;
        for (const auto &p : par_comb) {
          if (!parents_compatible) { break; }
          parents_compatible = (dataset->dataset_all_vars[s][p.first] == p.second);
        }

        if (parents_compatible) {
          set_instances_parent_compatible.insert(s);
          ++n_ij;
        }

      }

      double sum_over_k = 0;
      for (int k = 0; k < r_i; ++k) {
        int n_ijk = 0;
        for (const auto &s : set_instances_parent_compatible) {
          int val = d_node_ptr->potential_vals[k];
          // Check this node.
          if (dataset->dataset_all_vars[s][node_index] == val) { ++n_ijk; }
        }
        sum_over_k += LogOfFactorial(n_ijk);
      }

      sum_over_j +=
              (LogOfFactorial(r_i - 1)
               -
               LogOfFactorial(n_ij + r_i - 1)
               +
               sum_over_k);
    }


  } else {  // else if the node has no parents

    // Since the node has no parents
    int n_ij = dataset->num_instance;

    double sum_over_k = 0;
    for (int k = 0; k < r_i; ++k) {
      int n_ijk = 0;
      for (int s=0; s < dataset->num_instance; ++s) {
        int val = d_node_ptr->potential_vals[k];
        // Check this node.
        if (dataset->dataset_all_vars[s][node_index] == val) { ++n_ijk; }
      }
      sum_over_k += LogOfFactorial(n_ijk);
    }

    sum_over_j +=
            (LogOfFactorial(r_i - 1)
             -
             LogOfFactorial(n_ij + r_i - 1)
             +
             sum_over_k);
  }

  return sum_over_j;
}


double ScoreFunction::LogK2() {
  // todo: check the correctness

  // Use the notation like the paper (Cooper, 1992) (e.g. r_i, q_i, N_ij, N_ijk).

  // Assume equal prior on every possible structure.
  // Since it is equal for all structures, I will just ignore it.
  // (Or, I can say that I set it to be 1.)

  double sum_over_i = 0;

  for (const auto &node_ptr : network->set_node_ptr_container) {
    double sum_over_j = LogK2ForNode(node_ptr);
    sum_over_i += sum_over_j;
  }

  return sum_over_i;

}


double ScoreFunction::K2() {
  fprintf(stderr, "Simple K2 will cause overflow. Changed to log space.\n");
  return LogK2();
}


double ScoreFunction::LogBDeuForNode(Node *node_ptr, int equi_sample_size) {
  const int &node_index = node_ptr->GetNodeIndex();
  auto d_node_ptr = dynamic_cast<DiscreteNode*>(node_ptr);
  const int &r_i = d_node_ptr->num_potential_vals;
  double sum_over_j = 0;

  if (!node_ptr->set_parents_ptrs.empty()) {

    const int &q_i = node_ptr->set_discrete_parents_combinations.size();
    for (const auto &par_comb : node_ptr->set_discrete_parents_combinations) {

      set<int> set_instances_parent_compatible;
      int n_ij = 0;

      for (int s = 0; s < dataset->num_instance; ++s) {

        // Check parents.
        bool parents_compatible = true;
        for (const auto &p : par_comb) {
          if (!parents_compatible) { break; }
          parents_compatible = (dataset->dataset_all_vars[s][p.first] == p.second);
        }

        if (parents_compatible) {
          set_instances_parent_compatible.insert(s);
          ++n_ij;
        }

      }

      double sum_over_k = 0;
      for (int k = 0; k < r_i; ++k) {
        int n_ijk = 0;
        for (const auto &s : set_instances_parent_compatible) {
          int val = d_node_ptr->potential_vals[k];
          // Check this node.
          n_ijk += (dataset->dataset_all_vars[s][node_index] == val) ? 1 : 0;
        }

        double n_ijk_prime = (double) equi_sample_size / (r_i * q_i);
        int approx_n_ijk_prime = (int) n_ijk_prime + 1;
        sum_over_k +=
                LogOfFactorial(n_ijk + approx_n_ijk_prime - 1) - LogOfFactorial(approx_n_ijk_prime - 1);
      }

      double n_ij_prime = (double) equi_sample_size / q_i;
      int approx_n_ij_prime = (int) n_ij_prime + 1;

      double log_gma_np = LogOfFactorial(approx_n_ij_prime - 1);
      double log_gma_n_plus_np = LogOfFactorial(n_ij + approx_n_ij_prime);

      sum_over_j += log_gma_np - log_gma_n_plus_np + sum_over_k;
    }


  } else {

    // Since the node has no parents
    int n_ij = dataset->num_instance;
    const int &q_i = 1;

    double sum_over_k = 0;
    for (int k = 0; k < r_i; ++k) {
      int n_ijk = 0;
      for (int s = 0; s < dataset->num_instance; ++s) {
        int val = d_node_ptr->potential_vals[k];
        // Check this node.
        n_ijk += (dataset->dataset_all_vars[s][node_index] == val) ? 1 : 0;
      }

      double n_ijk_prime = (double) equi_sample_size / (r_i * q_i);
      int approx_n_ijk_prime = (int) n_ijk_prime + 1;
      sum_over_k +=
              LogOfFactorial(n_ijk + approx_n_ijk_prime - 1) - LogOfFactorial(approx_n_ijk_prime - 1);
    }

    double n_ij_prime = (double) equi_sample_size / q_i;
    int approx_n_ij_prime = (int) n_ij_prime + 1;

    double log_gma_np = LogOfFactorial(approx_n_ij_prime - 1);
    double log_gma_n_plus_np = LogOfFactorial(n_ij + approx_n_ij_prime);

    sum_over_j += log_gma_np - log_gma_n_plus_np + sum_over_k;

  }

  return sum_over_j;
}


double ScoreFunction::LogBDeu(int equi_sample_size) {
  // todo: check the correctness

  // Use the notation like the paper (e.g. r_i, q_i, N_ij, N_ijk).
  // In the paper, N' is the equivalent sample size.

  // Assume equal prior on every possible structure.
  // Since it is equal for all structures, I will just ignore it.
  // (Or, I can say that I set it to be 1.)

  double sum_over_i = 0;

  for (const auto &node_ptr : network->set_node_ptr_container) {
    double sum_over_j = LogBDeuForNode(node_ptr, equi_sample_size);
    sum_over_i += sum_over_j;
  }

  return sum_over_i;
}


double ScoreFunction::BDeu(int equi_sample_size) {
  fprintf(stderr, "Simple BDeu will cause overflow. Changed to log space.\n");
  return LogBDeu(equi_sample_size);
}


double ScoreFunction::AICForNode(Node *node) {
  double penalty = -node->GetNumParams();
  double log_likelihood_for_node = LogLikelihoodForNode(node);
  return (log_likelihood_for_node + penalty);
}


double ScoreFunction::AIC() {
  // todo: check the correctness
  double result = 0;
  for (auto n : network->set_node_ptr_container) {
    result += AICForNode(n);
  }
  return result;
}


double ScoreFunction::BICForNode(Node *node) {
  return MDLForNode(node);
}

double ScoreFunction::BIC() {
  return MDL();
}


double ScoreFunction::MDLForNode(Node *node) {
  double penalty = -(0.5 * log(network->num_nodes) * node->GetNumParams());
  double log_likelihood_for_node = LogLikelihoodForNode(node);
  return (log_likelihood_for_node + penalty);
}


double ScoreFunction::MDL() {
  // todo: check the correctness
  double result = 0;
  for (auto n : network->set_node_ptr_container) {
    result += MDLForNode(n);
  }
  return result;
}