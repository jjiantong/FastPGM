//
// Created by jjt on 2021/7/10.
//

#include "BNSLComparison.h"

//int BNSLComparison::GetSHD(Network *true_dag, Network *learned_cpdag) {
//    int error = 0;
//
//    // check every pair of nodes
//    for (int i1 = 0; i1 < learned_cpdag->num_nodes; ++i1) {
//        for (int i2 = 0; i2 < learned_cpdag->num_nodes; ++i2) {
//        }
//    }
//
//    List<Node> _allNodes = estGraph.getNodes();
//
//    List<Node> allNodes = new ArrayList<>(_allNodes);
//
//    for (int i1 = 0; i1 < allNodes.size(); i1++) {
//        for (int i2 = i1 + 1; i2 < allNodes.size(); i2++) {
//            Node l1 = allNodes.get(i1);
//            Node l2 = allNodes.get(i2);
//
//            Edge e1 = trueGraph.getEdge(l1, l2);
//            Edge e2 = estGraph.getEdge(l1, l2);
//
//            int shd = structuralHammingDistanceOneEdge3(e1, e2);
//            error += shd;
//        }
//    }
//    return error;
//}