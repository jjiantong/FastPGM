//
// Created by 23036255 on 18/03/2024.
//

#ifndef FASTBN_API_CELLTABLE_H
#define FASTBN_API_CELLTABLE_H

#include <pybind11/pybind11.h>
#include "fastbn/structure_learning/CellTable.h"

namespace py = pybind11;

void bind_CellTable(py::module &m) {

    py::class_<Counts3D>(m, "Counts3D")
            .def(py::init<int, int, int, int, const vector<int> &, const vector<int> &>())
            .def("FillTable", &Counts3D::FillTable)
            .def("CountLevel1", &Counts3D::CountLevel1)
            .def("CountLevelN", &Counts3D::CountLevelN);

    py::class_<Counts2D>(m, "Counts2D")
            .def(py::init<int, int, int, int>())
            .def("FillTable", &Counts2D::FillTable);

    py::class_<Counts3DGroup>(m, "Counts3DGroup")
            .def(py::init<int, int, int, int, const vector<int> &, const vector<int> &, int>())
            .def("FillTableGroup", &Counts3DGroup::FillTableGroup)
            .def("CountLevel1", &Counts3DGroup::CountLevel1)
            .def("CountLevelN", &Counts3DGroup::CountLevelN);
}

#endif //FASTBN_API_CELLTABLE_H
