#ifndef BAYESIANNETWORK_JUNCTIONTREE_H
#define BAYESIANNETWORK_JUNCTIONTREE_H

#include <set>
#include <vector>
#include <algorithm>
#include "common.h"
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
//    Network *network;//the learned network which can be used for inference
    JunctionTreeStructure *tree;
    Clique *jt_root;

    vector<vector<Clique*>> nodes_by_level;
    vector<vector<Separator*>> separators_by_level;
    int max_level;

    // 1d: number of nodes in the network; 2d: number of possible values of this node
    vector<vector<double>> probs_one_sample;


    JunctionTree() = default;
    JunctionTree(Network *net, Dataset *dts, bool is_dense);
    ~JunctionTree();

    virtual double EvaluateAccuracy(string path, int num_threads);

    void ResetJunctionTree();

protected:
    Clique* clique_backup; // use an array to backup cliques
    Separator* separator_backup; // use an array to backup separators

    int GetMaxLevel(Clique *root);
    void MarkLevel();
    void ReorganizeTableStorage(int num_threads);
    void BackUpJunctionTree();

    void LoadDiscreteEvidence(const DiscreteConfig &E, int num_threads, Timer *timer);
    void LoadEvidenceToNodes(vector<Clique*> &vector_reduced_node_ptr,
                             int index, int value_index, int num_threads, Timer *timer);
    void LoadEvidenceToNodesOptimized(vector<Clique*> &vector_reduced_node_ptr,
                                      int index, int value_index, int num_threads, Timer *timer);
    void MessagePassingUpdateJT(int num_threads, Timer *timer);

    void Collect(int num_threads, Timer *timer);
    void Distribute(int num_threads, Timer *timer);

    void SeparatorLevelCollection(int i, int num_threads, Timer *timer);
    void SeparatorLevelDistribution(int i, int num_threads, Timer *timer);
    void CliqueLevelCollection(int i, const vector<int> &has_kth_child, int k, int num_threads, Timer *timer);
    void CliqueLevelDistribution(int i, int num_threads, Timer *timer);

    void SeparatorLevelCollectionOptimized(int i, int num_threads, Timer *timer);
    void CliqueLevelDistributionOptimized(int i, int num_threads, Timer *timer);

    PotentialTable CalculateMarginalProbability();
    void GetProbabilitiesAllNodes(const DiscreteConfig &E);
    void GetProbabilitiesOneNode(const DiscreteConfig &E, int index);

    int InferenceUsingJT();
    int PredictUseJTInfer(const DiscreteConfig &E, int instance_index, double &mse, double &hd, int num_threads, Timer *timer);
    vector<int> PredictUseJTInfer(int num_threads, Timer *timer);

    /**
     * @brief: compute 2d index (j,k) according to 1d index s
     */
    inline void Compute2DIndex(int &j, int &k, int &s, int &size, int *cum_sum) {
        for (int m = size - 1; m >= 0; --m) {
            if (s >= cum_sum[m]) {
                j = m;
                break;
            }
        }
        k = s - cum_sum[j];
    }
};

#endif //BAYESIANNETWORK_JUNCTIONTREE_H
