//
// Created by Linjian Li on 2018/12/5.
//

#include "gadget.h"


set<DiscreteConfig> GenAllCombFromSets(set<DiscreteConfig> *set_of_sets) {

  // Error Case
//  if (set_of_sets->empty()) {
//    fprintf(stderr, "Error in function %s! \nThe size of set_of_sets is less than 1", __FUNCTION__);
//    exit(1);
//  }

  if (set_of_sets->empty()) { return set<DiscreteConfig>(); }

  auto its=set_of_sets->begin();
  DiscreteConfig to_be_inserted = *its;
  set<DiscreteConfig> result, temp_result;

  // Base Case
  if (set_of_sets->size()==1) {
    for (auto &p : to_be_inserted){
      DiscreteConfig c;
      c.insert(p);
      result.insert(c);
    }
    return result;
  }

  // Recursive Case (the size of set_of_sets is greater than 1)
  set_of_sets->erase(its);
  temp_result = GenAllCombFromSets(set_of_sets);
  for (auto &p : to_be_inserted){
    for (DiscreteConfig c : temp_result) {
      c.insert(p);
      result.insert(c);
    }
  }
  return result;
}


set<DiscreteConfig> ExpandCombFromTwoCombs(set<DiscreteConfig> *one, set<DiscreteConfig> *two) {
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
  return GenAllCombFromSets(&set_of_sets);
}


bool EachFirstIsInSecond(const DiscreteConfig *first, const DiscreteConfig *second) {
  for (const auto &f : *first) {
    if (second->find(f)==second->end()) return false;
  }
  return true;
}


bool FirstCompatibleSecond(const DiscreteConfig *first, const DiscreteConfig *second) {
  for (const auto &f : *first) {
    for (const auto &s : *second) {
      if (f.first==s.first && f.second!=s.second) return false;
    }
  }
  return true;
}


bool Conflict(const DiscreteConfig *first, const DiscreteConfig *second) {
  for (const auto &f : *first) {
    for (const auto &s : *second) {
      if (f.first==s.first && f.second!=s.second) {
        return true;
      }
    }
  }
  return false;
}

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


int* WidthFirstTraversalWithAdjacencyMatrix(int **graph, int num_nodes, int start) {
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
    for (int i=0; i<num_nodes; i++) {
      if (graph[pos][i]!=0 && markSet.find(i)==markSet.end()) {
        que.push(i);
      }
    }
  }
  return result;
}


vector<int> TopoSortOfDAGZeroInDegreeFirst(int **graph, int num_nodes) {
  vector<int> result;
  queue<int> que;

  int *in_degrees = new int[num_nodes](); // The parentheses at end will initialize the array to be all zeros.
  for (int i=0; i<num_nodes; ++i) {
    for (int j=0; j<num_nodes; ++j) {
      if (graph[i][j]==1) {++in_degrees[j];}
    }
  }

  for (int i=0; i<num_nodes; ++i) {
    if (in_degrees[i]==0) {que.push(i);}
  }
  while (!que.empty()) {
    for (int j=0; j<num_nodes; ++j) {
      if (graph[que.front()][j]==1) {
        --in_degrees[j];
        if (in_degrees[j]==0) {que.push(j);}
      }
    }
    result.push_back(que.front());
    que.pop();
  }
  delete[] in_degrees;
  return result;
}


string TrimRight(string s) {
  while (!s.empty() && s[s.size()-1]<33) { // ASCII. \t=09, \n=10, \r=13, space=32.
    s.erase(s.size()-1);
  }
  return s;
}


string TrimLeft(string s) {
  while (!s.empty() && s[0]<33) { // ASCII. \t=09, \n=10, \r=13, space=32.
    s.erase(0);
  }
  return s;
}


string Trim(string &s) {
  return TrimLeft( TrimRight(s) );
}


vector<string> Split(string &s, string delimiter) {
  vector<string> result;
  size_t begin=0, end=0;
  while ((end=s.find_first_of(delimiter, begin))!=string::npos) {
    result.push_back(s.substr(begin, end-begin));
    begin = ++end;
  }
  result.push_back(s.substr(begin, s.size()-begin));
  return result;
}

int FactorialForSmallInteger(int n) {
  if (n<0) { return -1; }
  int result = 1;
  while (n>0) {
    result *= n--;
  }
  return result;
}

double LogOfFactorial(int n) {
  if (n<0) { return -1; }
  double result = 0;
  while (n>1) {
    result += log(n--);
  }
  return result;
}


vector<vector<int>> NaryCount(vector<int> vec_range_each_digit) {
  vector<vector<int>> result_counts;
  int num_digits = vec_range_each_digit.size();
  int num_counts = 1;
  vector<int> single_count;
  single_count.reserve(num_digits);
  for (int i=0; i<num_digits; ++i) {
    single_count.push_back(0);
    num_counts *= vec_range_each_digit[i];
  }
  result_counts.reserve(num_counts);

  // The left-most digit is the most significant digit.
  // The domain of each digit start at 0,
  // so the max value of this digit is one smaller than the range.
  
  // While it does not overflow.
  while (single_count[0]<vec_range_each_digit[0]) {
    result_counts.push_back(single_count);
    int check_digit = num_digits-1;

    // Add 1 to count.

    ++single_count[check_digit]; // Add 1 to the least significant digit.

    // Then deal with the carries.

    // The domain of each digit start at 0,
    // so the max value of this digit is one smaller than the range.
    bool need_carry = single_count[check_digit]>=vec_range_each_digit[check_digit];
    while (need_carry && check_digit>0) {
      single_count[check_digit--] = 0;
      ++single_count[check_digit];
      need_carry = single_count[check_digit]>=vec_range_each_digit[check_digit];
    }
  }

  return result_counts;
}