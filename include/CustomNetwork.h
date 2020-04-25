//
// Created by LinjianLi on 2019/1/25.
//

#ifndef BAYESIANNETWORK_CUSTOMNETWORK_H
#define BAYESIANNETWORK_CUSTOMNETWORK_H

#include "Dataset.h"
#include "Network.h"
#include "Node.h"
#include "Factor.h"
#include "XMLBIFParser.h"
#include "gadget.h"
#include <string>
#include <set>
#include <queue>
#include <vector>
#include <cmath>
#include <iostream>

using namespace std;

typedef set< pair<int, int> > DiscreteConfig;//discrete variables and their corresponding values.
//DiscreteConfig is for storing the "condition" of a conditional probability.
class CustomNetwork : public Network {//CustomNetwork may be a tree shape graph or a generic graph (containing undirected loop).
 public:

  CustomNetwork();
  explicit CustomNetwork(bool pure_disc);

  /**
   * hasn't implemented due to the complexity (e.g., undirected loop)
   * the simplification of the network is for variable elimination
   */
  vector<int> SimplifyDefaultElimOrd(DiscreteConfig evidence) override;

  void GetNetFromXMLBIFFile(string file_path);//construct the network using content from the xml file.

};

#endif //BAYESIANNETWORK_CUSTOMNETWORK_H