//
// Created by LinjianLi on 2019/1/25.
//

#include "CustomNetwork.h"

pair<int*, int> CustomNetwork::SimplifyDefaultElimOrd(Combination evidence) {
  return {default_elim_ord, num_nodes-1};
}

void CustomNetwork::GetNetFromXMLBIFFile(string file_path) {
  
  // Check if the file exists.
  FILE *test_f_ptr = fopen(file_path.c_str(),"r");
  if (test_f_ptr==nullptr) {
    fprintf(stderr, "Error in function %s!", __FUNCTION__);
    fprintf(stderr, "Unable to open file %s!", file_path.c_str());
    exit(1);
  }
  
  XMLBIFParser xbp(file_path);
  vector<Node*> connected_nodes = xbp.GetConnectedNodes();
  
  network_name = xbp.xml_network_name_ptr->GetText();
  num_nodes = connected_nodes.size();
  for (auto &node_ptr : connected_nodes) {
    set_node_ptr_container.insert(node_ptr);
  }

  GenTopoOrd();
}