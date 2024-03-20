//
// Created by 23036255 on 18/03/2024.
//

#ifndef FASTBN_API_SEPARATOR_H
#define FASTBN_API_SEPARATOR_H

#include <pybind11/pybind11.h>
#include "fastbn/inference/Separator.h"

namespace py = pybind11;

void bind_Separator(py::module &m) {
    py::class_<Separator, Clique>(m, "Separator")
            .def(py::init<set<int>, Network*>())
            .def(py::init<>())
            .def("UpdateMessage", &Separator::UpdateMessage);
}

#endif //FASTBN_API_SEPARATOR_H
