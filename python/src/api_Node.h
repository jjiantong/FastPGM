//
// Created by jjt on 19/03/24.
//

#ifndef BAYESIANNETWORK_API_NODE_H
#define BAYESIANNETWORK_API_NODE_H

#include <pybind11/pybind11.h>
#include "fastbn/Node.h"

namespace py = pybind11;

void bind_Node(py::module &m) {

    py::class_<Node>(m, "Node")
            .def(py::init<>())
            .def(py::init<int>())
            .def(py::init<int, string>())
            .def("GetNodeIndex", &Node::GetNodeIndex)
            .def("SetNodeIndex", &Node::SetNodeIndex)
            .def("HasParents", &Node::HasParents)
            .def("AddChild", &Node::AddChild)
            .def("AddParent", &Node::AddParent)
            .def("IsChildOfThisNode", &Node::IsChildOfThisNode)
            .def("IsParentOfThisNode", &Node::IsParentOfThisNode)
            .def("RemoveChild", &Node::RemoveChild)
            .def("RemoveParent", &Node::RemoveParent);
}

#endif //BAYESIANNETWORK_API_NODE_H
