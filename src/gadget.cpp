//
// Created by Linjian Li on 2018/12/5.
//

#include "gadget.h"

/**
 * @brief: find the maximum probability of a variable which has multiple potential values.
 * This function is used in inference.
 */
int ArgMax(map<int, double> &x) {
  int argmax = (x.begin())->first;
  double max = (x.begin())->second;
  for (const auto &key_value : x) {
    if (key_value.second >= max) {
      max = key_value.second;
      argmax = key_value.first;
    }
  }
  return argmax;
}

/**
 * @brief: get probabilities; normalise the weights of a variable which has multiple potential values.
 * the input is the same as "ArgMax".
 */
map<int, double> Normalize(map<int, double> &x) {
  double denominator = 0;
  for (const auto &key_value : x) {
      denominator += key_value.second;
  }
  for (const auto &key_value : x) {
    x[key_value.first] /= denominator;
  }
  return x;
}

/**
 * @brief: build a (partial/full) configuration given the variables and their observation.
 * @param nodes_indexes: id of the variable
 * @param observations: value of the variable
 * @param num_of_observations: the length of the two arrays
 */
DiscreteConfig ArrayToDiscreteConfig(int *nodes_indexes, int *observations, int num_of_observations) {
  DiscreteConfig result;
  pair<int, int> p;
  for (int i = 0; i < num_of_observations; ++i) {
    p.first = nodes_indexes[i];
    p.second = observations[i];
    result.insert(p);
  }
  return result;
}

/**
 * @brief: convert a configuration (e.g., a config can be derived from an instance) into a map for fast query
 * set<pair<int, int>> --> map<int, int>
 */
map<int, int> DiscreteConfigToMap(DiscreteConfig &disc_cfg) {
  map<int, int> result;
  for (const auto var_val : disc_cfg) {
    result[var_val.first] = var_val.second;
  }
  if (disc_cfg.size() != result.size()) {
    fprintf(stderr, "Function [%s]: The given DiscreteConfig may contain duplicate keys!", __FUNCTION__);
  }
  return result;
}

/**
 * @brief: for inference; refer to docs in GitHub
 */
set<DiscreteConfig> ExpandConfgFromTwoConfgs(const set<DiscreteConfig> *one, const set<DiscreteConfig> *two) {
  set<int> set_all_vars;
  for (const auto &p : *one->begin()) {
    set_all_vars.insert(p.first);
  }
  for (const auto &p : *two->begin()) {
    set_all_vars.insert(p.first);
  }
  map<int, DiscreteConfig> map_var_to_combs_domain;
  for (const auto &v : set_all_vars) {
    map_var_to_combs_domain[v] = DiscreteConfig();
  }
  for (const auto &c : *one) {
    for (const auto &p : c) {
      map_var_to_combs_domain[p.first].insert(p);
    }
  }
  for (const auto &c : *two) {
    for (const auto &p : c) {
      map_var_to_combs_domain[p.first].insert(p);
    }
  }
  set<DiscreteConfig> set_of_sets;
  for (const auto &kv : map_var_to_combs_domain) {
    set_of_sets.insert(kv.second);
  }
  return GenAllCombinationsFromSets(&set_of_sets);
}

/**
 * @brief: check of the first config is the subset of the second config;
 * used in inference
 */
bool FirstIsSubsetOfSecond(const DiscreteConfig *first, const DiscreteConfig *second) {
  for (const auto &f : *first) {
    if (second->find(f)==second->end()) return false;
  }
  return true;
}

/**
 * @brief: the opposite of Conflict
 */
bool FirstCompatibleSecond(const DiscreteConfig *first, const DiscreteConfig *second) {
  for (const auto &f : *first) {
    for (const auto &s : *second) {
      if (f.first==s.first && f.second!=s.second) return false;
    }
  }
  return true;
}

/**
 * @brief: check if two configurations have a conflict of any shared variable.
 * @return:  false (if they have different values on the same variable) true (else)
 */
bool Conflict(const DiscreteConfig *cfg1, const DiscreteConfig *cfg2) {
  for (const auto &f : *cfg1) {
    for (const auto &s : *cfg2) {
      if (f.first==s.first && f.second!=s.second) {
        return true;
      }
    }
  }
  return false;
}

