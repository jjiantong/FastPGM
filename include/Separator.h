//
// Created by LinjianLi on 2019/2/20.
//

#ifndef BAYESIANNETWORK_SEPARATOR_H
#define BAYESIANNETWORK_SEPARATOR_H

#include "Clique.h"

class Separator : public Clique {
 public:
  int weight;
  Separator(set<Node*>);
};


#endif //BAYESIANNETWORK_SEPARATOR_H
