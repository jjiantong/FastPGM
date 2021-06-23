//
// Created by jjt on 2021/6/23.
//

#ifndef BAYESIANNETWORK_EDGE_H
#define BAYESIANNETWORK_EDGE_H

#include "Node.h"

enum EndPoint {ARROW, TAIL};

class Edge {
public:
    Node* node1;
    Node* node2;
    EndPoint end_point1;
    EndPoint end_point2;

    Edge(Node* node1, Node* node2, EndPoint end_point1, EndPoint end_point2); // for directed edges
    Edge(Node* node1, Node* node2); // for undirected edges

    Node* GetNode1();
    Node* GetNode2();
    EndPoint GetEndPoint1();
    EndPoint GetEndPoint2();

    bool IsDirected();
};





#endif //BAYESIANNETWORK_EDGE_H
