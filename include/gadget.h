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
#include <iostream>
#include <cmath>

using namespace std;
typedef set< pair<int, int> > Combination;

set<Combination> GenAllCombFromSets(set<Combination> *);
bool EachFirstIsInSecond(Combination *, Combination *);
bool FirstCompatibleSecond(Combination *, Combination *);
bool Conflict(Combination *, Combination *);
int* WidthFirstTraversalWithAdjacencyMatrix(int **, int, int);
vector<int> TopoSortOfDAGZeroInDegreeFirst(int **, int);

string TrimRight(string);
string TrimLeft(string);
string Trim(string &);
vector<string> Split(string &, string);

int FactorialForSmallInteger(int);
double LogOfFactorial(int);

vector<vector<int>> NaryCount(vector<int>);

#endif //BAYESIANNETWORK_GADGET_H
