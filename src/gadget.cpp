//
// Created by Linjian Li on 2018/12/5.
//

#include "gadget.h"


set<Combination> GenAllCombFromSets(set<Combination> *set_of_sets) {

  // Error Case
  if (set_of_sets->empty()) {
    fprintf(stderr, "Error in function %s! \nThe size of set_of_sets is less than 1", __FUNCTION__);
    exit(1);
  }

  auto its=set_of_sets->begin();
  Combination to_be_inserted = *its;
  set<Combination> result, temp_result;

  // Base Case
  if (set_of_sets->size()==1) {
    for (auto &p : to_be_inserted){
      Combination c;
      c.insert(p);
      result.insert(c);
    }
    return result;
  }

  // Recursive Case (the size of set_of_sets is greater than 1)
  set_of_sets->erase(its);
  temp_result = GenAllCombFromSets(set_of_sets);
  for (auto &p : to_be_inserted){
    for (Combination c : temp_result) {
      c.insert(p);
      result.insert(c);
    }
  }
  return result;
}


bool EachFirstIsInSecond(Combination *first, Combination *second) {
  for (auto &f : *first) {
    if (second->find(f)==second->end()) return false;
  }
  return true;
}


bool FirstCompatibleSecond(Combination *first, Combination *second) {
  for (auto &f : *first) {
    for (auto &s : *second) {
      if (f.first==s.first && f.second!=s.second) return false;
    }
  }
  return true;
}


bool Conflict(Combination *first, Combination *second) {
  for (auto &f : *first) {
    for (auto &s : *second) {
      if (f.first==s.first && f.second!=s.second) return true;
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

  int count = 0;
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


string Trim(string s) {
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

inline int FactorialForSmallInteger(int n) {
  int result = 1;
  while (n>0) {
    result *= n--;
  }
  return result;
}

