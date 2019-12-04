//
// Created by LinjianLi on 2019/2/9.
//

#include "DiscreteNode.h"
//#include "Network.h"

#include <utility>

DiscreteNode::DiscreteNode(): DiscreteNode(-1) {}

DiscreteNode::DiscreteNode(int index): DiscreteNode(index, to_string(index)) {}

DiscreteNode::DiscreteNode(int index, string name) {
  is_discrete = true;
  SetNodeIndex(index);
  node_name = std::move(name);

  num_potential_vals = -1;
}

void DiscreteNode::SetDomain(vector<string> str_domain) {
  SetDomainSize(str_domain.size());
  vec_str_potential_vals = str_domain;

  for (int i = 0; i < GetDomainSize(); ++i) {
    vec_potential_vals.push_back(i);
  }
}

void DiscreteNode::SetDomain(vector<int> int_domain) {
  SetDomainSize(int_domain.size());
  vec_potential_vals = int_domain;
}

int DiscreteNode::GetDomainSize() const {
  return num_potential_vals;
}

void DiscreteNode::SetDomainSize(int size) {
  num_potential_vals = size;
  vec_potential_vals.reserve(size);
}

void DiscreteNode::AddParent(Node *p) {
  if (!p->is_discrete) {
    fprintf(stderr, "Error in function %s! \n"
                    "Discrete node must not have continuous parent!", __FUNCTION__);
    exit(1);
  }
  int p_idx = p->GetNodeIndex();
  if (set_parent_indexes.find(p_idx) == set_parent_indexes.end()) {
    set_parent_indexes.insert(p_idx);
    vec_disc_parent_indexes.push_back(p_idx);
    map_disc_parents_domain_size[p_idx] = ((DiscreteNode*)p)->GetDomainSize();
  }
}

int DiscreteNode::GetNumParams() {
  int scale = this->GetNumParentsConfig();
  return this->GetDomainSize() * scale;
}

void DiscreteNode::ClearParams() {
  map_cond_prob_table_statistics.clear();
  map_total_count_under_parents_config.clear();
}


void DiscreteNode::PrintProbabilityTable() {
  cout << GetNodeIndex() << ":\t";

  if (this->HasParents()) {    // If this node has parents

    for(int i = 0; i<GetDomainSize(); ++i) {    // For each head variable of CPT
      int query = vec_potential_vals.at(i);
      auto it = set_discrete_parents_combinations.begin();
      for (int j = 0; j < GetNumParentsConfig(); ++j){  // For tail variables of CPT
        DiscreteConfig parcfg = *it;
        string condition = "parent_config_" + to_string(j);
        cout << "P(" << query << '|' << condition << ")=" << GetProbability(query, parcfg) << '\t';
        ++it;
      }
    }
    cout << endl;

  } else {

    DiscreteConfig parcfg;
    for(int i = 0; i < GetDomainSize(); ++i) {    // For each row of MPT
      int query = vec_potential_vals.at(i);
      cout << "P(" << query << ")=" << GetProbability(query, parcfg) << '\t';
    }
    cout << endl;

  }
}

int DiscreteNode::SampleNodeGivenParents(DiscreteConfig &evidence) {
  // The evidence should contain all parents of this node.
  // The evidence about other nodes (including children) are IGNORED!!!
  DiscreteConfig par_evi;
  for (auto &e : evidence) {
    if (set_parent_indexes.find(e.first) != set_parent_indexes.end()) {
      par_evi.insert(e);
    }
  }

  DiscreteConfig par_config;

  vector<int> weights;
  for (int i = 0; i < GetDomainSize(); ++i) {
    int w = (int) (GetProbability(vec_potential_vals.at(i), par_evi) * 10000);
    weights.push_back(w);
  }

  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  default_random_engine rand_gen(seed);
  discrete_distribution<int> this_distribution(weights.begin(),weights.end());
  return vec_potential_vals.at(this_distribution(rand_gen));
}


void DiscreteNode::SetLaplaceSmooth(double alpha) {
  this->laplace_smooth = alpha;
}


void DiscreteNode::AddInstanceOfVarVal(DiscreteConfig instance_of_var_val) {
  DiscreteConfig parents_config = GetDiscParConfigGivenAllVarValue(instance_of_var_val);
  AddCount(
          DiscreteConfigToMap(instance_of_var_val).at(GetNodeIndex()),
          parents_config,
          1);
}


void DiscreteNode::InitializeCPT() {

  if (!HasParents()) {
    DiscreteConfig par_config;
    map_total_count_under_parents_config[par_config] = 0;
    for (int i = 0; i < GetDomainSize(); ++i) {
      map_cond_prob_table_statistics[vec_potential_vals.at(i)][par_config] = 0;
    }
  } else {
    for (const auto &par_config : set_discrete_parents_combinations) {
      map_total_count_under_parents_config[par_config] = 0;
      for (int i = 0; i < GetDomainSize(); ++i) {
        map_cond_prob_table_statistics[vec_potential_vals.at(i)][par_config] = 0;
      }
    }
  }

  cpt_initialized = true;
}


void DiscreteNode::AddCount(int query_val, DiscreteConfig &parents_config, int count) {
  if (!cpt_initialized) { InitializeCPT(); }
  map_total_count_under_parents_config[parents_config] += count;
  map_cond_prob_table_statistics[query_val][parents_config] += count;
}

double DiscreteNode:: GetProbability(int query_val, DiscreteConfig &parents_config) {

  // If the given instance contains the value that has not been seen before,
  // return the smallest probability divided by the domain size and number of parents configurations.
  bool unseen_value = (set_discrete_parents_combinations.find(parents_config) == set_discrete_parents_combinations.end()
                       ||
                       std::find(vec_potential_vals.begin(), vec_potential_vals.end(), query_val) == vec_potential_vals.end());
  if (unseen_value) {
    fprintf(stdout, "In function [%s]: the given instance contains the value that has not been seen before.\n", __FUNCTION__);
    double min_prob = 1;
    for (int val : vec_potential_vals) {
      for (DiscreteConfig par_cfg : set_discrete_parents_combinations) {
        double temp = GetProbability(val, par_cfg);
        if (temp < min_prob) {
          min_prob = temp;
        }
      }
    }
    double prob = min_prob / (GetDomainSize() * GetNumParentsConfig());
    return prob;
  }

  int frequency_count =  map_cond_prob_table_statistics.at(query_val).at(parents_config);
  int total = map_total_count_under_parents_config[parents_config];
  double prob = (frequency_count + laplace_smooth) / (total + laplace_smooth * GetDomainSize());
  return prob;
}

//int DiscreteNode::GetIndexOfValue(int val) {
//  auto it = std::find(this->vec_potential_vals.begin(), this->vec_potential_vals.end(), val);
//  int val_index = std::distance(this->vec_potential_vals.begin(), it);
//  return val_index;
//}
