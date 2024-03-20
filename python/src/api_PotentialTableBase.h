//
// Created by jjt on 19/03/24.
//

#ifndef BAYESIANNETWORK_API_POTENTIALTABLEBASE_H
#define BAYESIANNETWORK_API_POTENTIALTABLEBASE_H

#include <pybind11/pybind11.h>
#include "fastbn/PotentialTableBase.h"

namespace py = pybind11;

void bind_PotentialTableBase(py::module &m) {

    py::class_<PotentialTableBase>(m, "PotentialTableBase")
            .def(py::init<>())
            .def(py::init<DiscreteNode *, Network *>())
            .def(py::init<DiscreteNode *, int>())
            .def("ConstructPotentialTableWithEmptyPotentials", &PotentialTableBase::ConstructPotentialTableWithEmptyPotentials)
            .def("ConstructEmptyPotentialTable", &PotentialTableBase::ConstructEmptyPotentialTable)
            .def("TableReorganization", &PotentialTableBase::TableReorganization)
            .def("TableReorganizationPre", &PotentialTableBase::TableReorganizationPre)
            .def("TableReorganizationMain", &PotentialTableBase::TableReorganizationMain)
            .def("TableReorganizationPost", &PotentialTableBase::TableReorganizationPost)
            .def("TableReduction", &PotentialTableBase::TableReduction)
            .def("TableReductionPre", &PotentialTableBase::TableReductionPre)
            .def("TableReductionMain", &PotentialTableBase::TableReductionMain)
            .def("TableReductionPost", &PotentialTableBase::TableReductionPost)
            .def("GetReducedPotentials", &PotentialTableBase::GetReducedPotentials)
            .def("GetReducedICPTPotentials", &PotentialTableBase::GetReducedICPTPotentials)
            .def("GetReducedPotential", &PotentialTableBase::GetReducedPotential)
            .def("GetReducedIndexAndPotential", &PotentialTableBase::GetReducedIndexAndPotential)
            .def("TableMarginalization", &PotentialTableBase::TableMarginalization)
            .def("TableMarginalizationPre", &PotentialTableBase::TableMarginalizationPre)
            .def("TableMarginalizationMain", &PotentialTableBase::TableMarginalizationMain)
            .def("TableMarginalizationPost", &PotentialTableBase::TableMarginalizationPost)
            .def("TableMarginalization", &PotentialTableBase::TableMarginalization)
            .def("TableMarginalizationPre", &PotentialTableBase::TableMarginalizationPre)
            .def("TableMarginalizationSimplified", &PotentialTableBase::TableMarginalizationSimplified)
            .def("TableMarginalizationOneVariablePost", &PotentialTableBase::TableMarginalizationOneVariablePost)
            .def("TableMarginalizationOneVariable", &PotentialTableBase::TableMarginalizationOneVariable)
            .def("TableMarginalizationOneVariablePre", &PotentialTableBase::TableMarginalizationOneVariablePre)
            .def("GetMarginalizedProbabilities", &PotentialTableBase::GetMarginalizedProbabilities)
            .def("TableExtension", &PotentialTableBase::TableExtension)
            .def("TableExtensionPre", &PotentialTableBase::TableExtensionPre)
            .def("TableExtensionMain", &PotentialTableBase::TableExtensionMain)
            .def("TableExtensionPost", &PotentialTableBase::TableExtensionPost)
            .def("TableMultiplication", &PotentialTableBase::TableMultiplication)
            .def("TableMultiplicationTwoExtension", &PotentialTableBase::TableMultiplicationTwoExtension)
            .def("TableMultiplicationOneVariable", &PotentialTableBase::TableMultiplicationOneVariable)
            .def("TableDivision", &PotentialTableBase::TableDivision)
            .def("Normalize", &PotentialTableBase::Normalize)
            .def("NormalizeCPT", &PotentialTableBase::NormalizeCPT)
            .def("NormalizeICPT", &PotentialTableBase::NormalizeICPT)
            .def("NormalizePtScore", &PotentialTableBase::NormalizePtScore)
            .def("UniformDistribution", &PotentialTableBase::UniformDistribution)
            .def("GetVariableIndex", &PotentialTableBase::GetVariableIndex)
            .def("GetConfigValueByTableIndex1", &PotentialTableBase::GetConfigValueByTableIndex,
                 "GetConfigValueByTableIndex with 4 arguments")
            .def("GetTableIndexByConfigValue1", &PotentialTableBase::GetTableIndexByConfigValue,
                 "GetTableIndexByConfigValue with 3 arguments")
            .def("GetConfigValueByTableIndex2",
                 py::overload_cast<const int &, int *>(&PotentialTableBase::GetConfigValueByTableIndex),
                 "GetConfigValueByTableIndex with 2 arguments")
            .def("GetTableIndexByConfigValue",
                 py::overload_cast<int *>(&PotentialTableBase::GetTableIndexByConfigValue),
                 "GetTableIndexByConfigValue with 1 argument")
            .def("GetRelativeIndexByConfigValue", &PotentialTableBase::GetRelativeIndexByConfigValue)
            .def("ConstructVarDimsAndCumLevels", &PotentialTableBase::ConstructVarDimsAndCumLevels)
            .def("ConstructCumLevels", &PotentialTableBase::ConstructCumLevels);
}

#endif //BAYESIANNETWORK_API_POTENTIALTABLEBASE_H
