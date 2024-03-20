//
// Created by jjt on 19/03/24.
//

#ifndef BAYESIANNETWORK_API_CONTINUOUSNODE_H
#define BAYESIANNETWORK_API_CONTINUOUSNODE_H

#include <pybind11/pybind11.h>
#include "fastbn/ContinuousNode.h"

namespace py = pybind11;

void bind_ContinuousNode(py::module &m) {
    py::class_<ContinuousNode, Node>(m, "ContinuousNode")
            .def(py::init<int>())
            .def(py::init<>())
            .def(py::init<int, string>())
            .def("AddChild", &ContinuousNode::AddChild)
            .def("RemoveParent", &ContinuousNode::RemoveParent);
}

#endif //BAYESIANNETWORK_API_CONTINUOUSNODE_H
