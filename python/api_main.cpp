//
// Created by jjt on 17/03/24.
//
#include <pybind11/pybind11.h>
#include "fastpgm/fastpgm_api/fastpgm_api.h"

namespace py = pybind11;

PYBIND11_MODULE(cmake_example, m) {

    m.def("BNSL_PCStable", &BNSL_PCStable);
    m.def("BNL_PCStable", &BNL_PCStable);
    m.def("BNEI_JT", &BNEI_JT);
    m.def("C_PCStable_JT", &C_PCStable_JT);
}