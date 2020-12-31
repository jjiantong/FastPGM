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

  num_potential_vals = -1;//may be 0 is better?
}

/**
 * @brief: the values of the variables are represented using "string" in this function.
 */
void DiscreteNode::SetDomain(vector<string> str_domain) {
  SetDomainSize(str_domain.size());
  vec_str_potential_vals = str_domain;//potential values in string form

  for (int i = 0; i < GetDomainSize(); ++i) {
    vec_potential_vals.push_back(i);//potential values in "int" form
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

/**
 * @brief: obtain the number of probability values in the conditional probability table
 */
int DiscreteNode::GetNumParams() {
  int scale = this->GetNumParentsConfig();//the size of all parent combination (i.e., # of columns of the table)
  return this->GetDomainSize() * scale;
}

/**
 * @brief: clear the cells of the conditional probability table
 */
void DiscreteNode::ClearParams() {
  map_cond_prob_table_statistics.clear();
  map_total_count_under_parents_config.clear();

  //TODO: potential bugs; may need to add the following line
  //cpt_initialized = false;
}


void DiscreteNode::PrintProbabilityTable() {//checked
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

/**
 * @brief: helper of generating instances from the network for approximate inference
 */
int DiscreteNode::SampleNodeGivenParents(DiscreteConfig &evidence) {
  //Important: The evidence should contain all parents of this node. This condition can be met by generating an instance from the roots (without parents).
  // The evidence about other nodes (including children) are IGNORED!!!
  DiscreteConfig par_evi;//for storing the parents of the current node
  for (auto &e : evidence) {
    if (set_parent_indexes.find(e.first) != set_parent_indexes.end()) {
      par_evi.insert(e);
    }
  }

  DiscreteConfig par_config;//not used

  //every potential value of this node has a weight
  vector<int> weights;
  for (int i = 0; i < GetDomainSize(); ++i) {
    int query_value = vec_potential_vals.at(i);//potential value of the current node
    int w = (int) (GetProbability(query_value, par_evi) * 10000);//convert the probability into int for calling API
    weights.push_back(w);
  }

  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  default_random_engine rand_gen(seed);
  discrete_distribution<int> this_distribution(weights.begin(),weights.end());
  return vec_potential_vals.at(this_distribution(rand_gen));//get the final value
}


void DiscreteNode::SetLaplaceSmooth(double alpha) {
  this->laplace_smooth = alpha;
}

double DiscreteNode::GetLaplaceSmooth() {
  return this->laplace_smooth;
}

/**
 * @brief: update counter when meeting new training instances
 * @param instance_of_var_val contains the values of all variables for the new instance
 */
void DiscreteNode::AddInstanceOfVarVal(DiscreteConfig instance_of_var_val) {
  // filter out the variables and values which are not the parents of the current node.
  DiscreteConfig parents_config = GetDiscParConfigGivenAllVarValue(instance_of_var_val);
  // store the new instance in a map
  std::map<int, int> discreteConfigMap = DiscreteConfigToMap(instance_of_var_val);
  // GetNodeIndex() get the id of the current node, the current node is the query variable
  int query_value = discreteConfigMap.at(GetNodeIndex());
  //update probability table of the node with the query value
  AddCount(query_value, parents_config, 1);
}

/**
 * @brief: initialize the conditional probability table
 */
void DiscreteNode::InitializeCPT() {

  if (!HasParents()) {//the node doesn't have a parent
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

/**
 * @brief: update the maps of a node given the variable of the new instance
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

  // If the given instance contains the parent configuration or query value that has not been seen before,
  // return the smallest probability divided by the domain size and number of parents configurations.
  bool unseen_value = (set_discrete_parents_combinations.find(parents_config) == set_discrete_parents_combinations.end() ||
                       std::find(vec_potential_vals.begin(), vec_potential_vals.end(), query_val) == vec_potential_vals.end());
  if (unseen_value) {
    fprintf(stdout, "In function [%s]: the given instance contains the value that has not been seen before.\n", __FUNCTION__);
    double min_prob = 1;
    // traverse all potential values of child & all configurations of parents and compute all the probabilities
    // however, 1. getting probability for the same seen value will do the same computation
    // more importantly, 2. each time getting probabilities for the unseen value will get probabilities of all the seen values...
    // TODO: can we compute the probabilities of all the seen values once and store them?
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
