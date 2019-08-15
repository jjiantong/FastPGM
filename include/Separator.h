//
// Created by LinjianLi on 2019/2/20.
//

#ifndef BAYESIANNETWORK_SEPARATOR_H
#define BAYESIANNETWORK_SEPARATOR_H

#include "Clique.h"

class Separator : public Clique {
 protected:
  Separator(const Separator&) = default;

 public:
  int weight;
  map<Combination, double> map_old_potentials;

  Separator() = default;
  Separator(set<Node*>);

  Separator* CopyWithoutPtr();

  void UpdateUseMessage(Factor) override;
  Factor ConstructMessage() override;


};


#endif //BAYESIANNETWORK_SEPARATOR_H
