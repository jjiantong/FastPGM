#ifndef BAYESIANNETWORK_JUNCTIONTREE_H
#define BAYESIANNETWORK_JUNCTIONTREE_H

#include <set>
#include <vector>
#include <algorithm>
#include "Clique.h"
#include "Separator.h"
#include "Network.h"
#include "Inference.h"

/**
 * @brief: this class is for exact inference. The other two methods include brute force and variable elimination.
 */
class JunctionTree: public Inference {
    //==================================================
public:
    Network *network;//the learned network which can be used for inference
    vector<Clique*> vector_clique_ptr_container;//store all the cliques in this Junction Tree
    vector<Separator*> vector_separator_ptr_container;//all the separators in the Junction tree

    vector<int> elimination_ordering;
//  map<int, Clique*> map_elim_var_to_clique; //key: main variable of a clique; value: the clique

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

    Factor BeliefPropagationCalcuDiscreteVarMarginal(int query_index);
    PotentialTable BeliefPropagationCalcuDiscreteVarMarginal2(int query_index);
    int InferenceUsingBeliefPropagation(int &query_index);

    virtual double EvaluateAccuracy(Dataset *dts, int num_threads, int num_samp, string alg, bool is_dense);
    int PredictUseJTInfer(const DiscreteConfig &E, int Y_index, int num_threads, Timer *timer);
    vector<int> PredictUseJTInfer(const vector<DiscreteConfig> &evidences, int target_node_idx, int num_threads, Timer *timer);

    int GetIndexByCliquePtr(Clique* clq);

    //==================================================
protected:
    Clique* clique_backup; // use an array to backup cliques
    Separator* separator_backup; // use an array to backup separators
//  map<Clique*,Clique> map_cliques_backup;
//  map<Separator*,Separator> map_separators_backup;

//  void Triangulate(Network *net, int **adjac_matrix, vector<int> elim_ord);
    void Triangulate(Network *net, int **adjac_matrix, vector<bool> &has_processed);
//  void FormListShapeJunctionTree(set<Clique*> &cliques);
    void FormJunctionTree();
//  void CliqueMerging(int low, int high);
    void NumberTheCliquesAndSeparators();
    void AssignPotentials();
    float GetAveAndMaxCliqueSize(int &max_size);
    void MarkLevel();
    void BackUpJunctionTree();

    virtual void MessagePassingUpdateJT(int num_threads, Timer *timer);
    void Collect(int num_threads, Timer *timer);
    void Distribute(int num_threads, Timer *timer);
    void SeparatorLevelOperation(bool is_collect, int i, int num_threads, Timer *timer);

//  static vector<int> MinNeighbourElimOrd(int **adjac_matrix, int &num_nodes);
    static void Moralize(int **direc_adjac_matrix, int &num_nodes);
//  void GenMapElimVarToClique();
};

#endif //BAYESIANNETWORK_JUNCTIONTREE_H
