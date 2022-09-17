#ifndef BAYESIANNETWORK_SEPARATOR_H
#define BAYESIANNETWORK_SEPARATOR_H

#include "Clique.h"

/**
 * @brief: Separator is used like an edge, and the other none separator cliques are nodes in a network.
 */
class Separator : public Clique {
public:
    int weight;//the number of nodes in this separator
    bool is_in_jt;
    PotentialTable old_ptable;

    Separator();
    explicit Separator(set<int>, Network*);
    ~Separator() {};

    void UpdateMessage(const PotentialTable &pt) override;
};

#endif //BAYESIANNETWORK_SEPARATOR_H