/**
 * @brief: this function is used in structure learning; the structure of a network can be constrained based on an order.
 * @param: a: the id of a variable
 * @param: b: the id of another variable
 * @return: If a occurs before b in vec, return true. Otherwise, return false.
 */
bool OccurInCorrectOrder(int a, int b, vector<int> vec) {
  bool have_met_a = false, have_met_b = false;
  for (const auto &elem : vec) {
    have_met_a = (have_met_a || (elem==a));
    have_met_b = (have_met_b || (elem==b));
    if (have_met_b && !have_met_a) {
      return false;
    } else if (have_met_b && have_met_a) {
      return true;
    }
  }
  return false;
}

/**
 * @brief: for structure learning
 * @param graph: a set of nodes/variables
 * @param num_nodes: the size of the graph
 * @param ord: a valid order from topological ordering
 * @return: If ord is one of the topological orderings of the graph, return true. Else, return false.
 */
bool DAGObeyOrdering(int **graph, int num_nodes, vector<int> ord) {
  if (num_nodes!=ord.size()) {
    fprintf(stderr, "Error in function [%s]. Number of nodes is not equal to the size of ordering.", __FUNCTION__);
    exit(1);  // return false;
  }
  int *in_degrees = new int[num_nodes](); // The parentheses at end will initialize the array to be all zeros.
  for (int i=0; i<num_nodes; ++i) {
    for (int j=0; j<num_nodes; ++j) {
      if (graph[i][j]==1) { ++in_degrees[j]; }
    }
  }
  for (const auto &o : ord) {
    if (in_degrees[o] != 0) {
      return false;
    }
    for (int j=0; j<num_nodes; ++j) {
      if (graph[o][j]==1) { --in_degrees[j]; }
    }
  }
  return true;
}

/**
 * @brief: for structure learning; check whether a directed graph has a cycle.
 * This function uses BFS, but other graph traversal algorithms can work as well
 */
bool DirectedGraphContainsCircleByBFS(int **graph, int num_nodes) {//TODO: double check correctness
  int visited_count = 0;
  queue<int> que;

  // Calculate the in-degrees of all nodes.
  int *in_degrees = new int[num_nodes](); // The parentheses at end will initialize the array to be all zeros.
  for (int i = 0; i < num_nodes; ++i) {
    for (int j = 0; j < num_nodes; ++j) {
      if (graph[i][j] == 1) { ++in_degrees[j]; }
    }
  }

  for (int i = 0; i < num_nodes; ++i) {
    if (in_degrees[i] == 0) { que.push(i); }
  }

  while (!que.empty()) {
    for (int j = 0; j < num_nodes; ++j) {
      if (graph[que.front()][j] == 1) {
        --in_degrees[j];
        if (in_degrees[j] == 0) { que.push(j); }
      }
    }
    que.pop();
    ++visited_count;
  }

  delete[] in_degrees;
  return (visited_count != num_nodes);
}

/**
 * @brief: obtain a sequence given a graph
 * @return an array containing the node indexes in the order of breadth first traversal
 */
int* BreadthFirstTraversalWithAdjacencyMatrix(int **graph, int num_nodes, int start) {
  int *result = new int[num_nodes];
  int itResult = 0;
  queue<int> que;
  set<int> markSet;
  que.push(start);
  while (!que.empty()) {
    int pos = que.front();
    result[itResult++] = pos;
    markSet.insert(pos);
    que.pop();
    for (int i = 0; i < num_nodes; i++) {
      if (graph[pos][i] != 0 && markSet.find(i) == markSet.end()) {
        que.push(i);
      }
    }
  }
  return result;
}

/**
 * @brief: obtain the node indexes with topological ordering (the first is the node with in-degree = 0)
 * @param graph: 2-d array representation of the adjacency matrix of the graph
 * @param num_nodes: number of the nodes in the graph
 */
