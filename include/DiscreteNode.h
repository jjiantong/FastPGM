#ifndef BAYESIANNETWORK_DISCRETENODE_H
#define BAYESIANNETWORK_DISCRETENODE_H


#include <set>
#include <map>
#include <utility>
#include <string>
#include "common.h"
#include "Node.h"

using namespace std;

//class Network;  // Forward declaration.

class DiscreteNode : public Node {//the parent nodes of DiscreteNode must be discrete variables.
public:
    vector<string> vec_str_potential_vals;//the values of some variables are strings (e.g., gender = "male" or "female"); can be improved.

    // =============== refactor like Weka ===============
    // Keep the count instead of probability.

    //"map_cond_prob_table_statistics" and "map_total_count_under_parents_config" together can be used to compute the probability.

    // Key: query variable (child) value; Value: (parents config, joint count).
    // If the node has no parent, then the second dimension, DiscreteConfig (i.e., parents_config) is empty.
    map<int, map<DiscreteConfig, int> > map_cond_prob_table_statistics; // P(AB)
    // Key: query variable (child) value; Value: (parents config, conditional probability).
    map<int, map<DiscreteConfig, double> > map_cond_prob; // P(A|B)

    //Key: parents config. Value: total count.
    map<DiscreteConfig, int> map_total_count_under_parents_config; // P(B)

    void SetLaplaceSmooth(double alpha);//for smoothness when computing probability
    double GetLaplaceSmooth();

    void AddInstanceOfVarVal(DiscreteConfig instance_of_var_val);
    void AddCount(int query_val, DiscreteConfig &parents_config, int count);
    double GetProbability(int query_val, DiscreteConfig &parents_config);

    bool cpt_initialized = false;//cpt stands for "conditional probability table"
    void InitializeCPT();
    // ==================================================

    DiscreteNode();
    explicit DiscreteNode(int index);
    DiscreteNode(const DiscreteNode &n) = default;
    DiscreteNode(DiscreteNode &n) = default;

    int GetDomainSize() const;
    void SetDomainSize(int size);
    void AddParent(Node *node_ptr) override;
    int GetNumParams() override;
    void PrintProbabilityTable();

    int GetNumPotentialVals();

protected:
    int num_potential_vals;
    double laplace_smooth = 1;
};


#endif //BAYESIANNETWORK_DISCRETENODE_H
