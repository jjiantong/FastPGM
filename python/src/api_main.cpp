//
// Created by jjt on 17/03/24.
//
#include <pybind11/pybind11.h>
#include "api_test1.h"
#include "api_test2.h"

namespace py = pybind11;

PYBIND11_MODULE(cmake_example, m) {
    bind_Test1(m);
    bind_Test2(m);
}