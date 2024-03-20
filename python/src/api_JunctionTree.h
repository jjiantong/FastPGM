//
// Created by 23036255 on 18/03/2024.
//

#ifndef FASTBN_API_JUNCTIONTREE_H
#define FASTBN_API_JUNCTIONTREE_H

#include <pybind11/pybind11.h>
#include "fastbn/inference/JuntionTree.h"

namespace py = pybind11;

void bind_JuntionTree(py::module &m) {
    py::class_<JunctionTree, Inference>(m, "JunctionTree")
            .def(py::init<int, Network*, Dataset*>())
            .def(py::init<>())
            .def("Predict", &JunctionTree::Predict)
            .def("ResetJunctionTree", &JunctionTree::ResetJunctionTree)
            .def("GetMaxLevel", &JunctionTree::GetMaxLevel)
            .def("MarkLevel", &JunctionTree::MarkLevel)
            .def("ReorganizeTableStorage", &JunctionTree::ReorganizeTableStorage)
            .def("BackUpJunctionTree", &JunctionTree::BackUpJunctionTree)
            .def("LoadDiscreteEvidence", &JunctionTree::LoadDiscreteEvidence)
            .def("LoadEvidenceToNodes", &JunctionTree::LoadEvidenceToNodes)
            .def("LoadEvidenceToNodesOptimized", &JunctionTree::LoadEvidenceToNodesOptimized)
            .def("MessagePassingUpdateJT", &JunctionTree::MessagePassingUpdateJT)
            .def("Collect", &JunctionTree::Collect)
            .def("Distribute", &JunctionTree::Distribute)
            .def("SeparatorLevelCollection", &JunctionTree::SeparatorLevelCollection)
            .def("SeparatorLevelDistribution", &JunctionTree::SeparatorLevelDistribution)
            .def("CliqueLevelCollection", &JunctionTree::CliqueLevelCollection)
            .def("CliqueLevelDistribution", &JunctionTree::CliqueLevelDistribution)
            .def("SeparatorLevelCollectionOptimized", &JunctionTree::SeparatorLevelCollectionOptimized)
            .def("CliqueLevelDistributionOptimized", &JunctionTree::CliqueLevelDistributionOptimized)
            .def("CalculateMarginalProbability", &JunctionTree::CalculateMarginalProbability)
            .def("GetProbabilitiesAllNodes", &JunctionTree::GetProbabilitiesAllNodes)
            .def("InferenceUsingJT", &JunctionTree::InferenceUsingJT)
            .def("PredictUseJTInfer", &JunctionTree::PredictUseJTInfer)
            .def("Compute2DIndex", &JunctionTree::Compute2DIndex);
}

#endif //FASTBN_API_JUNCTIONTREE_H
