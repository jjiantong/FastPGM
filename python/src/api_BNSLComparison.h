//
// Created by 23036255 on 18/03/2024.
//

#ifndef FASTBN_API_BNSLCOMPARISON_H
#define FASTBN_API_BNSLCOMPARISON_H

#include <pybind11/pybind11.h>
#include "fastbn/structure_learning/BNSLComparison.h"

namespace py = pybind11;

void bind_BNSLComparison(py::module &m) {

    py::class_<BNSLComparison>(m, "BNSLComparison")
            .def(py::init<Network*, Network*>())
            .def("GetSHD", &BNSLComparison::GetSHD)
            .def("GetSHDOneEdge", &BNSLComparison::GetSHDOneEdge);
}

#endif //FASTBN_API_BNSLCOMPARISON_H
