#include <string>
#include <vector>
#include "gtest/gtest.h"
#include "tinyxml2.h"

#include "CustomNetwork.h"

using namespace std;
using namespace tinyxml2;

TEST(XMLBIFParser, DISABLED_usability) {
  string file = "../../data/interchange-format-file/dog-problem.xml";
  XMLDocument doc;
  doc.LoadFile(file.c_str());
  const XMLElement *net = doc.FirstChildElement("BIF")->FirstChildElement("NETWORK");

  string elem_type = net->Value();
  EXPECT_EQ(elem_type,"NETWORK");

  string net_name = net->FirstChildElement("NAME")->GetText();
  EXPECT_EQ(net_name,"Dog-Problem");

  const XMLElement *var1 = net->FirstChildElement("VARIABLE");
  string var1_name = var1->FirstChildElement("NAME")->GetText();
  EXPECT_EQ(var1_name,"light-on");

  EXPECT_EQ((string)net->FirstChildElement()
                       ->NextSiblingElement()
                       ->FirstChildElement()->GetText(),
             "light-on");

}

TEST(XMLBIFParser, dog_problem_xmlbif) {
  string file = "../../data/interchange-format-file/dog-problem.xml";
  CustomNetwork dog_net;
  dog_net.GetNetFromXMLBIFFile(file);
  auto *node_family_out = dog_net.FindNodePtrByName("family-out");
  auto *node_bowel_problem = dog_net.FindNodePtrByName("bowel-problem");
  auto *node_dog_out = dog_net.FindNodePtrByName("dog-out");

  EXPECT_EQ(node_family_out->set_parents_ptrs.size(),0);
  EXPECT_EQ(node_bowel_problem->set_parents_ptrs.size(),0);
  EXPECT_EQ(node_dog_out->set_parents_ptrs.size(),2);
  EXPECT_NE(node_dog_out->set_parents_ptrs.find(node_family_out)
            ,
            node_dog_out->set_parents_ptrs.end());
  EXPECT_NE(node_dog_out->set_parents_ptrs.find(node_bowel_problem)
            ,
            node_dog_out->set_parents_ptrs.end());
  EXPECT_NE(node_family_out->set_children_ptrs.find(node_dog_out)
            ,
            node_family_out->set_children_ptrs.end());


  DiscreteConfig condition;
  condition.insert(pair<int,int>(node_bowel_problem->GetNodeIndex(),node_bowel_problem->vec_potential_vals[0]));
  condition.insert(pair<int,int>(node_family_out->GetNodeIndex(),node_family_out->vec_potential_vals[1]));
  EXPECT_EQ(dynamic_cast<DiscreteNode*>(node_dog_out)->map_cond_prob_table[node_dog_out->vec_potential_vals[0]][condition],0.97);

}