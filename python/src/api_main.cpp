//
// Created by jjt on 17/03/24.
//
#include <pybind11/pybind11.h>
#include "api_test1.h"
#include "api_test2.h"
//#include "api_BNSLComparison.h"
//#include "api_CellTable.h"
//#include "api_ChoiceGenerator.h"
////#include "api_Clique.h"
////#include "api_common.h"
////#include "api_ContinuousNode.h"
////#include "api_CustomNetwork.h"
////#include "api_Dataset.h"
////#include "api_DiscreteNode.h"
//#include "api_Edge.h"
////#include "api_IndependenceTest.h"
////#include "api_Inference.h"
////#include "api_JunctionTree.h"
////#include "api_JuntionTreeStructure.h"
////#include "api_Network.h"
//#include "api_Node.h"
////#include "api_ParameterLearning.h"
////#include "api_PCStable.h"
////#include "api_PotentialTableBase.h"
////#include "api_SampleSetGenerator.h"
////#include "api_Separator.h"
////#include "api_StructureLearning.h"
//#include "api_Timer.h"
////#include "api_XMLBIFParser.h"

namespace py = pybind11;

PYBIND11_MODULE(cmake_example, m) {
    bind_Test1(m);
    bind_Test2(m);

//    bind_BNSLComparison(m);
//    bind_CellTable(m);
//    bind_ChoiceGenerator(m);
////    bind_Clique(m);
////    bind_common(m);
////    bind_ContinuousNode(m); // depends on Node
////    bind_CustomNetwork(m);
////    bind_Dataset(m);
////    bind_DiscreteNode(m);
//    bind_Edge(m);
////    bind_IndependenceTest(m);
////    bind_Inference(m);
////    bind_JuntionTree(m);
////    bind_JuntionTreeStructure(m);
////    bind_Network(m);
//    bind_Node(m);
////    bind_ParameterLearning(m);
////    bind_PCStable(m);
////    bind_PotentialTableBase(m);
////    bind_SampleSetGenerator(m);
////    bind_Separator(m);
////    bind_StructureLearning(m);
//    bind_Timer(m);
////    bind_XMLBIFParser(m);


}