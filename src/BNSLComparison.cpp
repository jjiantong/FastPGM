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

//    for (int i = 0; i < true_graph->num_edges; ++i) {
//        Edge edge = true_graph->edge_order.at(i);
//        if (edge.GetEndPoint1() == ARROW){
//            cout << edge.GetNode2()->GetNodeIndex() << " -> " << edge.GetNode1()->GetNodeIndex() << endl;
//        } else {
//            cout << edge.GetNode1()->GetNodeIndex() << " -> " << edge.GetNode2()->GetNodeIndex() << endl;
//        }
//    }
//    cout << "num edges = " << true_graph->edge_order.size() << endl;

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

    if (learned_graph->IsDAG()) {
        learned_graph->OrderEdge();
        learned_graph->FindCompelled();
    }

    int error = 0;

    // check every pair of nodes
    for (int i1 = 0; i1 < learned_graph->num_nodes; ++i1) {
        for (int i2 = i1 + 1; i2 < learned_graph->num_nodes; ++i2) {
            Node* node1_1 = true_graph->FindNodePtrByIndex(i1);
            Node* node1_2 = true_graph->FindNodePtrByIndex(i2);
            Node* node2_1 = learned_graph->FindNodePtrByIndex(i1);
            Node* node2_2 = learned_graph->FindNodePtrByIndex(i2);

            int pos1 = true_graph->GetEdge(node1_1, node1_2);
            int pos2 = learned_graph->GetEdge(node2_1, node2_2);

//            if (pos1 != -1) {
//                Edge e1 = true_graph->vec_edges.at(pos1);
//                if (!e1.IsDirected()) {
//                    cout << e1.GetNode1()->GetNodeIndex() << " -- " << e1.GetNode2()->GetNodeIndex() << endl;
//                } else {
//                    cout << e1.GetNode1()->GetNodeIndex() << " -> " << e1.GetNode2()->GetNodeIndex() << endl;
//                }
//            } else {
//                cout << "no edge between " << i1 << " and " << i2 << endl;
//            }
//            if (pos2 != -1) {
//                Edge e2 = learned_graph->vec_edges.at(pos2);
//                if (!e2.IsDirected()) {
//                    cout << e2.GetNode1()->GetNodeIndex() << " -- " << e2.GetNode2()->GetNodeIndex() << endl;
//                } else {
//                    cout << e2.GetNode1()->GetNodeIndex() << " -> " << e2.GetNode2()->GetNodeIndex() << endl;
//                }
//            } else {
//                cout << "no edge between " << i1 << " and " << i2 << endl;
//            }

//            int shd = GetSHDOneEdge(e1, e2);
//            error += shd;
        }
    }
    return error;
}

//int BNSLComparison::GetSHDOneEdge(Edge true_edge, Edge learned_edge) {
//    if (noEdge3(true_edge) && nondirected3(learned_edge)) {
//        return 1;
//    } else if (noEdge3(learned_edge) && nondirected3(true_edge)) {
//        return 1;
//    } else if (noEdge3(true_edge) && directed3(learned_edge)) {
//        return 2;
//    } else if (noEdge3(learned_edge) && directed3(true_edge)) {
//        return 2;
//    } else if (nondirected3(true_edge) && directed3(learned_edge)) {
//        return 1;
//    } else if (nondirected3(learned_edge) && directed3(true_edge)) {
//        return 1;
//    } else if (directed3(true_edge) && directed3(learned_edge)) {
//        if (Edges.getDirectedEdgeHead(true_edge) == Edges.getDirectedEdgeTail(learned_edge)) {
//            return 1;
//        }
//    }
//
//    return 0;
//}