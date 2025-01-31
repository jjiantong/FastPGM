//
// Created by jjt on 12/09/22.
//

#ifndef BAYESIANNETWORK_JUNCTIONTREESTRUCTURE_H
#define BAYESIANNETWORK_JUNCTIONTREESTRUCTURE_H

#include "fastpgm/inference/Clique.h"
#include "Separator.h"
#include "fastpgm/Network.h"

/**
 * @brief: this class is to transform the BN into a secondary structure called junction tree.
 * it is used in the junction tree algorithm
 */
class JunctionTreeStructure {
public:
    Network *network;//the learned network which can be used for inference
    vector<Clique*> vector_clique_ptr_container;//store all the cliques in this Junction Tree
    vector<Separator*> vector_separator_ptr_container;//all the separators in the Junction tree

    JunctionTreeStructure() = default;
    JunctionTreeStructure(Network *net, int classification);
    ~JunctionTreeStructure();

protected:
    /**
     * functions for junction tree construction
     */
    static void Moralize(int **direc_adjac_matrix, int &num_nodes);
    void Triangulate(Network *net, int **adjac_matrix, vector<bool> &has_processed);
    void FormJunctionTree();
    void AssignPotentials(int classification);
    float GetAveAndMaxCliqueSize(int &max_size);
};

#endif //BAYESIANNETWORK_JUNCTIONTREESTRUCTURE_H
