#ifndef BAYESIANNETWORK_JUNCTIONTREE_H
#define BAYESIANNETWORK_JUNCTIONTREE_H

#include <set>
#include <vector>
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
  set<Clique*> set_clique_ptr_container;//store all the cliques in this Junction Tree
  set<Separator*> set_separator_ptr_container;//all the separators in the Junction tree

  vector<int> elimination_ordering;
//  map<int, Clique*> map_elim_var_to_clique; //key: main variable of a clique; value: the clique


  JunctionTree() = default;
  JunctionTree(Network *net);
  JunctionTree(Network *net, string elim_ord_strategy);
  JunctionTree(Network *net, string elim_ord_strategy, vector<int> custom_elim_ord);
  JunctionTree(JunctionTree*);
  virtual ~JunctionTree() = default;

  void ResetJunctionTree();
  virtual void LoadDiscreteEvidence(DiscreteConfig E);
//  void LoadEvidenceAndMessagePassingUpdateJT(const DiscreteConfig &E);

  void PrintAllCliquesPotentials() const;
  void PrintAllSeparatorsPotentials() const;

  Factor BeliefPropagationCalcuDiscreteVarMarginal(int query_index);
  int InferenceUsingBeliefPropagation(int &query_index);

    virtual double EvaluateAccuracy(Dataset *dts, int num_samp, string alg, bool is_dense);
    int PredictUseJTInfer(DiscreteConfig E, int Y_index, Timer *timer);
    vector<int> PredictUseJTInfer(vector<DiscreteConfig> evidences, int target_node_idx, Timer *timer);

  //==================================================
 protected:
  map<Clique*,Clique> map_cliques_backup;
  map<Separator*,Separator> map_separators_backup;

  void Triangulate(Network *net,
                   int **adjac_matrix,
                   int &num_nodes,
                   vector<int> elim_ord,
                   set<Clique*> &cliques);
//  void FormListShapeJunctionTree(set<Clique*> &cliques);
  void FormJunctionTree(set<Clique*> &cliques);
  void NumberTheCliquesAndSeparators();
  void AssignPotentials(Timer *timer);
  void BackUpJunctionTree();
  virtual void MessagePassingUpdateJT(Timer *timer);
  static vector<int> MinNeighbourElimOrd(int **adjac_matrix, int &num_nodes);
  static void Moralize(int **direc_adjac_matrix, int &num_nodes);
//  void GenMapElimVarToClique();

};


#endif //BAYESIANNETWORK_JUNCTIONTREE_H
