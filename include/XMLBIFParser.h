//
// Created by llj on 3/17/19.
//

#ifndef BAYESIANNETWORK_XMLBIFPARSER_H
#define BAYESIANNETWORK_XMLBIFPARSER_H

#include <cstdio>
#include "tinyxml2.h"
#include "Node.h"
#include "DiscreteNode.h"
#include "ContinuousNode.h"

using namespace tinyxml2;

class XMLBIFParser {

 public:

  XMLDocument xml_doc;
  XMLElement *xml_network_ptr;
  XMLElement *xml_network_name_ptr;
  vector<XMLElement*> vec_xml_vars_ptr;
  vector<XMLElement*> vec_xml_probs_ptr;

  XMLBIFParser() = default;
  XMLBIFParser(string &file);
  void LoadFile(string &file);

  vector<Node*> GetUnconnectedNodes() const;
  void AssignProbsToNodes(vector<XMLElement*> vec_xml_elems_ptr, vector<Node*> vec_nodes_ptr);

  vector<Node*> GetConnectedNodes();

};


#endif //BAYESIANNETWORK_XMLBIFPARSER_H