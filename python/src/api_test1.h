//
// Created by jjt on 16/03/24.
//
#include <pybind11/pybind11.h>
#include "fastbn/fastbn_api/Test1.h"

namespace py = pybind11;

void bind_Test1(py::module &m) {
    py::class_<Test1>(m, "Test1")
            .def(py::init<int, int>())
            .def("add1", &Test1::add1)
            .def("add2", &Test1::add2);
}