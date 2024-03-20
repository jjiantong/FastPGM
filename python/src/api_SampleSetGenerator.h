//
// Created by jjt on 20/03/24.
//

#ifndef BAYESIANNETWORK_API_SAMPLESETGENERATOR_H
#define BAYESIANNETWORK_API_SAMPLESETGENERATOR_H

#include <pybind11/pybind11.h>
#include "fastbn/SampleSetGenerator.h"

namespace py = pybind11;

void bind_SampleSetGenerator(py::module &m) {

    py::class_<SampleSetGenerator>(m, "SampleSetGenerator")
            .def(py::init<Network*, int>())
            .def("GenerateSamplesBasedOnCPTs", &SampleSetGenerator::GenerateSamplesBasedOnCPTs);
}

#endif //BAYESIANNETWORK_API_SAMPLESETGENERATOR_H
