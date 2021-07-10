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
    /**
     * used when we want to find an edge given two nodes
     * set to true only when we cannot find such an edge, otherwise it is always false
     */
    bool empty;

    Edge(){};
    Edge(Node* node1, Node* node2, EndPoint end_point1, EndPoint end_point2); // for directed edges
    Edge(Node* node1, Node* node2); // for undirected edges

    // define the "==" operator to compare two Edge object.
    bool operator == (const Edge edge) const {
        return (this->node1 == edge.node1) &&
               (this->node2 == edge.node2) &&
               (this->end_point1 == edge.end_point1) &&
               (this->end_point2 == edge.end_point2);
    }

    Node* GetNode1();
    Node* GetNode2();
    EndPoint GetEndPoint1();
    EndPoint GetEndPoint2();
    bool IsDirected();
};





#endif //BAYESIANNETWORK_EDGE_H
