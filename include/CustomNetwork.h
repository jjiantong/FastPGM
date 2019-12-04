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

typedef set< pair<int, int> > DiscreteConfig;

class CustomNetwork : public Network {
 public:

  CustomNetwork();
  explicit CustomNetwork(bool pure_disc);

  vector<int> SimplifyDefaultElimOrd(DiscreteConfig evidence) override;

  void GetNetFromXMLBIFFile(string file_path);

};

#endif //BAYESIANNETWORK_CUSTOMNETWORK_H