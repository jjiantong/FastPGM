#ifndef BAYESIANNETWORK_CLIQUE_H
#define BAYESIANNETWORK_CLIQUE_H

#include <set>
#include <map>
#include <utility>
#include <string>
#include <queue>
#include "fastbn/Node.h"
#include "fastbn/Network.h"
#include "fastbn/PotentialTableBase.h"
#include "fastbn/Timer.h"
#include "omp.h"

typedef set< pair<int, int> > DiscreteConfig;

class Clique {
public:
//    bool pure_discrete;
    set <int> clique_variables;
    PotentialTableBase p_table;
    set<Clique*> set_neighbours_ptr; // neighbor cliques

    /**
     * In junction tree algorithm,
     * the "Collect" force messages to flow from downstream to upstream,
     * and the "Distribute" force messages flow from upstream to downstream.
     * So, we need a member to record the upstream of this clique (node).
     */
    Clique *ptr_upstream_clique; // a clique only has one upstream, and all the other neighbours are downstreams.
    vector<Clique *> ptr_downstream_cliques;

    /**
     * Data structures needed for elimination tree
     * (like junction tree) of Gaussian Bayesian network.
     * Proposed in [Local Propagation in Conditional Gaussian Bayesian Networks (Cowell, 2005)]
     * Note that, separators between continuous cliques only retain post_bag but not lp_potential.
     */

    Clique();
    Clique(set<int> set_node_index, Network *net);
    virtual ~Clique() {};

    void Collect2();
    void Collect3(vector<vector<Clique*>> &cliques, int max_level, int num_threads, Timer *timer);

    void Distribute2();
    void Distribute3(vector<vector<Clique*>> &cliques, int max_level, int num_threads);

    virtual void UpdateMessage(const PotentialTableBase &pt);

protected:
    void Distribute2(PotentialTableBase &pt);
};

#endif //BAYESIANNETWORK_CLIQUE_H
