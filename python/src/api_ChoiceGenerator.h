//
// Created by 23036255 on 18/03/2024.
//

#ifndef FASTBN_API_CHOICEGENERATOR_H
#define FASTBN_API_CHOICEGENERATOR_H

#include <pybind11/pybind11.h>
#include "fastbn/ChoiceGenerator.h"

namespace py = pybind11;

void bind_ChoiceGenerator(py::module &m) {

    py::class_<ChoiceGenerator>(m, "ChoiceGenerator")
            .def(py::init<int, int>())
            .def("NextN", &ChoiceGenerator::NextN)
            .def("Next", &ChoiceGenerator::Next)
            .def("Fill", &ChoiceGenerator::Fill);
}

#endif //FASTBN_API_CHOICEGENERATOR_H
