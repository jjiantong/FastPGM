//
// Created by jjt on 20/03/24.
//

#ifndef BAYESIANNETWORK_API_XMLBIFPARSER_H
#define BAYESIANNETWORK_API_XMLBIFPARSER_H

#include <pybind11/pybind11.h>
#include "fastbn/XMLBIFParser.h"

namespace py = pybind11;

void bind_XMLBIFParser(py::module &m) {

    py::class_<XMLBIFParser>(m, "XMLBIFParser")
            .def(py::init<>())
            .def(py::init<string &>())
            .def("LoadFile", &XMLBIFParser::LoadFile)
            .def("GetUnconnectedNodes", &XMLBIFParser::GetUnconnectedNodes)
            .def("AssignProbsToNodes", &XMLBIFParser::AssignProbsToNodes)
            .def("GetConnectedNodes", &XMLBIFParser::GetConnectedNodes);
}

#endif //BAYESIANNETWORK_API_XMLBIFPARSER_H
