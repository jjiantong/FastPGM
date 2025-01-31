#include "fastpgm/DiscreteNode.h"

#include <utility>

DiscreteNode::DiscreteNode(): DiscreteNode(-1) {}

DiscreteNode::DiscreteNode(int index) {
    is_discrete = true;
    SetNodeIndex(index);
    num_potential_vals = -1;//may be 0 is better?
}

int DiscreteNode::GetDomainSize() const {
  return num_potential_vals;
}

void DiscreteNode::SetDomainSize(int size) {
  num_potential_vals = size;
}

void DiscreteNode::AddParent(Node *p) {
  if (!p->is_discrete) {
    fprintf(stderr, "Error in function %s! \n"
                    "Discrete node must not have continuous parent!", __FUNCTION__);
    exit(1);
  }

  Node::AddParent(p);
//  int p_idx = p->GetNodeIndex();
//  if (set_parent_indexes.find(p_idx) == set_parent_indexes.end()) {
//    set_parent_indexes.insert(p_idx);
//
//    auto dp = (DiscreteNode *) p;
//    vec_disc_parent_indexes.push_back(p_idx);
//    map_disc_parents_domain_size[p_idx] = dp->GetDomainSize();
//
//    // Update possible parent configurations
//    set<DiscreteConfig> new_par_combs;
//    for (const auto &val : dp->vec_potential_vals) {
//      DiscVarVal vv(p_idx, val);
//      for (auto old_par_comb : set_discrete_parents_combinations) {
//        old_par_comb.insert(vv);
//        new_par_combs.insert(old_par_comb);
//      }
//    }
//    this->set_discrete_parents_combinations = new_par_combs;
//  }
}

void DiscreteNode::SetLaplaceSmooth(double alpha) {
  this->laplace_smooth = alpha;
}

string DiscreteNode::GetValueNameByIndex(const int &index) {
    for (auto it = possible_values_ids.begin(); it != possible_values_ids.end(); ++it) {
        if (it->second == index) {
            return it->first;
        }
    }
}

/**
 * @brief: initialize the conditional probability table
 */
void DiscreteNode::InitializeCPT() {
    if (!HasParents()) {//the node doesn't have a parent
        DiscreteConfig par_config;
        map_total_count_under_parents_config[par_config] = 0;
        for (int i = 0; i < GetDomainSize(); ++i) {
            map_cond_prob_table_statistics[i][par_config] = 0;
        }
    } else {
        for (const auto &par_config : set_discrete_parents_combinations) {
            map_total_count_under_parents_config[par_config] = 0; // todo
            for (int i = 0; i < GetDomainSize(); ++i) {
                map_cond_prob_table_statistics[i][par_config] = 0;
            }
        }
    }
    cpt_initialized = true;
}

/**
 * @brief: update the maps of a node given the variable of the new instance
 * map_total_count_under_parents_config: count for specific parent configuration
 * map_cond_prob_table_statistics: (joint) count for query variable (child) given parents configuration
 */
void DiscreteNode::AddCount(int query_val, DiscreteConfig &parents_config, int count) {
  if (!cpt_initialized) {
    InitializeCPT();
  }
  map_total_count_under_parents_config[parents_config] += count;
  map_cond_prob_table_statistics[query_val][parents_config] += count;
}

/**
 * @brief: use the counters in the probability table to compute the probabilities
 * parent configuration must be full for looking up the probability in the table
 */
// TODO: check the algorithm for the case of unseen values (based on a forgotten paper of weka)
double DiscreteNode:: GetProbability(int query_val, DiscreteConfig &parents_config) {
    int frequency_count =  map_cond_prob_table_statistics[query_val][parents_config]; // P(AB)
    int total = map_total_count_under_parents_config[parents_config]; // P(B)
    double prob = (frequency_count + laplace_smooth) / (total + laplace_smooth * GetDomainSize()); // P(A|B) = P(AB) / P(B)
    return prob;
}