//
// Created by jjt on 2021/7/10.
//

#include "BNSLComparison.h"

int BNSLComparison::GetSHD(Network *true_dag, Network *learned_cpdag) {
    int error = 0;

    // check every pair of nodes
    for (int i1 = 0; i1 < learned_cpdag->num_nodes; ++i1) {
        for (int i2 = i1 + 1; i2 < learned_cpdag->num_nodes; ++i2) {
            Node* node1_1 = true_dag->FindNodePtrByIndex(i1);
            Node* node1_2 = true_dag->FindNodePtrByIndex(i2);
            Node* node2_1 = learned_cpdag->FindNodePtrByIndex(i1);
            Node* node2_2 = learned_cpdag->FindNodePtrByIndex(i2);

            Edge e1 = true_dag->GetEdge(node1_1, node1_2);
            Edge e2 = learned_cpdag->GetEdge(node2_1, node2_2);

//            if (!e1.empty) {
//                if (!e1.IsDirected()) {
//                    cout << e1.GetNode1()->GetNodeIndex() << " -- " << e1.GetNode2()->GetNodeIndex() << endl;
//                } else {
//                    cout << e1.GetNode1()->GetNodeIndex() << " -> " << e1.GetNode2()->GetNodeIndex() << endl;
//                }
//            } else {
//                cout << "no edge between " << i1 << " and " << i2 << endl;
//            }
//            if (!e2.empty) {
//                if (!e2.IsDirected()) {
//                    cout << e2.GetNode1()->GetNodeIndex() << " -- " << e2.GetNode2()->GetNodeIndex() << endl;
//                } else {
//                    cout << e2.GetNode1()->GetNodeIndex() << " -> " << e2.GetNode2()->GetNodeIndex() << endl;
//                }
//            } else {
//                cout << "no edge between " << i1 << " and " << i2 << endl;
//            }

            int shd = GetSHDOneEdge(e1, e2);
            error += shd;
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