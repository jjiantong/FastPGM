//
// Created by llj on 3/17/19.
//

#include <iostream>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "gtest/gtest.h"
#include "tinyxml2.h"

using namespace std;
using namespace tinyxml2;

TEST(TinyXML, DISABLED_usability) {
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