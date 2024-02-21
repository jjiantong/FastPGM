//
// Created by jjt on 2021/6/17.
//

#include "fastbo/structure_learning/StructureLearning.h"

/**
 * @brief: add each node in the network, contains:
 * index, name, domain size, possible values
 */
void StructureLearning::AssignNodeInformation(Dataset *dts) {
    network->num_nodes = dts->num_vars;

    // Assign an index for each node.
    for (int i = 0; i < network->num_nodes; ++i) {
        // construct a node in the network
        DiscreteNode *node_ptr = new DiscreteNode(i);

        // give this node a name, mainly for print
        if (dts->vec_var_names.size() == network->num_nodes) {
            node_ptr->node_name = dts->vec_var_names[i];
        } else {
            node_ptr->node_name = to_string(i);//use id as name
        }

        // copy `vars_possible_values`_ids from dts
        node_ptr->possible_values_ids = dts->vars_possible_values_ids[i];

        //set the potential values for this node
        int domain_size = dts->num_of_possible_values_of_disc_vars[i];
        node_ptr->SetDomainSize(domain_size);
        network->map_idx_node_ptr[i] = node_ptr;
    }
}

/**
 * @brief: assign an order of the nodes; the order will be used in structure learning
 */
vector<int> StructureLearning::AssignNodeOrder(string topo_ord_constraint) {

    vector<int> ord;
    ord.reserve(network->num_nodes);
    for (int i = 0; i < network->num_nodes; ++i) {
        ord.push_back(i);   // the nodes are created the same order as in the dataset.
    }

    cout << "topo_ord_constraint: " << topo_ord_constraint << endl;
    //choose an order to serve as a constraint during learning
    if (topo_ord_constraint == "dataset-ord") {
        // Do nothing.
    }
    else if (topo_ord_constraint == "random") {
        std::srand(unsigned(std::time(0)));
        std::random_shuffle(ord.begin(), ord.end());
    }
    else if (topo_ord_constraint == "best") {
        ord = vector<int> {};//no order is provided (i.e. no constraint)
    }
    else {
        fprintf(stderr, "Error in function [%s]!\nInvalid topological ordering restriction!", __FUNCTION__);
        exit(1);
    }

    return ord;
}

/**
 * @brief: direct the edge (direction: from a to c)
 *      1, delete undirected edge a--c
 *      2, add directed edge a->c
 * It is ensured that a--c exists.
 */
bool StructureLearning::Direct(int a, int c) {
    // Instead of deleting undirected edge -> adding directed edge and checking loop
    // -> if loop exists, deleting undirected edge, we can just adding directed edge
    // and checking loop -> if loop doesn't exist, deleting undirected edge. This is
    // because checking circle doesn't need `vec_edges` and deleting undirected edge
    // only affects `vec_edges`.
    bool added = network->AddDirectedEdge(a, c);
    if (added) {
        network->DeleteUndirectedEdge(a, c);
    }
    return added;
}

/**
 * @brief: find common neighbors of x and y
 * @param x_idx index of node x
 * @param y_idx index of node y
 * @return common neighbor set
 */
vector<int> StructureLearning::GetCommonAdjacents(const map<int, map<int, double>> &adjacencies, int x_idx, int y_idx) {
    set<int> adjacent_x, adjacent_y, set_common_idx;
    for (auto it = adjacencies.at(x_idx).begin(); it != adjacencies.at(x_idx).end(); ++it) {
        adjacent_x.insert((*it).first);
    }
    for (auto it = adjacencies.at(y_idx).begin(); it != adjacencies.at(y_idx).end(); ++it) {
        adjacent_y.insert((*it).first);
    }
    set_intersection(adjacent_x.begin(), adjacent_x.end(),
                     adjacent_y.begin(), adjacent_y.end(),
                     inserter(set_common_idx, set_common_idx.begin()));
    vector<int> common_idx(set_common_idx.begin(), set_common_idx.end());
    return common_idx;
}

/**
 * convert cpdag to dag.
 */
void StructureLearning::DirectLeftEdges(const map<int, map<int, double>> &adjacencies) {

    for (auto edge_it = network->vec_edges.begin(); edge_it != network->vec_edges.end();) {
        int x_idx = (*edge_it).GetNode1()->GetNodeIndex();
        int y_idx = (*edge_it).GetNode2()->GetNodeIndex();

        // if the edge is undirected, direct it
        if (network->IsUndirected(adjacencies, x_idx, y_idx)) {
            bool direct; // TODO: just for test here, need to change to Random01()
            if (Random01() == 0) {
                direct = Direct(x_idx, y_idx);
                if (!direct) { // if the undirected edge x--y remains
                    cout << "Randomly directing causes unexpected loop, trying to direct again... " << endl;
                    direct = Direct(y_idx, x_idx);
                }
            } else {
                direct = Direct(y_idx, x_idx);
                if (!direct) { // if the undirected edge x--y remains
                    cout << "Randomly directing causes unexpected loop, trying to direct again... " << endl;
                    direct = Direct(x_idx, y_idx);
                }
            }
            if (!direct) { // if the undirected edge x--y remains
                cerr << "Error: randomly directing causes loop anyway. " << endl;
                exit(1);
            }
        } else { // if the edge is not undirected
            // Instead of checking the next edge, here we just jump out this loop, because in our implementation,
            // the directed edges are always behind the undirected ones in `vec_edges`. Thus, if we meet a directed
            // edge, we don't need to check the following edges.
            break;
//                edge_it++;
        }
    } // finish checking all edges
}

