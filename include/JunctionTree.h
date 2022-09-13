#ifndef BAYESIANNETWORK_JUNCTIONTREE_H
#define BAYESIANNETWORK_JUNCTIONTREE_H

#include <set>
#include <vector>
#include <algorithm>
#include "Clique.h"
#include "Separator.h"
#include "Network.h"
#include "Inference.h"
#include "JunctionTreeStructure.h"

/**
 * @brief: this class is for exact inference algorithm junction tree algorithm (JT).
 * JT first convert the BN into a junction tree structure,
 * then performs message passing along the junction tree.
 */
class JunctionTree: public Inference {
    //==================================================
public:
    Network *network;//the learned network which can be used for inference
    JunctionTreeStructure *tree;
    Clique *arb_root;

    vector<vector<Clique*>> nodes_by_level;
    vector<vector<Separator*>> separators_by_level;
    int max_level;

    JunctionTree() = default;
    JunctionTree(Network *net);
    ~JunctionTree();

    void ResetJunctionTree();
    virtual void LoadDiscreteEvidence(const DiscreteConfig &E, int num_threads, Timer *timer);
    void LoadEvidenceToNodes(vector<Clique*> &vector_reduced_node_ptr,
                             int index, int value_index, int num_threads, Timer *timer);

    PotentialTable BeliefPropagationCalcuDiscreteVarMarginal2(int query_index);
    int InferenceUsingBeliefPropagation(int &query_index);

    virtual double EvaluateAccuracy(Dataset *dts, int num_threads, int num_samp, string alg, bool is_dense);
    int PredictUseJTInfer(const DiscreteConfig &E, int Y_index, int num_threads, Timer *timer);
    vector<int> PredictUseJTInfer(const vector<DiscreteConfig> &evidences, int target_node_idx, int num_threads, Timer *timer);

    //==================================================
protected:
    Clique* clique_backup; // use an array to backup cliques
    Separator* separator_backup; // use an array to backup separators

    void MarkLevel();
    void BackUpJunctionTree();

    virtual void MessagePassingUpdateJT(int num_threads, Timer *timer);
    void Collect(int num_threads, Timer *timer);
    void Distribute(int num_threads, Timer *timer);
    void SeparatorLevelOperation(bool is_collect, int i, int num_threads, Timer *timer);
};

#endif //BAYESIANNETWORK_JUNCTIONTREE_H
