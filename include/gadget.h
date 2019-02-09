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

using namespace std;
typedef set< pair<int, int> > Combination;

set<Combination> GenAllCombFromSets(set<Combination> *);
bool EachFirstIsInSecond(Combination *, Combination *);
bool FirstCompatibleSecond(Combination *, Combination *);
int* WidthFirstTraversalWithAdjacencyMatrix(int **, int, int);

string TrimRight(string);
string TrimLeft(string);
string Trim(string &);
vector<string> Split(string &, string);

#endif //BAYESIANNETWORK_GADGET_H
