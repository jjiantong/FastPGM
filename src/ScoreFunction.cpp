//
// Created by llj on 3/12/19.
//

#include "ScoreFunction.h"

ScoreFunction::ScoreFunction(Network *net, Dataset *dts) {
  this->network = net;
  this->dataset = dts;
  num_network_params = 0;
  for (const auto &node_ptr : net->set_node_ptr_container) {
    int scale = node_ptr->set_discrete_parents_combinations.empty() ? 1 : node_ptr->set_discrete_parents_combinations.size();
    num_network_params += dynamic_cast<DiscreteNode*>(node_ptr)->num_potential_vals * scale;
  }
}


double ScoreFunction::ScoreForNode(Node *node_ptr, string metric) {
  if (metric=="log likelihood") {
    return LogLikelihoodForNode(node_ptr);
  } else if (metric=="log K2") {
    return LogK2ForNode(node_ptr, network, dataset);
  } else if (metric=="log BDeu") {
    return LogBDeuForNode(node_ptr, dataset, 10);
  } else if (metric=="aic") {
    return AICForNode(node_ptr, dataset);
  } else if (metric=="bic") {
    return BICForNode(node_ptr, dataset);
  } else if (metric=="mdl") {
    return MDLForNode(node_ptr, dataset);
  } else {
    return LogLikelihoodForNode(node_ptr);
  }
}


double ScoreFunction::LogLikelihoodForNode(Node *node_ptr) {
  return LogLikelihoodForNode(node_ptr, network, dataset);
}



double ScoreFunction::LogLikelihoodForNode(Node *node_ptr, Network *net, Dataset *dts) {
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

      for (int s=0; s < dts->num_instance; ++s) {

        // Check parents.
        bool parents_compatible = true;
        for (const auto &p : par_comb) {
          if (!parents_compatible) {break;}
          parents_compatible = (dts->dataset_all_vars[s][p.first] == p.second);
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
          if (dts->dataset_all_vars[s][index] == val) { ++n_ijk; }
        }

        double tmp = ( n_ijk==0 ? 0 : log((double)n_ijk/n_ij) );

        log_likelihood += n_ijk * tmp;
      }
    }


  } else {  // else if the node has no parents


    // Since the node has no parents
    int n_ij = dts->num_instance;

    for (int k=0; k<r_i; ++k){
      int val = d_node_ptr->potential_vals[k];

      int n_ijk = 0;

      for (int s=0; s < dts->num_instance; ++s) {
        // Check this node.
        if (dts->dataset_all_vars[s][index] == val) { ++n_ijk; }
      }

      double tmp = ( n_ijk==0 ? 0 : log((double)n_ijk/n_ij) );

      log_likelihood += n_ijk * tmp;
    }
  }

  return log_likelihood;
}


double ScoreFunction::LogLikelihood() {
  return LogLikelihood(network, dataset);
}


double ScoreFunction::LogLikelihood(Network *net, Dataset *dts) {
  // todo: check the correctness
  double log_likelihood = 0;
  for (auto &node_ptr : net->set_node_ptr_container) {
    log_likelihood += LogLikelihoodForNode(node_ptr, net, dts);
  }
  return log_likelihood;
}


double ScoreFunction::K2() {
  return LogK2(network, dataset);
}

