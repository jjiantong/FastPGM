//
// Created by LinjianLi on 2019/2/9.
//

#include "DiscreteNode.h"

DiscreteNode::DiscreteNode() {
  is_discrete = true;
}

DiscreteNode::DiscreteNode(int index) {
  is_discrete = true;
  SetNodeIndex(index);
}

DiscreteNode::DiscreteNode(int index, string name): DiscreteNode(index) {
  node_name = name;
}

void DiscreteNode::SetDomain(vector<string> str_domain) {
  num_potential_vals = str_domain.size();
  vec_str_potential_vals = str_domain;
  for (const auto &s : str_domain) {
    vec_potential_vals.push_back(vec_potential_vals.size());
  }
  potential_vals = new int[num_potential_vals];
  for (int i=0; i<num_potential_vals; ++i) {
    potential_vals[i] = i;
  }
}

void DiscreteNode::SetDomain(vector<int> int_domain) {
  num_potential_vals = int_domain.size();
  vec_potential_vals = int_domain;
  potential_vals = new int[num_potential_vals];
  for (int i=0; i<num_potential_vals; ++i) {
    potential_vals[i] = int_domain.at(i);
  }
}

void DiscreteNode::AddParent(Node *p) {
  if (!p->is_discrete) {
    fprintf(stderr, "Error in function %s! \n"
                    "Discrete node must not have continuous parent!", __FUNCTION__);
    exit(1);
  }
  set_parents_ptrs.insert(p);
}

void DiscreteNode::ClearParams() {
  if (set_parents_ptrs.empty()) {
    for (auto &kv : map_marg_prob_table) {
      kv.second = 0;
    }
  } else {
    for (auto &kv : map_cond_prob_table) {
      for (auto &kv2 : kv.second) {
        kv2.second = 0;
      }
    }
  }
}


void DiscreteNode::PrintProbabilityTable() {
  cout << GetNodeIndex() << ":\t";

  if (set_parents_ptrs.empty()) {    // If this node has no parents
    for(int i=0; i<num_potential_vals; ++i) {    // For each row of MPT
      int query = potential_vals[i];
      cout << "P(" << query << ")=" << map_marg_prob_table[query] << '\t';
    }
    cout << endl;

  } else {  // If this node has parents

    for(int i=0; i<num_potential_vals; ++i) {    // For each row of CPT
      int query = potential_vals[i];
      for (const auto &comb : set_discrete_parents_combinations) {  // For each column of CPT
        string condition;
        for (auto &p : comb) {
          condition += ("\"" + to_string(p.first) + "\"=" + to_string(p.second));
        }
        cout << "P(" << query << '|' << condition << ")=" << map_cond_prob_table[query][comb] << '\t';
      }
    }
    cout << endl;
  }
}

int DiscreteNode::SampleNodeGivenParents(DiscreteConfig evidence) {
  // The evidence should contain all parents of this node.
  // The evidence about other nodes (including children) are IGNORED!!!
  set<int> set_par_indexes;
  for (auto &par : set_parents_ptrs) {
    set_par_indexes.insert(par->GetNodeIndex());
  }
  DiscreteConfig par_evi;
  for (auto &e : evidence) {
    if (set_par_indexes.find(e.first)!=set_par_indexes.end()) {
      par_evi.insert(e);
    }
  }

  vector<int> weights;
  if (par_evi.empty()) {
    for (int i=0; i<num_potential_vals; ++i) {
      int w = (int)(map_marg_prob_table[potential_vals[i]]*10000);
      weights.push_back(w);
    }
  } else {
    for (int i=0; i<num_potential_vals; ++i) {
      int w = (int)(map_cond_prob_table[potential_vals[i]][par_evi]*10000);
      weights.push_back(w);
    }
  }

  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  default_random_engine rand_gen(seed);
  discrete_distribution<int> this_distribution(weights.begin(),weights.end());
  return potential_vals[this_distribution(rand_gen)];
}