//
// Created by Linjian Li on 2018/12/5.
//

#ifndef BAYESIANNETWORK_GADGET_H
#define BAYESIANNETWORK_GADGET_H

#include <set>
#include <utility>
#include <string>
#include <queue>
#include <iostream>

using namespace std;
typedef set< pair<int, int> > Combination;

set<Combination> generate_all_possible_combinations_from_several_sets(set<Combination>*);
bool full_first_is_compatible_with_partial_second(Combination*, Combination*);
bool partial_first_is_compatible_with_partial_second_on_common_variable(Combination*, Combination*);
int* widthFirstTraversalWithAdjacencyMatrix(int **, int, int);

#endif //BAYESIANNETWORK_GADGET_H
