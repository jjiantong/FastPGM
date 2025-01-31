#ifndef BAYESIANNETWORK_NETWORK_H
#define BAYESIANNETWORK_NETWORK_H

#include "common.h"
#include "Dataset.h"
#include "Node.h"
#include "Edge.h"
#include "PotentialTableBase.h"
#include <string>
#include <set>
#include <queue>
#include <vector>
#include <cmath>
#include <iostream>
#include <random>
#include <chrono>
#include <sys/time.h>
#include <locale.h>
#include <algorithm>
//#include <bits/stdc++.h>
#include <climits>
#include "omp.h"

using namespace std;

class PotentialTableBase;
class Dataset;
class Network {//this class is used by both the customized networks and networks learning from data.
public:
    string network_name;//a name for each bayesian network (usually an xml file contains a name for the network).
    int num_nodes = 0;
    int num_edges = 0;
    bool pure_discrete;
    vector<int> topo_ord;
    /** the default value to infer is the first (i.e. root) variable **/
    vector<int> vec_default_elim_ord;//the elimination order is obtained by reverse topological sort.

    map<int, Node*> map_idx_node_ptr;  // Key: node index. Value: node pointer. This map is a helper for FindNodePtrByIndex.
    vector<Edge> vec_edges;  // edges in the network

    vector<PotentialTableBase> pts; // potential tables for each node

    Network();
    explicit Network(bool pure_disc);
    Network(Network &net);
    Network(vector<Node*> nodes, string name);
    virtual ~Network();

    Node* FindNodePtrByIndex(const int &index) const;
    Node* FindNodePtrByName(const string &name) const;

    int GetUndirectedEdge(Node* node1, Node* node2);
    int GetDirectedEdge(Node* node1, Node* node2);
    int GetEdge(Node* node1, Node* node2);
    int GetDirectedEdgeFromEdgeOrder(const vector<Edge> &edge_order, Node* node1, Node* node2);
    void PrintEdges();
    void CheckEdges();

    bool AddDirectedEdge(int p_index, int c_index);
    bool DeleteDirectedEdge(int p_index, int c_index);
    void AddUndirectedEdge(int p_index, int c_index);
    bool DeleteUndirectedEdge(int p_index, int c_index);
    void GenerateUndirectedCompleteGraph();

    // whether two nodes are adjacent, or say, whether an edge (either directed and undirected) between two nodes exists
    bool IsAdjacentTo(const map<int, map<int, double>> &adjacencies, int node_idx1, int node_idx2);
    // whether an edge from node1->node2 exists, or say, whether node1 is a parent of node2
    bool IsDirectedFromTo(int node_idx1, int node_idx2);
    // whether an edge node1--node2 exists, or say,
    // whether node1 is adjacent to node2 && node1 is not a parent of node2 && node2 is not a parent of node1
    bool IsUndirected(const map<int, map<int, double>> &adjacencies, int node_idx1, int node_idx2);

    void SetParentChild(Node *par, Node *chi); // checked

    void RemoveParentChild(Node *par, Node *chi);

    set<int> GetParentIdxesOfNode(int node_index);
    set<int> GetChildrenIdxesOfNode(int node_index);

    vector<int> GetTopoOrd();

    int** ConvertDAGToAdjacencyMatrix();
    bool ContainCircle();

    /**
     * is used for Structural Hamming Distance (SHD)
     * we need to convert DAGs to CPDAGs before computing the SHD, including two steps:
     *      1. order the edges
     *      2. label each edge as either compelled or reversible
     */
    vector<Edge> OrderEdge();
    void FindCompelled(vector<Edge> &edge_order);
    bool IsDAG();

protected:
    vector<int> GenTopoOrd();
};


#endif //BAYESIANNETWORK_NETWORK_H

