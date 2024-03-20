//
// Created by 23036255 on 18/03/2024.
//

#ifndef FASTBN_API_CLIQUE_H
#define FASTBN_API_CLIQUE_H

#include <pybind11/pybind11.h>
#include "fastbn/inference/Clique.h"

namespace py = pybind11;

void bind_Clique(py::module &m) {
    py::class_<Clique>(m, "Clique")
            .def(py::init<>())
            .def(py::init<set<int>, Network*>())
            .def("Collect2", &Clique::Collect2)
            .def("Collect3", &Clique::Collect3)
            .def("Distribute2", &Clique::Distribute2)
            .def("Distribute3", &Clique::Distribute3)
            .def("UpdateMessage", &Clique::UpdateMessage);
}

#endif //FASTBN_API_CLIQUE_H
