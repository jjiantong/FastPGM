#pragma clang diagnostic push
#pragma ide diagnostic ignored "openmp-use-default-none"
//
// Created by LinjianLi on 2019/1/25.
//

#include "CustomNetwork.h"

CustomNetwork::CustomNetwork(): CustomNetwork(true) {}

CustomNetwork::CustomNetwork(bool pure_disc) {
  this->pure_discrete = pure_disc;//whether the network only contains discrete variable (Gaussian networks contain numeric variables)
}

vector<int> CustomNetwork::SimplifyDefaultElimOrd(DiscreteConfig evidence) {
  return vec_default_elim_ord;
}

void CustomNetwork::GetNetFromXMLBIFFile(string file_path) {//XMLBIF is the xml file format; BIF: Interchange Format for Bayesian Network.
  
  // Check if the file exists.
  FILE *test_f_ptr = fopen(file_path.c_str(),"r");
  if (test_f_ptr==nullptr) {
    fprintf(stderr, "Error in function %s!", __FUNCTION__);
    fprintf(stderr, "Unable to open file %s!", file_path.c_str());
    exit(1);
  }
//TODO: Check XMLBIF Parser and the rest of this function

 XMLBIFParser xbp(file_path);
  vector<Node*> connected_nodes = xbp.GetConnectedNodes();
  
  network_name = xbp.xml_network_name_ptr->GetText();
  num_nodes = connected_nodes.size();
  for (auto &node_ptr : connected_nodes) {
    map_idx_node_ptr[node_ptr->GetNodeIndex()] = node_ptr;
  }

  GenTopoOrd();
}
#pragma clang diagnostic pop