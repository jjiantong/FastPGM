#ifndef BAYESIANNETWORK_SEPARATOR_H
#define BAYESIANNETWORK_SEPARATOR_H

#include <algorithm>
#include "fastbo/inference/Clique.h"

/**
 * @brief: Separator is used like an edge, and the other none separator cliques are nodes in a network.
 */
class Separator : public Clique {
public:
    bool is_in_jt;
    PotentialTableBase old_ptable;

    Separator();
    explicit Separator(set<int>, Network*);
    ~Separator() {};

    void UpdateMessage(const PotentialTableBase &pt) override;
};

#endif //BAYESIANNETWORK_SEPARATOR_H