/**
 * find a root for each connected sub-graph of the resulting DAG.
 * sometimes pc-stable results in a DAG that contains multiple connected sub-graphs that are independent with each
 * other. this method is to identify each of the sub-graphs and return the roots for them.
 */
vector<int> StructureLearning::FindRootsInDAGForest() {
    vector<int> roots;
    vector<bool> visited(network->num_nodes, false);
    queue<int> que;

    // compute the in-degrees for each node
    int *in_degrees = new int[network->num_nodes];
    for (auto &i_n_p : network->map_idx_node_ptr) { // for each node
        auto node_idx = i_n_p.first;
        in_degrees[node_idx] = network->GetParentIdxesOfNode(node_idx).size();
    }

    while (1) {
        // find the node that has the most children from all the unvisited nodes with in-degree = 0
        int max_children = -1;
        int root = -1;
        for (int i = 0; i < network->num_nodes; ++i) {
            if (in_degrees[i] == 0 && visited[i] == false) {
                int num_children = network->GetChildrenIdxesOfNode(i).size();
                if (num_children > max_children) {
                    max_children = network->GetChildrenIdxesOfNode(i).size();
                    root = i;
                }
            }
        }
        // while loop exit condition: no unvisited node with in-degree = 0
        if (max_children == -1) {
            break;
        }
        visited[root] = true;
        que.push(root);
        roots.push_back(root);

        // do a bfs based on the root above, resulting in one of the sub-graph
        while (!que.empty()) {
            int current = que.front();
            que.pop();

            for (const int &child: network->GetChildrenIdxesOfNode(current)) {
                if (!visited[child]) {
                    visited[child] = true;
                    que.push(child);
                }
            }
            for (const int &parent: network->GetParentIdxesOfNode(current)) {
                if (!visited[parent]) {
                    visited[parent] = true;
                    que.push(parent);
                }
            }
        }
    }

    SAFE_DELETE_ARRAY(in_degrees);
    return roots;
}

/**
 * @brief: add a root node.
 * this method is used if the resulting network structure contains more than 1 connected sub-graphs. we add a `ROOT`
 * node to be the parent of each sub-graph's root identified by `FindRootsInDAGForest`.
 *      id: network->num_nodes
 *      name: ROOT
 *      type: discrete
 *      possible values: 0, 1
 */
void StructureLearning::AddRootNode(vector<int> &sub_roots) {
    // I think the added node should be discrete node
    DiscreteNode *root = new DiscreteNode(network->num_nodes);
    root->node_name = "ROOT";
    root->possible_values_ids["0"] = 0;
    root->possible_values_ids["1"] = 1;
    root->SetDomainSize(2);

    // set parent - child relationships. set `ROOT` as the parent of the variables in `sub_roots`
    for (const int &child_index: sub_roots) {
        Node* child_ptr = network->FindNodePtrByIndex(child_index);
        network->SetParentChild(root, child_ptr);
        Edge edge(root, child_ptr, TAIL, ARROW);
        network->vec_edges.push_back(edge);
        network->num_edges++;
    }

    network->map_idx_node_ptr[network->num_nodes] = root;
    network->num_nodes++;
}

void StructureLearning::SaveBNStructure(string struct_file) {
    ofstream out_file(struct_file);
    if (!out_file.is_open()) {
        fprintf(stderr, "Error in function %s!", __FUNCTION__);
        fprintf(stderr, "Unable to open file %s!", struct_file.c_str());
        exit(1);
    }

    string head = "number of nodes = " + to_string(network->num_nodes)
            + ", number of edges = " + to_string(network->num_edges);
    out_file << head << endl;

    if (network->vec_edges[0].IsDirected()) {
        // DAG
        vector<int> topo = network->GetTopoOrd();

        int edge_count = 0;
        for (int i = 0; i < network->num_nodes; ++i) {
            int this_idx = topo[i];
            Node *this_ptr = network->FindNodePtrByIndex(this_idx);
            string this_name = this_ptr->node_name;

            for (const int &child_idx: this_ptr->set_children_indexes) {
                Node *child_ptr = network->FindNodePtrByIndex(child_idx);
                string child_name = child_ptr->node_name;
                string s_edge = to_string(edge_count) + ": "
                        + this_name + "/v" + to_string(this_idx) + " -> "
                        + child_name + "/v" + to_string(child_idx);
                out_file << s_edge << endl;
                edge_count++;
            }
        }
    } else {
        // CPDAG
        for (int i = 0; i < network->num_edges; ++i) {
            string s_edge = to_string(i) + ": ";
            Edge edge = network->vec_edges.at(i);
            Node *node1 = edge.GetNode1();
            Node *node2 = edge.GetNode2();
            if (!edge.IsDirected()) {
                s_edge += node1->node_name + "/v" + to_string(node1->GetNodeIndex()) + " -- "
                        + node2->node_name + "/v" + to_string(node2->GetNodeIndex());
            } else if (edge.GetEndPoint1() == TAIL){
                s_edge += node1->node_name + "/v" + to_string(node1->GetNodeIndex()) + " -> "
                        + node2->node_name + "/v" + to_string(node2->GetNodeIndex());
            } else {
                s_edge += node2->node_name + "/v" + to_string(node2->GetNodeIndex()) + " -> "
                        + node1->node_name + "/v" + to_string(node1->GetNodeIndex());
            }
            out_file << s_edge << endl;
        }
    }

    cout << "Learned structure has been saved in. " << struct_file << endl;
    out_file.close();
}