# class XMLBIFParser

## XMLBIFParser(string& file)
Call `LoadFile`.

## void LoadFile(string& file)
Load the XMLBIF file, and store the needed data (`NAME`, `VARIABLE`, `PROBABILITY`) into the an instance of class `XMLBIFParser`. 

## vector<Node\*> GetUnconnectedNodes()
After loading the XMLBIF file, get a `vector` of unconnected nodes from the data of file. The nodes get do not contain information about links and probabilities.

## void AssignProbsToNodes(vector<XMLElement\*> vec_xml_elems_ptr, vector<Node\*> vec_nodes_ptr)
After getting a `vector` of unconnected nodes, assign probabilities in the XMLBIF file to the nodes and add links.


## vector<Node\*> GetConnectedNodes()
First, call `GetUnconnectedNodes` get a `vector` of unconnected nodes. Second, call `AssignProbsToNodes` to get a `vector` of connected nodes.