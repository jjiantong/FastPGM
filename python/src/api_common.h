//
// Created by 23036255 on 18/03/2024.
//

#ifndef FASTBN_API_COMMON_H
#define FASTBN_API_COMMON_H

#include <pybind11/pybind11.h>
#include "fastbn/common.h"

namespace py = pybind11;

void bind_common(py::module &m) {
    m.def("GenAllCombinationsFromSets<int>", &GenAllCombinationsFromSets<int>);
    m.def("GenAllCombinationsFromSets<double>", &GenAllCombinationsFromSets<double>);
    // todo: other possible types here

    m.def("DirectedGraphContainsCircle", &DirectedGraphContainsCircle);
    m.def("TopoSortOfDAGZeroInDegreeFirst", &TopoSortOfDAGZeroInDegreeFirst);
    m.def("TrimRight", &TrimRight);
    m.def("TrimLeft", &TrimLeft);
    m.def("Trim", &Trim);
    m.def("TrimRightComma", &TrimRightComma);
    m.def("TrimLeftParenthesis", &TrimLeftParenthesis);
    m.def("TrimRightCommaAndParenthesis", &TrimRightCommaAndParenthesis);
    m.def("TrimRightCommaAndSemicolon", &TrimRightCommaAndSemicolon);
    m.def("Split", &Split);
    m.def("NaryCount", &NaryCount);
    m.def("Random01", &Random01);

    py::class_<Value>(m, "Value")
            .def(py::init<>())
            .def("UseInt", &Value::UseInt)
            .def("SetInt", &Value::SetInt)
            .def("SetFloat", &Value::SetFloat)
            .def("GetInt", &Value::GetInt)
            .def("GetFloat", &Value::GetFloat);
}

#endif //FASTBN_API_COMMON_H