vector<int> TopoSortOfDAGZeroInDegreeFirst(int **graph, int num_nodes) {
  vector<int> result;
  queue<int> que;

  // Calculate the in-degrees of all nodes.
  int *in_degrees = new int[num_nodes](); // The parentheses at end will initialize the array to be all zeros.
  for (int i = 0; i < num_nodes; ++i) {
    for (int j = 0; j < num_nodes; ++j) {
      if (graph[i][j] == 1) { ++in_degrees[j]; }
    }
  }

  for (int i = 0; i < num_nodes; ++i) {
    if (in_degrees[i] == 0) { que.push(i); }
  }

  while (!que.empty()) {
    for (int j = 0; j < num_nodes; ++j) {
      if (graph[que.front()][j] == 1) {
        --in_degrees[j];
        if (in_degrees[j] == 0) { que.push(j); }
      }
    }
    result.push_back(que.front());
    que.pop();
  }
  delete[] in_degrees;
  return result;
}


/**
 * @brief: trim all \t, \n, \r and whitespace characters on the right of a string.
 */
string TrimRight(string s) {
  while (!s.empty() && s[s.size()-1]<33) { // ASCII. \t=09, \n=10, \r=13, space=32.
    s.erase(s.size()-1);
  }
  return s;
}

/**
 * @brief: trim all \t, \n, \r and whitespace characters on the left of a string.
 */
string TrimLeft(string s) {
  while (!s.empty() && s[0]<33) { // ASCII. \t=09, \n=10, \r=13, space=32.
    s.erase(0);
  }
  return s;
}

/**
 * @brief: trim all \t, \n, \r and whitespace characters on the left or right of a string.
 */
string Trim(string &s) {
  return TrimLeft( TrimRight(s) );
}

/**
 * @brief: split string s into a vector of strings by the delimiter
 */
vector<string> Split(string &s, string delimiter) {
  vector<string> result;
  size_t begin = 0, end = 0;
  while ((end=s.find_first_of(delimiter, begin)) != string::npos) {
    result.push_back(s.substr(begin, end-begin));
    begin = (++end);
  }
  result.push_back(s.substr(begin, s.size()-begin));
  return result;
}

/**
 * @brief: get the factorial for integer n
 */
int FactorialForSmallInteger(int n) {
  if (n < 0) { return -1; }
  int result = 1;
  while (n > 0) {
    result *= (n--);
  }
  return result;
}

/**
 * @brief: to avoid overflow; get the log of the factorial for integer n
 */
double LogOfFactorial(int n) {
  if (n < 0) { return -1; }
  double result = 0;
  while (n > 1) {
    result += log(n--);
  }
  return result;
}

/**
 * @brief: given the range of each digit, obtain all possible configs (N-ary count)
 */
vector<vector<int>> NaryCount(vector<int> vec_range_each_digit) {//checked on 11/Apr/2020

  int num_digits = vec_range_each_digit.size();
  int num_counts = 1;   // The number of all n-ary counts.
  vector<int> single_count;
  single_count.reserve(num_digits);
  for (int i = 0; i < num_digits; ++i) {
    single_count.push_back(0);
    num_counts *= vec_range_each_digit[i];
  }

  vector<vector<int>> result_counts;
  result_counts.reserve(num_counts);

  // The left-most digit is the most significant digit.
  // The domain of each digit start at 0,
  // so the max value of this digit is one smaller than the range.

  for (int i = 0; i < num_counts; ++i) {
    result_counts.push_back(single_count);
    int check_digit = num_digits-1;

    // Add 1 to count.

    ++single_count[check_digit]; // Add 1 to the least significant digit.

    // Then deal with the carries.

    // The domain of each digit start at 0,
    // so the max value of this digit is one smaller than the range.
    bool need_carry = (single_count[check_digit] >= vec_range_each_digit[check_digit]);
    while (need_carry && check_digit > 0) {
      single_count[check_digit--] = 0;
      ++single_count[check_digit];
      need_carry = (single_count[check_digit] >= vec_range_each_digit[check_digit]);
    }
  }

  return result_counts;
}

/**
 * @brief: obtain the (n+1)th possible config (N-ary count)
 */
vector<int> TheNthNaryCount(vector<int> vec_range_each_digit, int n) {
  // Like decimal to binary.
  // Faster than generating all count first and find the n-th one.
  int num_digits = vec_range_each_digit.size();
  vector<int> result(num_digits, 0);
  int i = num_digits - 1;
  while (n > 0) {
    int radix = vec_range_each_digit.at(i);
    result.at(i--) = n % radix;
    n /= radix;
  }
  return result;
}