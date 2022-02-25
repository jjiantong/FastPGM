//
// Created by LinjianLi on 2019/2/20.
//

#ifndef BAYESIANNETWORK_SEPARATOR_H
#define BAYESIANNETWORK_SEPARATOR_H

#include "Clique.h"

/**
 * @brief: Separator is used like an edge, and the other none separator cliques are nodes in a network.
 */
class Separator : public Clique {
 protected:
  Separator(const Separator&) = default;

 public:
  int weight;//the number of nodes in this separator

//    set<int> old_related_variables;
//    set<DiscreteConfig> set_old_disc_configs;
//  map<DiscreteConfig, double> map_old_potentials;//this member is used when constructing message in separator

//  Factor *old_table;
    Factor old_table;
    PotentialTable old_ptable;

  Separator();
  explicit Separator(set<Node*>);
//  ~Separator();

  Separator* CopyWithoutPtr();

  void UpdateUseMessage(Factor &f, Timer *timer) override;
    void UpdateUseMessage2(PotentialTable &pt, Timer *timer) override;
  void ConstructMessage(Timer *timer) override;
    void ConstructMessage2(Timer *timer) override;
};


#endif //BAYESIANNETWORK_SEPARATOR_H
