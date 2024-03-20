//
// Created by 23036255 on 18/03/2024.
//

#ifndef FASTBN_API_STRUCTURELEARNING_H
#define FASTBN_API_STRUCTURELEARNING_H

#include <pybind11/pybind11.h>
#include "fastbn/structure_learning/StructureLearning.h"

namespace py = pybind11;

void bind_StructureLearning(py::module &m) {

    py::class_<StructureLearning>(m, "StructureLearning")
            .def(py::init<>()) // todo
            .def("StructLearnCompData", &StructureLearning::StructLearnCompData)
            .def("AssignNodeInformation", &StructureLearning::AssignNodeInformation)
            .def("AssignNodeOrder", &StructureLearning::AssignNodeOrder)
            .def("Direct", &StructureLearning::Direct)
            .def("GetCommonAdjacents", &StructureLearning::GetCommonAdjacents)
            .def("DirectLeftEdges", &StructureLearning::DirectLeftEdges)
            .def("FindRootsInDAGForest", &StructureLearning::FindRootsInDAGForest)
            .def("AddRootNode", &StructureLearning::AddRootNode)
            .def("SaveBNStructure", &StructureLearning::SaveBNStructure);
}

#endif //FASTBN_API_STRUCTURELEARNING_H
