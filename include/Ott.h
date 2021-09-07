//
// Created by jjt on 2021/6/17.
//

#ifndef BAYESIANNETWORK_OTT_H
#define BAYESIANNETWORK_OTT_H

#include "StructureLearning.h"
#include "ScoreFunction.h"

class Ott: public StructureLearning {
public:
    string order_constraint;
    vector<int> order;

    Ott(Network *net) {network = net;};
    Ott(Network *net, string oc) {network = net; order_constraint = oc;};

    virtual void StructLearnCompData(Dataset *dts, int group_size, int num_threads, bool print_struct, bool verbose);

    void StructLearnByOtt(Dataset *dts);
    pair<double, set<Node*>> F(Node *node, set<Node*> &candidate_parents, Dataset *dts,
                               map<Node*, map<set<Node*>, double>> &dynamic_program);
    pair<double, vector<pair<Node*, set<Node*>>>> Q(set<Node*> &set_nodes, vector<int> topo_ord, Dataset *dts,
                                                    map<Node*, map<set<Node*>, double>> &dynamic_program_for_F,
                                                    map<pair<set<Node*>, vector<int>>,
                                                    pair<double, vector<pair<Node*, set<Node*>>>>> dynamic_program_for_Q);
    vector<int> M(set<Node*> &set_nodes, Dataset *dts, map<Node*, map<set<Node*>, double>> &dynamic_program_for_F,
                  map<pair<set<Node*>, vector<int>>, pair<double, vector<pair<Node*, set<Node*>>>>> dynamic_program_for_Q,
                  map<set<Node*>, vector<int>> dynamic_program_for_M);
};

#endif //BAYESIANNETWORK_OTT_H
