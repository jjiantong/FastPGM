//
// Created by LinjianLi on 2019/1/25.
//

#ifndef BAYESIANNETWORK_CUSTOMNETWORK_H
#define BAYESIANNETWORK_CUSTOMNETWORK_H

#include "Trainer.h"
#include "Network.h"
#include "Node.h"
#include "Edge.h"
#include "Factor.h"
#include "gadget.h"
#include <string>
#include <set>
#include <queue>
#include <vector>
#include <cmath>
#include <iostream>

using namespace std;

typedef set< pair<int, int> > Combination;

class CustomNetwork : public Network {
public:

	CustomNetwork() = default;

	/*
	 * Support generating custom network from a file.
	 * The format of the file:
	 *   The first line is the number of nodes.
	 *   Each line from the second line till the end represents a node and its child(ren) separated by white space.
	 *   For example: {
	 *     3
	 *     0 -> 1 2
	 *     1 -> 2
	 *     2 ->
	 *   }
	 *   The above example means that there are 3 nodes.
	 *   n_0 (node 0) has children n_1 and n_2.
	 *   n_1 has child n_2.
	 *   n_2 has no child.
	 */
	void ConstructCustomNetworkFromFile(string);
	void StructLearnCompData(Trainer *);
	pair<int*, int> SimplifyDefaultElimOrd();

};

#endif //BAYESIANNETWORK_CUSTOMNETWORK_H