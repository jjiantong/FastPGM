//
// Created by jjt on 20/03/24.
//

#ifndef BAYESIANNETWORK_API_TIMER_H
#define BAYESIANNETWORK_API_TIMER_H

#include <pybind11/pybind11.h>
#include "fastbn/Timer.h"

namespace py = pybind11;

void bind_Timer(py::module &m) {

    py::class_<Timer>(m, "Timer")
            .def(py::init<>())
            .def("Start", &Timer::Start)
            .def("Stop", &Timer::Stop)
            .def("Print", &Timer::Print);
}
#endif //BAYESIANNETWORK_API_TIMER_H
