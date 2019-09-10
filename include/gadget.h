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

using namespace std;
typedef set< pair<int, int> > Combination;

set<Combination> GenAllCombFromSets(set<Combination> *);
set<Combination> ExpandCombFromTwoCombs(set<Combination> *one, set<Combination> *two);
bool EachFirstIsInSecond(const Combination *, const Combination *);
bool FirstCompatibleSecond(const Combination *, const Combination *);
bool Conflict(const Combination *, const Combination *);
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
