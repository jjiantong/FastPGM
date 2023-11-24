//
// Created by jjt on 2021/7/10.
//

#include "fastbo/structure_learning/BNSLComparison.h"

BNSLComparison::BNSLComparison(Network *true_graph, Network *learned_graph) {
    this->true_graph = true_graph;
    this->learned_graph = learned_graph;
}

int BNSLComparison::GetSHD() {
    if (!learned_graph->IsDAG()) { // see notes in IsDAG()
        // if the learned graph is a CPDAG, we need to first change the true DAG to CPDAG
        vector<Edge> edge_order = true_graph->OrderEdge();
        true_graph->FindCompelled(edge_order);

        for (int i = 0; i < true_graph->num_edges; ++i) {
            Edge edge = true_graph->vec_edges.at(i);
            if (edge.label == REVERSIBLE) {
                // if reversible, convert the directed edge to undirected edge
                Node* node1 = edge.GetNode1();
                Node* node2 = edge.GetNode2();
                Edge new_edge = Edge(node1, node2);
                true_graph->vec_edges.at(i) = new_edge;
            }
        }

//    cout << "true cpdag: " << endl;
//    true_graph->PrintEachEdgeWithIndex();
    }

    int error = 0;
    // check every pair of nodes
    for (int i1 = 0; i1 < learned_graph->num_nodes; ++i1) {
        for (int i2 = i1 + 1; i2 < learned_graph->num_nodes; ++i2) {
            int shd = GetSHDOneEdge(i1, i2);
            error += shd;
        }
    }
    return error;
}

int BNSLComparison::GetSHDOneEdge(int index1, int index2) {
    Node* node1_1 = true_graph->FindNodePtrByIndex(index1);
    Node* node1_2 = true_graph->FindNodePtrByIndex(index2);
    Node* node2_1 = learned_graph->FindNodePtrByIndex(index1);
    Node* node2_2 = learned_graph->FindNodePtrByIndex(index2);

    int pos1 = true_graph->GetEdge(node1_1, node1_2);
    int pos2 = learned_graph->GetEdge(node2_1, node2_2);

    if (pos1 == -1 && pos2 == -1) {
        return 0;
    } else if (pos1 != -1 && pos2 != -1) {
        Edge e1  = true_graph->vec_edges.at(pos1);
        Edge e2  = learned_graph->vec_edges.at(pos2);

        // important note: in our implementation (of pc-stable), for undirected edges, the ordering of the two nodes
        // fixed; for directed edges, the former node (i.e., node1) is always the parent node while the latter one is
        // always the child node, and thus endpoint1 and endpoint2 are always TAIL and ARROW respectively.
        // if using other implementations that do not satisfy the conditions, change the if statement condition here.
        if (e1.GetNode1()->GetNodeIndex() == e2.GetNode1()->GetNodeIndex() &&
            e1.GetNode2()->GetNodeIndex() == e2.GetNode2()->GetNodeIndex() &&
            e1.GetEndPoint2() == e2.GetEndPoint2()) {
            return 0;
        } else {
            return 1;
        }
    } else {
        return 1;
    }
}
