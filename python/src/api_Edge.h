//
// Created by jjt on 19/03/24.
//

#ifndef BAYESIANNETWORK_API_EDGE_H
#define BAYESIANNETWORK_API_EDGE_H

#include <pybind11/pybind11.h>
#include "fastbn/Edge.h"

namespace py = pybind11;

void bind_Edge(py::module &m) {

    py::class_<Edge>(m, "Edge")
            .def(py::init<>())
            .def(py::init<Node*, Node*, EndPoint, EndPoint>())
            .def(py::init<Node*, Node*>())
            .def("GetNode1", &Edge::GetNode1)
            .def("GetNode2", &Edge::GetNode2)
            .def("GetEndPoint1", &Edge::GetEndPoint1)
            .def("GetEndPoint2", &Edge::GetEndPoint2)
            .def("IsDirected", &Edge::IsDirected);
}

#endif //BAYESIANNETWORK_API_EDGE_H
