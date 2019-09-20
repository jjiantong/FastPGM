//
// Created by Linjian Li on 2018/12/5.
//

#ifndef BAYESIANNETWORK_GADGET_H
#define BAYESIANNETWORK_GADGET_H

#include <set>
#include <utility>
#include <string>
#include <queue>
#include <vector>
#include <map>
#include <iostream>
#include <cmath>
#include <cfloat>

using namespace std;
typedef set< pair<int, int> > DiscreteConfig;

struct Value {
 private:
  int int_ = INT32_MIN;
  float float_ = -FLT_MAX;
  bool use_int = true;
 public:
  bool UseInt() const { return use_int; }
  void SetInt(int i) { int_ = i; float_ = -FLT_MAX; use_int = true; }
  void SetFloat(float f) { float_ = f; int_ = INT32_MIN; use_int = false; }
  int GetInt() const { if (!use_int) exit(1); return int_; }
  float GetFloat() const { if (use_int) exit(1); return float_; }
  bool operator <(const Value v) const {
    if (this->UseInt() && v.UseInt()) {
      return this->GetInt() < v.GetInt();
    } else if (this->UseInt() && !v.UseInt()) {
      return this->GetInt() < v.GetFloat();
    } else if (!this->UseInt() && v.UseInt()) {
      return this->GetFloat() < v.GetInt();
    } else {
      return this->GetFloat() < v.GetFloat();
    }
  }

};

typedef pair<int, Value> VarVal;

typedef set<VarVal> Configuration;

set<DiscreteConfig> GenAllConfgFromSets(set<DiscreteConfig> *set_of_sets);
set<DiscreteConfig> ExpandConfgFromTwoConfgs(set<DiscreteConfig> *one, set<DiscreteConfig> *two);
bool FirstIsSubsetOfSecond(const DiscreteConfig *first, const DiscreteConfig *second);
bool FirstCompatibleSecond(const DiscreteConfig *, const DiscreteConfig *);
bool Conflict(const DiscreteConfig *cfg1, const DiscreteConfig *cfg2);
bool OccurInCorrectOrder(int a, int b, vector<int> vec);
int* WidthFirstTraversalWithAdjacencyMatrix(int **graph, int num_nodes, int start);
vector<int> TopoSortOfDAGZeroInDegreeFirst(int **graph, int num_nodes);

string TrimRight(string);
string TrimLeft(string);
string Trim(string &);
vector<string> Split(string &s, string delimiter);

int FactorialForSmallInteger(int);
double LogOfFactorial(int);

vector<vector<int>> NaryCount(vector<int> vec_range_each_digit);

#endif //BAYESIANNETWORK_GADGET_H
