//
// Created by 23036255 on 18/03/2024.
//

#ifndef FASTBN_PARAMETERLEARNING_H
#define FASTBN_PARAMETERLEARNING_H

#include <pybind11/pybind11.h>
#include "fastbn/parameter_learning/ParameterLearning.h"

namespace py = pybind11;

void bind_ParameterLearning(py::module &m) {

    py::class_<ParameterLearning>(m, "ParameterLearning")
            .def(py::init<Network*>())
            .def("LearnParamsKnowStructCompData", &ParameterLearning::LearnParamsKnowStructCompData)
            .def("SaveBNParameter", &ParameterLearning::SaveBNParameter);
}

#endif //FASTBN_PARAMETERLEARNING_H
