//
// Created by 23036255 on 18/03/2024.
//

#ifndef FASTBN_API_INDEPENDENCETEST_H
#define FASTBN_API_INDEPENDENCETEST_H

#include <pybind11/pybind11.h>
#include "fastbn/structure_learning/IndependenceTest.h"

namespace py = pybind11;

void bind_IndependenceTest(py::module &m) {

    py::class_<IndependenceTest>(m, "IndependenceTest")
            .def(py::init<Dataset*, double>())
            .def("IndependenceResult", &IndependenceTest::IndependenceResult)
            .def("ComputeGSquareXYZ", &IndependenceTest::ComputeGSquareXYZ)
            .def("ComputeGSquareXYZGroup", &IndependenceTest::ComputeGSquareXYZGroup)
            .def("ComputeGSquareXY", &IndependenceTest::ComputeGSquareXY);

    py::class_<IndependenceTest::Result>(m, "Result")
            .def(py::init<double, bool, int>());
}

#endif //FASTBN_API_INDEPENDENCETEST_H
