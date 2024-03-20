//
// Created by 23036255 on 18/03/2024.
//

#ifndef FASTBN_API_PCSTABLE_H
#define FASTBN_API_PCSTABLE_H

#include <pybind11/pybind11.h>
#include "fastbn/structure_learning/PCStable.h"

namespace py = pybind11;

void bind_PCStable(py::module &m) {
    py::class_<PCStable, StructureLearning>(m, "PCStable")
            .def(py::init<Network*, double, int>())
            .def("StructLearnCompData", &PCStable::StructLearnCompData)
            .def("StructLearnByPCStable", &PCStable::StructLearnByPCStable)
            .def("SearchAtDepth", &PCStable::SearchAtDepth)
            .def("CheckEdge", &PCStable::CheckEdge)
            .def("FindAdjacencies", &PCStable::FindAdjacencies)
            .def("Testing", &PCStable::Testing)
            .def("FreeDegree", &PCStable::FreeDegree)
            .def("OrientVStructure", &PCStable::OrientVStructure)
            .def("OrientImplied", &PCStable::OrientImplied)
            .def("Rule1", &PCStable::Rule1)
            .def("Rule2", &PCStable::Rule2)
            .def("Rule3", &PCStable::Rule3)
            .def("R3Helper", &PCStable::R3Helper);
}

#endif //FASTBN_API_PCSTABLE_H
