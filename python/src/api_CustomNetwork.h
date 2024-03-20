//
// Created by jjt on 19/03/24.
//

#ifndef BAYESIANNETWORK_API_CUSTOMNETWORK_H
#define BAYESIANNETWORK_API_CUSTOMNETWORK_H

#include <pybind11/pybind11.h>
#include "fastbn/CustomNetwork.h"

namespace py = pybind11;

void bind_CustomNetwork(py::module &m) {
    py::class_<CustomNetwork, Network>(m, "CustomNetwork")
            .def(py::init<bool>())
            .def(py::init<>())
            .def("LoadXMLBIFFile", &CustomNetwork::LoadXMLBIFFile)
            .def("LoadBIFFile", &CustomNetwork::LoadBIFFile);
}

#endif //BAYESIANNETWORK_API_CUSTOMNETWORK_H
