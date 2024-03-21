//
// Created by jjt on 17/03/24.
//
#include <pybind11/pybind11.h>
#include "fastbn/fastbn_api/Test2.h"

namespace py = pybind11;

void bind_Test2(py::module &m) {

    py::class_<Test2>(m, "Test2")
            .def(py::init<>())
            .def("sub1", &Test2::sub1)
            .def("sub2", &Test2::sub2);
}