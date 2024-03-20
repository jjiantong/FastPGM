//
// Created by 23036255 on 18/03/2024.
//

#ifndef FASTBN_API_JUNTIONTREESTRUCTURE_H
#define FASTBN_API_JUNTIONTREESTRUCTURE_H

#include <pybind11/pybind11.h>
#include "fastbn/inference/JuntionTreeStructure.h"

namespace py = pybind11;

void bind_JuntionTreeStructure(py::module &m) {
    py::class_<JuntionTreeStructure>(m, "JuntionTreeStructure")
            .def(py::init<Network*, int>())
            .def(py::init<>())
            .def_static("Moralize", &JuntionTreeStructure::Moralize)
            .def("Triangulate", &JuntionTreeStructure::Triangulate)
            .def("FormJunctionTree", &JuntionTreeStructure::FormJunctionTree)
            .def("AssignPotentials", &JuntionTreeStructure::AssignPotentials)
            .def("GetAveAndMaxCliqueSize", &JuntionTreeStructure::GetAveAndMaxCliqueSize);
}

#endif //FASTBN_API_JUNTIONTREESTRUCTURE_H
