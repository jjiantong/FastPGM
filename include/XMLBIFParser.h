#ifndef BAYESIANNETWORK_XMLBIFPARSER_H
#define BAYESIANNETWORK_XMLBIFPARSER_H

#include <cstdio>
#include <cstdlib>
#include <time.h>

#include "tinyxml2.h"
#include "Node.h"
#include "DiscreteNode.h"
#include "ContinuousNode.h"

using namespace tinyxml2;

class XMLBIFParser {

 public:

  XMLDocument xml_doc;
  XMLElement *xml_network_ptr; // the entire node of the network (<NETWORK>)
  XMLElement *xml_network_name_ptr; // <NAME>
  vector<XMLElement*> vec_xml_vars_ptr; // <VARIABLE>
  vector<XMLElement*> vec_xml_probs_ptr; //<PROBABILITY>

  XMLBIFParser() = default;
  XMLBIFParser(string &file);
  void LoadFile(string &file);

  vector<Node*> GetUnconnectedNodes() const;
  void AssignProbsToNodes(vector<Node*> vec_nodes_ptr);

  vector<Node*> GetConnectedNodes();

};


#endif //BAYESIANNETWORK_XMLBIFPARSER_H
