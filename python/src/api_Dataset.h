//
// Created by jjt on 19/03/24.
//

#ifndef BAYESIANNETWORK_API_DATASET_H
#define BAYESIANNETWORK_API_DATASET_H

#include <pybind11/pybind11.h>
#include "fastbn/Dataset.h"

namespace py = pybind11;

void bind_Dataset(py::module &m) {

    py::class_<Dataset>(m, "Dataset")
            .def(py::init<>())
            .def(py::init<Dataset*>())
            .def(py::init<Network*>())
            .def("LoadLIBSVMTrainingData", &Dataset::LoadLIBSVMTrainingData)
            .def("LoadLIBSVMTestingData", &Dataset::LoadLIBSVMTestingData)
            .def("SamplesToLIBSVMFile1", &Dataset::SamplesToLIBSVMFile,
                 "SamplesToLIBSVMFile with DiscreteConfig")
            .def("SamplesToLIBSVMFile2",
                 py::overload_cast<vector<Configuration> &, string &>(&Dataset::SamplesToLIBSVMFile),
                 "SamplesToLIBSVMFile with Configuration")
            .def("LoadCSVTrainingData", &Dataset::LoadCSVTrainingData)
            .def("LoadCSVTestingData", &Dataset::LoadCSVTestingData)
            .def("SamplesToCSVFile1", &Dataset::SamplesToCSVFile,
                 "SamplesToCSVFile with DiscreteConfig")
            .def("SamplesToCSVFile2",
                 py::overload_cast<vector<Configuration> &, string &, vector<string>>(&Dataset::SamplesToLIBSVMFile),
                 "SamplesToCSVFile with Configuration")
            .def("StoreLIBSVMData", &Dataset::StoreLIBSVMData)
            .def("StoreCSVData", &Dataset::StoreCSVData)
            .def("Vector2IntArray", &Dataset::Vector2IntArray)
            .def("RowMajor2ColumnMajor", &Dataset::RowMajor2ColumnMajor);
}

#endif //BAYESIANNETWORK_API_DATASET_H
