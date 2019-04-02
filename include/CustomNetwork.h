//
// Created by LinjianLi on 2019/1/25.
//

#ifndef BAYESIANNETWORK_CUSTOMNETWORK_H
#define BAYESIANNETWORK_CUSTOMNETWORK_H

#include "Trainer.h"
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

typedef set< pair<int, int> > Combination;

class CustomNetwork : public Network {
 public:

  CustomNetwork() = default;

  void StructLearnCompData(Trainer *);
  pair<int*, int> SimplifyDefaultElimOrd(Combination) override;

  void GetNetFromXMLBIFFile(string);

};

#endif //BAYESIANNETWORK_CUSTOMNETWORK_H