double ScoreFunction::K2(Network *net, Dataset *dts) {
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
    auto d_node_ptr = dynamic_cast<DiscreteNode*>(node_ptr);
    const int &r_i = d_node_ptr->num_potential_vals;

    double multiply_over_j = 1;

    for (const auto &par_comb : node_ptr->set_discrete_parents_combinations) {

      set<int> set_instances_parent_compatible;
      int n_ij = 0;

      for (int s=0; s < dts->num_instance; ++s) {

        // Check parents.
        bool parents_compatible = true;
        for (const auto &p : par_comb) {
          if (!parents_compatible) {break;}
          parents_compatible = (dts->dataset_all_vars[s][p.first] == p.second);
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
          int val = d_node_ptr->potential_vals[k];
          // Check this node.
          n_ijk += (dts->dataset_all_vars[s][node_index] == val) ? 1 : 0;
        }
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


double ScoreFunction::LogK2ForNode(Node *node_ptr, Network *net, Dataset *dts) {
  const int &node_index = node_ptr->GetNodeIndex();
  auto d_node_ptr = dynamic_cast<DiscreteNode*>(node_ptr);
  const int &r_i = d_node_ptr->num_potential_vals;

  double sum_over_j = 0;

  if (!node_ptr->set_parents_ptrs.empty()) {


    for (const auto &par_comb : node_ptr->set_discrete_parents_combinations) {

      set<int> set_instances_parent_compatible;
      int n_ij = 0;

      for (int s = 0; s < dts->num_instance; ++s) {

        // Check parents.
        bool parents_compatible = true;
        for (const auto &p : par_comb) {
          if (!parents_compatible) { break; }
          parents_compatible = (dts->dataset_all_vars[s][p.first] == p.second);
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
          if (dts->dataset_all_vars[s][node_index] == val) { ++n_ijk; }
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
    int n_ij = dts->num_instance;

    double sum_over_k = 0;
    for (int k = 0; k < r_i; ++k) {
      int n_ijk = 0;
      for (int s=0; s < dts->num_instance; ++s) {
        int val = d_node_ptr->potential_vals[k];
        // Check this node.
        if (dts->dataset_all_vars[s][node_index] == val) { ++n_ijk; }
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


double ScoreFunction::LogK2(Network *net, Dataset *dts) {
  // todo: check the correctness

  // Use the notation like the paper (Cooper, 1992) (e.g. r_i, q_i, N_ij, N_ijk).

  // Assume equal prior on every possible structure.
  // Since it is equal for all structures, I will just ignore it.
  // (Or, I can say that I set it to be 1.)

  double sum_over_i = 0;

  for (const auto &node_ptr : net->set_node_ptr_container) {
    double sum_over_j = LogK2ForNode(node_ptr, net, dts);
    sum_over_i += sum_over_j;
  }

  return sum_over_i;

}



double ScoreFunction::BDeu(int equi_sample_size) {
  return LogBDeu(network, dataset, equi_sample_size);
}

double ScoreFunction::BDeu(Network *net, Dataset *dts, int equi_sample_size) {
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
    auto d_node_ptr = dynamic_cast<DiscreteNode*>(node_ptr);
    const int &r_i = d_node_ptr->num_potential_vals;
    const int &q_i = node_ptr->set_discrete_parents_combinations.size();
    double multiply_over_j = 1;

    for (const auto &par_comb : node_ptr->set_discrete_parents_combinations) {

      set<int> set_instances_parent_compatible;
      int n_ij = 0;

      for (int s=0; s < dts->num_instance; ++s) {

        // Check parents.
        bool parents_compatible = true;
        for (const auto &p : par_comb) {
          if (!parents_compatible) {break;}
          parents_compatible = (dts->dataset_all_vars[s][p.first] == p.second);
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
          int val = d_node_ptr->potential_vals[k];
          // Check this node.
          n_ijk += (dts->dataset_all_vars[s][node_index] == val) ? 1 : 0;
        }

        double n_ijk_prime = (double)equi_sample_size/(r_i*q_i);
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


double ScoreFunction::LogBDeuForNode(Node *node_ptr, Dataset *dts, int equi_sample_size) {
  const int &node_index = node_ptr->GetNodeIndex();
  auto d_node_ptr = dynamic_cast<DiscreteNode*>(node_ptr);
  const int &r_i = d_node_ptr->num_potential_vals;
  double sum_over_j = 0;

  if (!node_ptr->set_parents_ptrs.empty()) {

    const int &q_i = node_ptr->set_discrete_parents_combinations.size();
    for (const auto &par_comb : node_ptr->set_discrete_parents_combinations) {

      set<int> set_instances_parent_compatible;
      int n_ij = 0;

      for (int s = 0; s < dts->num_instance; ++s) {

        // Check parents.
        bool parents_compatible = true;
        for (const auto &p : par_comb) {
          if (!parents_compatible) { break; }
          parents_compatible = (dts->dataset_all_vars[s][p.first] == p.second);
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
          n_ijk += (dts->dataset_all_vars[s][node_index] == val) ? 1 : 0;
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
    int n_ij = dts->num_instance;
    const int &q_i = 1;

    double sum_over_k = 0;
    for (int k = 0; k < r_i; ++k) {
      int n_ijk = 0;
      for (int s = 0; s < dts->num_instance; ++s) {
        int val = d_node_ptr->potential_vals[k];
        // Check this node.
        n_ijk += (dts->dataset_all_vars[s][node_index] == val) ? 1 : 0;
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


double ScoreFunction::LogBDeu(Network *net, Dataset *dts, int equi_sample_size) {
  // todo: check the correctness

  // Use the notation like the paper (e.g. r_i, q_i, N_ij, N_ijk).
  // In the paper, N' is the equivalent sample size.

  // Assume equal prior on every possible structure.
  // Since it is equal for all structures, I will just ignore it.
  // (Or, I can say that I set it to be 1.)

  double sum_over_i = 0;

  for (const auto &node_ptr : net->set_node_ptr_container) {
    double sum_over_j = LogBDeuForNode(node_ptr, dts, equi_sample_size);
    sum_over_i += sum_over_j;
  }

  return sum_over_i;
}



double ScoreFunction::AIC() {
  return AIC(network, dataset);
}


double ScoreFunction::AICForNode(Node *node, Dataset *dts) {
  double penalty = -node->GetNumParams();
  double log_likelihood_for_node = LogLikelihoodForNode(node);
  return (log_likelihood_for_node + penalty);
}


double ScoreFunction::AIC(Network *net, Dataset *dts) {
  // todo: check the correctness
  double result = 0;
  for (auto n : net->set_node_ptr_container) {
    result += AICForNode(n, dts);
  }
  return result;
}

double ScoreFunction::BIC() {
  return BIC(network, dataset);
}

double ScoreFunction::BICForNode(Node *node, Dataset *dts) {
  return MDLForNode(node, dts);
}

double ScoreFunction::BIC(Network *net, Dataset *dts) {
  return MDL(net, dts);
}

double ScoreFunction::MDL() {
  return MDL(network, dataset);
}


double ScoreFunction::MDLForNode(Node *node, Dataset *dts) {
  double penalty = -(0.5 * log(network->num_nodes) * node->GetNumParams());
  double log_likelihood_for_node = LogLikelihoodForNode(node);
  return (log_likelihood_for_node + penalty);
}


double ScoreFunction::MDL(Network *net, Dataset *dts) {
  // todo: check the correctness
  double result = 0;
  for (auto n : net->set_node_ptr_container) {
    result += MDLForNode(n, dts);
  }
  return result;
}