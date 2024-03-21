//
// Created by jjt on 17/03/24.
//
#include <pybind11/pybind11.h>
#include "fastbn/fastbn_api/fastbn_api.h"

namespace py = pybind11;

PYBIND11_MODULE(cmake_example, m) {

    m.def("BNSL_PCStable", &BNSL_PCStable);
}