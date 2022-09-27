#include "DiscreteNode.h"

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

/**
 * @brief: obtain the number of probability values in the conditional probability table
 * i.e., (# of potential values of the child) * (# of parent configurations)
 */
int DiscreteNode::GetNumParams() {
  int scale = this->GetNumParentsConfig();//the size of all parent combination (i.e., # of columns of the table)
  return this->GetDomainSize() * scale;
}

void DiscreteNode::PrintProbabilityTable() {//checked
  cout << GetNodeIndex() << ":\t";

  if (this->HasParents()) {    // If this node has parents

    for(int i = 0; i<GetDomainSize(); ++i) {    // For each head variable of CPT (i.e., value of child)
        int query = i;
      auto it = set_discrete_parents_combinations.begin();
      for (int j = 0; j < GetNumParentsConfig(); ++j){  // For tail variables of CPT (i.e., parent configuration)
        DiscreteConfig parcfg = *it;
        string condition = "parent_config_" + to_string(j);
        cout << "P(" << query << '|' << condition << ")=" << GetProbability(query, parcfg) << '\t';
        ++it;
      }
    }
    cout << endl;

  } else {

    DiscreteConfig parcfg;
    for(int i = 0; i < GetDomainSize(); ++i) {    // For each row of CPT
        int query = i;
      cout << "P(" << query << ")=" << GetProbability(query, parcfg) << '\t';
    }
    cout << endl;

  }
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
  int query_value = discreteConfigMap[GetNodeIndex()];
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
            map_cond_prob_table_statistics[i][par_config] = 0;
            map_cond_prob[i][par_config] = -1;
        }
    } else {
        for (const auto &par_config : set_discrete_parents_combinations) {
            map_total_count_under_parents_config[par_config] = 0; // todo
            for (int i = 0; i < GetDomainSize(); ++i) {
                map_cond_prob_table_statistics[i][par_config] = 0;
                map_cond_prob[i][par_config] = -1;
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
    if (map_cond_prob[query_val][parents_config] < 0) {
        int frequency_count =  map_cond_prob_table_statistics[query_val][parents_config]; // P(AB)
        int total = map_total_count_under_parents_config[parents_config]; // P(B)
        map_cond_prob[query_val][parents_config] = (frequency_count + laplace_smooth) / (total + laplace_smooth * GetDomainSize()); // P(A|B) = P(AB) / P(B)
//        double prob = (frequency_count + laplace_smooth) / (total + laplace_smooth * GetDomainSize()); // P(A|B) = P(AB) / P(B)
//        return prob;
    }
    return map_cond_prob[query_val][parents_config];
}

int DiscreteNode::GetNumPotentialVals() {
    return num_potential_vals;
}