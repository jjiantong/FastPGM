//
// Created by jjt on 19/03/24.
//

#ifndef BAYESIANNETWORK_API_DISCRETENODE_H
#define BAYESIANNETWORK_API_DISCRETENODE_H

#include <pybind11/pybind11.h>
#include "fastbn/DiscreteNode.h"

namespace py = pybind11;

void bind_DiscreteNode(py::module &m) {
    py::class_<DiscreteNode, Node>(m, "DiscreteNode")
            .def(py::init<int>())
            .def(py::init<>())
            .def("SetLaplaceSmooth", &DiscreteNode::SetLaplaceSmooth)
            .def("GetValueNameByIndex", &DiscreteNode::GetValueNameByIndex)
            .def("AddCount", &DiscreteNode::AddCount)
            .def("GetProbability", &DiscreteNode::GetProbability)
            .def("InitializeCPT", &DiscreteNode::InitializeCPT)
            .def("GetDomainSize", &DiscreteNode::GetDomainSize)
            .def("SetDomainSize", &DiscreteNode::SetDomainSize)
            .def("AddParent", &DiscreteNode::AddParent);
}

#endif //BAYESIANNETWORK_API_DISCRETENODE_H
