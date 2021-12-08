//
// Created by jjt on 2021/6/23.
//

#include "Edge.h"

Edge::Edge(Node* node1, Node* node2, EndPoint end_point1, EndPoint end_point2) {
    this->node1 = node1;
    this->node2 = node2;
    this->end_point1 = end_point1;
    this->end_point2 = end_point2;
    is_ordered = false;
    need_remove = false;
    process = NO;
}

Edge::Edge(Node* node1, Node* node2) {
    int index1 = node1->GetNodeIndex();
    int index2 = node2->GetNodeIndex();

    // check the order TODO
    if (index1 < index2) {
        this->node1 = node1;
        this->node2 = node2;
    } else {
        this->node1 = node2;
        this->node2 = node1;
    }
    this->end_point1 = TAIL;
    this->end_point2 = TAIL;
    is_ordered = false;
    need_remove = false;
    process = NO;
}

Node* Edge::GetNode1() {
    return node1;
}

Node* Edge::GetNode2() {
    return node2;
}

EndPoint Edge::GetEndPoint1() {
    return end_point1;
}

EndPoint Edge::GetEndPoint2() {
    return end_point2;
}

bool Edge::IsDirected() {
    if ((end_point1 == ARROW && end_point2 == TAIL) ||
        (end_point2 == ARROW && end_point1 == TAIL)) {
        return true;
    } else {
        return false;
    }
}