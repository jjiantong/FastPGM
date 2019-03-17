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

TEST(TinyXML, usability) {
  XMLDocument doc;
  doc.LoadFile("../../data/interchange-format-file/dog-problem.xml");
}