//
// Created by jjt on 19/03/24.
//

#ifndef BAYESIANNETWORK_API_NETWORK_H
#define BAYESIANNETWORK_API_NETWORK_H

#include <pybind11/pybind11.h>
#include "fastbn/Network.h"

namespace py = pybind11;

void bind_Network(py::module &m) {

    py::class_<Network>(m, "Network")
            .def(py::init<>())
            .def(py::init<bool>())
            .def(py::init<Network &>())
            .def(py::init<vector<Node*>, string>())
            .def("FindNodePtrByIndex", &Network::FindNodePtrByIndex)
            .def("FindNodePtrByName", &Network::FindNodePtrByName)
            .def("GetUndirectedEdge", &Network::GetUndirectedEdge)
            .def("GetDirectedEdge", &Network::GetDirectedEdge)
            .def("GetEdge", &Network::GetEdge)
            .def("GetDirectedEdgeFromEdgeOrder", &Network::GetDirectedEdgeFromEdgeOrder)
            .def("PrintEdges", &Network::PrintEdges)
            .def("CheckEdges", &Network::CheckEdges)
            .def("AddDirectedEdge", &Network::AddDirectedEdge)
            .def("DeleteDirectedEdge", &Network::DeleteDirectedEdge)
            .def("AddUndirectedEdge", &Network::AddUndirectedEdge)
            .def("DeleteUndirectedEdge", &Network::DeleteUndirectedEdge)
            .def("GenerateUndirectedCompleteGraph", &Network::GenerateUndirectedCompleteGraph)
            .def("IsAdjacentTo", &Network::IsAdjacentTo)
            .def("IsDirectedFromTo", &Network::IsDirectedFromTo)
            .def("IsUndirected", &Network::IsUndirected)
            .def("SetParentChild", &Network::SetParentChild)
            .def("RemoveParentChild", &Network::RemoveParentChild)
            .def("GetParentIdxesOfNode", &Network::GetParentIdxesOfNode)
            .def("GetChildrenIdxesOfNode", &Network::GetChildrenIdxesOfNode)
            .def("GetTopoOrd", &Network::GetTopoOrd)
            .def("ConvertDAGToAdjacencyMatrix", &Network::ConvertDAGToAdjacencyMatrix)
            .def("ContainCircle", &Network::ContainCircle)
            .def("OrderEdge", &Network::OrderEdge)
            .def("FindCompelled", &Network::FindCompelled)
            .def("IsDAG", &Network::IsDAG);
}

#endif //BAYESIANNETWORK_API_NETWORK_H
