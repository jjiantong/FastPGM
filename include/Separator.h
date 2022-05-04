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

    bool is_in_jt;

    Factor old_table;
    PotentialTable old_ptable;

  Separator();
  explicit Separator(set<Node*>);
    explicit Separator(set<int>, Network*);
    ~Separator() {};

  Separator* CopyWithoutPtr();

  void UpdateUseMessage(const Factor &f, Timer *timer) override;
  void ConstructMessage(Timer *timer) override;

    void UpdateUseMessage2(const PotentialTable &pt, Timer *timer) override;
    void ConstructMessage2(Timer *timer) override;
};

#endif //BAYESIANNETWORK_SEPARATOR_H
