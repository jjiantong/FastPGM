//
// Created by jjt on 2021/7/10.
//

#include "BNSLComparison.h"

BNSLComparison::BNSLComparison(Network *true_graph, Network *learned_graph) {
    this->true_graph = true_graph;
    this->learned_graph = learned_graph;
}

int BNSLComparison::GetSHD() {
    true_graph->OrderEdge();
    true_graph->FindCompelled();

//    cout << "size of edge order = " << true_graph->edge_order.size() << endl;
//    cout << "capacity of edge order = " << true_graph->edge_order.capacity() << endl;
//    for (int i = 0; i < true_graph->num_edges; ++i) {
//        Edge edge = true_graph->vec_edges.at(i);
//        if (edge.GetEndPoint1() == ARROW){
//            cout << edge.GetNode2()->GetNodeIndex() << " -> " << edge.GetNode1()->GetNodeIndex();
//        } else {
//            cout << edge.GetNode1()->GetNodeIndex() << " -> " << edge.GetNode2()->GetNodeIndex();
//        }
//        if (edge.label == COMPELLED) {
//            cout << ": COMPELLED" << endl;
//        } else if (edge.label == REVERSIBLE) {
//            cout << ": REVERSIBLE" << endl;
//        } else {
//            cout << ": ERROR!!" << endl;
//        }
//    }
//    cout << "num edges = " << true_graph->num_edges << endl;

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

    cout << "true cpdag: " << endl;
    for (int i = 0; i < true_graph->num_edges; ++i) {
        Edge edge = true_graph->vec_edges.at(i);
        if (!edge.IsDirected()) {
            cout << edge.GetNode1()->GetNodeIndex() << " -- " << edge.GetNode2()->GetNodeIndex() << endl;
        } else if (edge.GetEndPoint1() == ARROW){
            cout << edge.GetNode2()->GetNodeIndex() << " -> " << edge.GetNode1()->GetNodeIndex() << endl;
        } else {
            cout << edge.GetNode1()->GetNodeIndex() << " -> " << edge.GetNode2()->GetNodeIndex() << endl;
        }
    }
    cout << "num edges = " << true_graph->num_edges << endl;

    if (learned_graph->IsDAG()) {
        learned_graph->OrderEdge();
        learned_graph->FindCompelled();

        for (int i = 0; i < learned_graph->num_edges; ++i) {
            Edge edge = learned_graph->vec_edges.at(i);
            if (edge.label == REVERSIBLE) {
                // if reversible, convert the directed edge to undirected edge
                Node* node1 = edge.GetNode1();
                Node* node2 = edge.GetNode2();
                Edge new_edge = Edge(node1, node2);
                learned_graph->vec_edges.at(i) = new_edge;
            }
        }
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

//    if (pos1 != -1) {
//        Edge e1 = true_graph->vec_edges.at(pos1);
//        if (!e1.IsDirected()) {
//            cout << e1.GetNode1()->GetNodeIndex() << " -- " << e1.GetNode2()->GetNodeIndex() << endl;
//        } else if (e1.GetEndPoint1() == ARROW){
//            cout << e1.GetNode2()->GetNodeIndex() << " -> " << e1.GetNode1()->GetNodeIndex() << endl;
//        } else {
//            cout << e1.GetNode1()->GetNodeIndex() << " -> " << e1.GetNode2()->GetNodeIndex() << endl;
//        }
//    } else {
//        cout << "no edge between " << i1 << " and " << i2 << " in true graph" << endl;
//    }
//    if (pos2 != -1) {
//        Edge e2 = learned_graph->vec_edges.at(pos2);
//        if (!e2.IsDirected()) {
//            cout << e2.GetNode1()->GetNodeIndex() << " -- " << e2.GetNode2()->GetNodeIndex() << endl;
//        } else if (e2.GetEndPoint1() == ARROW){
//            cout << e2.GetNode2()->GetNodeIndex() << " -> " << e2.GetNode1()->GetNodeIndex() << endl;
//        } else {
//            cout << e2.GetNode1()->GetNodeIndex() << " -> " << e2.GetNode2()->GetNodeIndex() << endl;
//        }
//    } else {
//        cout << "no edge between " << i1 << " and " << i2 << " in learned graph" << endl;
//    }

    if (pos1 == -1 && pos2 == -1) {
//        cout << "both edges are not existed" << endl;
        return 0;
    } else if (pos1 != -1 && pos2 != -1) {
        Edge e1  = true_graph->vec_edges.at(pos1);
        Edge e2  = learned_graph->vec_edges.at(pos2);

//        if (!e1.IsDirected()) {
//            cout << e1.GetNode1()->GetNodeIndex() << " -- " << e1.GetNode2()->GetNodeIndex() << endl;
//        } else if (e1.GetEndPoint1() == ARROW){
//            cout << e1.GetNode2()->GetNodeIndex() << " -> " << e1.GetNode1()->GetNodeIndex() << endl;
//        } else {
//            cout << e1.GetNode1()->GetNodeIndex() << " -> " << e1.GetNode2()->GetNodeIndex() << endl;
//        }
//        if (!e2.IsDirected()) {
//            cout << e2.GetNode1()->GetNodeIndex() << " -- " << e2.GetNode2()->GetNodeIndex() << endl;
//        } else if (e2.GetEndPoint1() == ARROW){
//            cout << e2.GetNode2()->GetNodeIndex() << " -> " << e2.GetNode1()->GetNodeIndex() << endl;
//        } else {
//            cout << e2.GetNode1()->GetNodeIndex() << " -> " << e2.GetNode2()->GetNodeIndex() << endl;
//        }

        if (e1.GetNode1()->GetNodeIndex() == e2.GetNode1()->GetNodeIndex() &&
            e1.GetNode2()->GetNodeIndex() == e2.GetNode2()->GetNodeIndex() &&
            e1.GetEndPoint1() == e2.GetEndPoint1() && e1.GetEndPoint2() == e2.GetEndPoint2()) {
//            cout << "e1 = e2, return 0" << endl;
            return 0;
        } else {
//            cout << "e1 != e2, return 1" << endl;
            return 1;
        }
    } else {
//        cout << "one exists, the other dont, return 1" << endl;
        return 1;
    }
}