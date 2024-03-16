#include <pybind11/pybind11.h>
#include "fastbn/Parameter.h"
#include "fastbn/Dataset.h"

namespace py = pybind11;

PYBIND11_MODULE(_core, m) {

    m.def("add", &add);

    py::class_<Dataset>(m, "Dataset")
            .def(py::init<>());
//            .def("load_csv_training_data", &Dataset::LoadCSVTrainingData);

//    py::class_<MyOtherClass>(m, "MyOtherClass")
//            .def(py::init<std::string>())
//            .def("print_message", &MyOtherClass::printMessage)
//            .def("get_message", &MyOtherClass::getMessage);
}
