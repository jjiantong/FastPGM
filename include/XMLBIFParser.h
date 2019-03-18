//
// Created by llj on 3/17/19.
//

#ifndef BAYESIANNETWORK_XMLBIFPARSER_H
#define BAYESIANNETWORK_XMLBIFPARSER_H

#include <stdio.h>
#include "tinyxml2.h"
#include "Node.h"

using namespace tinyxml2;

typedef set< pair<int, int> > Combination;

class XMLBIFParser {

 public:

  XMLDocument xml_doc;
  XMLElement *xml_network_ptr;
  XMLElement *xml_network_name_ptr;
  vector<XMLElement*> vec_xml_vars_ptr;
  vector<XMLElement*> vec_xml_probs_ptr;

  XMLBIFParser() = default;
  XMLBIFParser(string&);
  void LoadFile(string&);

  vector<Node*> GetNodes();
  void AssignProbsToNodes(vector<XMLElement*>, vector<Node*>);

};


#endif //BAYESIANNETWORK_XMLBIFPARSER_H
