//
// Created by jjt on 2021/6/17.
//

#ifndef BAYESIANNETWORK_STRUCTURELEARNING_H
#define BAYESIANNETWORK_STRUCTURELEARNING_H

#include "fastbo/Network.h"
#include "fastbo/Dataset.h"
#include "fastbo/DiscreteNode.h"


class StructureLearning {
public:
    Network *network; // the network to be learned

    virtual ~StructureLearning() {};

    /**
     * @param group_size and @param num_threads are used only for PCStable
     */
    virtual void StructLearnCompData(Dataset *dts, int group_size, int num_threads,
                                     bool dag, bool add_root, bool save_struct, string struct_file, int verbose) = 0;
    void AssignNodeInformation(Dataset *dts);
    vector<int> AssignNodeOrder(string topo_ord_constraint);

    bool Direct(int a, int c);
    vector<int> GetCommonAdjacents(const map<int, map<int, double>> &adjacencies, int x_idx, int y_idx);

    void DirectLeftEdges(const map<int, map<int, double>> &adjacencies);
    vector<int> FindRootsInDAGForest();
    void AddRootNode(vector<int> &sub_roots);

    void SaveBNStructure(string file);
};

#endif //BAYESIANNETWORK_STRUCTURELEARNING_H
