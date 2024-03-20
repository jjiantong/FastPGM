//
// Created by 23036255 on 18/03/2024.
//

#ifndef FASTBN_API_INFERENCE_H
#define FASTBN_API_INFERENCE_H

#include <pybind11/pybind11.h>
#include "fastbn/inference/Inference.h"

namespace py = pybind11;

void bind_Inference(py::module &m) {
    m.def("Round", &Round);

    py::class_<Inference>(m, "Inference")
            .def(py::init<int, Network*, Dataset*>())
            .def(py::init<Network*>())
            .def("EvaluateAccuracy", &Inference::EvaluateAccuracy)
            .def("Predict", &Inference::Predict)
            .def("Accuracy", &Inference::Accuracy)
            .def("ArgMax", &Inference::ArgMax)
            .def("LoadGroundTruthProbabilityTable", &Inference::LoadGroundTruthProbabilityTable)
            .def("CalculateMSE", &Inference::CalculateMSE)
            .def("CalculateHellingerDistance", &Inference::CalculateHellingerDistance);
}

#endif //FASTBN_API_INFERENCE_H
