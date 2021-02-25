#include <string>
#include <vector>
#include "gtest/gtest.h"
#include "tinyxml2.h"

#include "CustomNetwork.h"

using namespace std;
using namespace tinyxml2;

//TEST(XMLBIFParser, usability) {//xml is used to describe the network; this test is to verify the correctness of reading file content.
//  string file = "../../data/interchange-format-file/dog-problem.xml";
//  XMLDocument doc;
//  doc.LoadFile(file.c_str());
//  const XMLElement *net = doc.FirstChildElement("BIF")->FirstChildElement("NETWORK");
//
//  string elem_type = net->Value();
//  EXPECT_EQ(elem_type,"NETWORK");
//
//  string net_name = net->FirstChildElement("NAME")->GetText();
//  EXPECT_EQ(net_name,"Dog-Problem");
//
//  const XMLElement *var1 = net->FirstChildElement("VARIABLE");
//  string var1_name = var1->FirstChildElement("NAME")->GetText();
//  EXPECT_EQ(var1_name,"light-on");
//
//  EXPECT_EQ((string)net->FirstChildElement()
//                       ->NextSiblingElement()
//                       ->FirstChildElement()->GetText(),
//             "light-on");
//
//}
//
//TEST(XMLBIFParser, dog_problem_xmlbif) {//dog-problem bayesian network can be found online.
//  string file = "../../data/interchange-format-file/dog-problem.xml";
//  CustomNetwork dog_net;//self defined
//  dog_net.GetNetFromXMLBIFFile(file);//construct a network with values based on the xml file
//  auto *node_family_out = dog_net.FindNodePtrByName("family-out");//
//  auto *node_bowel_problem = dog_net.FindNodePtrByName("bowel-problem");
//  auto *node_dog_out = dog_net.FindNodePtrByName("dog-out");
//
//  EXPECT_EQ(node_family_out->set_parent_indexes.size(),0);//node_family_out has no parent.
//  EXPECT_EQ(node_bowel_problem->set_parent_indexes.size(),0);
//  EXPECT_EQ(node_dog_out->set_parent_indexes.size(),2);//node_dog_out has two parents
//  EXPECT_NE(node_dog_out->set_parent_indexes.find(node_family_out->GetNodeIndex())
//            ,
//            node_dog_out->set_parent_indexes.end());//family_out must be one of the parents of dog_out.
//  EXPECT_NE(node_dog_out->set_parent_indexes.find(node_bowel_problem->GetNodeIndex())
//            ,
//            node_dog_out->set_parent_indexes.end());
//  EXPECT_NE(node_family_out->set_parent_indexes.find(node_dog_out->GetNodeIndex())
//            ,
//            node_family_out->set_parent_indexes.end());
//
//
//  DiscreteConfig condition;//compute p(dog_out=0 |bowel_problem=0, family_out=1) where "0" and "1" are the indices of the values of the variables.
//  condition.insert(pair<int,int>(node_bowel_problem->GetNodeIndex(),dynamic_cast<DiscreteNode*>(node_bowel_problem)->vec_potential_vals[0]));
//  condition.insert(pair<int,int>(node_family_out->GetNodeIndex(),dynamic_cast<DiscreteNode*>(node_family_out)->vec_potential_vals[1]));
//  double prob = dynamic_cast<DiscreteNode*>(node_dog_out)->GetProbability(dynamic_cast<DiscreteNode*>(node_dog_out)->vec_potential_vals[0], condition);
//  EXPECT_EQ(prob, 0.97);
//
//}