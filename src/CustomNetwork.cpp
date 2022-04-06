#pragma clang diagnostic push
#pragma ide diagnostic ignored "openmp-use-default-none"

#include "CustomNetwork.h"

CustomNetwork::CustomNetwork(): CustomNetwork(true) {}

CustomNetwork::CustomNetwork(bool pure_disc) {
  this->pure_discrete = pure_disc;//whether the network only contains discrete variable (Gaussian networks contain numeric variables)
}

//TODO: implement/improve this function
vector<int> CustomNetwork::SimplifyDefaultElimOrd(DiscreteConfig evidence) {
  return vec_default_elim_ord;
}

/**
 * @brief: construct a network using XML file (cf. dog-problem.xml under "interchange-format-file" folder for an example)
 */
void CustomNetwork::GetNetFromXMLBIFFile(string file_path) {//XMLBIF is the xml file format; BIF: Interchange Format for Bayesian Network.
  
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
    map_idx_node_ptr[node_ptr->GetNodeIndex()] = node_ptr;
  }

  GenTopoOrd();
}

/**
 * @brief: construct a DAG using a BIF file
 * @example: interchange-format-file/asia.bif
 * we only care about nodes and edges (the relationships between nodes)
 * we do not care about the dimension or possible values of the nodes, or the CPTs
 */
void CustomNetwork::LoadBIFFile(string path) {
    ifstream in_file;
    in_file.open(path);
    if (!in_file.is_open()) {
        fprintf(stderr, "Error in function %s!", __FUNCTION__);
        fprintf(stderr, "Unable to open file %s!", path.c_str());
        exit(1);
    }

    cout << "Data file opened. Begin to load network structure. " << endl;

    string line;
    /**
     * 1. "network"
     * network unknown {
     * }
     * TODO: here we just skip the first two lines
     */
    getline(in_file, line);
    getline(in_file, line);

    /**
     * 2. "variable", we only care about the node name, not about the dimension and possible values
     * TODO: only support discrete variables now
     * variable BirthAsphyxia {
     *  type discrete [ 2 ] { yes, no };
     * }
     * variable HypDistrib {
     *  type discrete [ 2 ] { Equal, Unequal };
     * }
     */
    int node_idx = 0;
    getline(in_file, line);
    // if there is a whitespace at the beginning  of the line
    // it will cause a bug if we do not trim it
    line = TrimLeft(line);
    vector<string> parsed_line = Split(line, " ");

    while (parsed_line.at(0).compare("probability") != 0) { // it is about variable
        if (parsed_line.at(0).compare("variable") == 0) { // case 1: variable BirthAsphyxia {
            DiscreteNode *node_ptr = new DiscreteNode(node_idx);
            // give this node a name, mainly for print
            node_ptr->node_name = parsed_line.at(1);
            map_idx_node_ptr.insert(pair<int, Node*>(node_idx, node_ptr)); // todo
            node_idx++;
        } else if (parsed_line.at(0).compare("type") == 0) {
            if (parsed_line.at(1).compare("discrete") != 0) { // it is not a discrete variable
                fprintf(stderr, "Error in function [%s]\nContain continuous variable in BN!", __FUNCTION__);
                exit(1);
            }
        } // do nothing for the other case

        getline(in_file, line);
        line = TrimLeft(line);
        parsed_line = Split(line, " ");
    }
    num_nodes = map_idx_node_ptr.size();

    /**
     * 3. "probability", we only care about the edges, not about the CPTs
     * probability ( LowerBodyO2 | HypDistrib, HypoxiaInO2 ) {
     *  (Equal, Mild) 0.1, 0.3, 0.6;
     *  (Unequal, Mild) 0.4, 0.5, 0.1;
     *  (Equal, Moderate) 0.3, 0.6, 0.1;
     *  (Unequal, Moderate) 0.50, 0.45, 0.05;
     *  (Equal, Severe) 0.5, 0.4, 0.1;
     *  (Unequal, Severe) 0.60, 0.35, 0.05;
     * }
     * it seems to have slight format difference in this part, but it does not matter since we do not care about CPTs
     */
    while (!in_file.eof()) {
        // these two lines have to be at the beginning of the while loop, not the end
        line = TrimLeft(line);
        parsed_line = Split(line, " ");

        if (parsed_line.at(0).compare("probability") == 0) { // we do not care about other lines
            if (parsed_line.at(1).compare("(") != 0) {
                fprintf(stderr, "Error in function [%s]\nError in format!", __FUNCTION__);
                exit(1);
            }
            if (parsed_line.at(3).compare(")") != 0) {
                // 1) get the child node
                Node* child_node = FindNodePtrByName(parsed_line.at(2));

                // 2) get the parents of the node
                vector<Node*> parent_nodes;
                int pos = 4;
                while (parsed_line.at(pos).compare(")") != 0) {
                    string name = TrimRightComma(parsed_line.at(pos));
                    parent_nodes.push_back(FindNodePtrByName(name));
                    ++pos;
                }

                // 3) add an edge between one parent node -> the child node
                for (const auto &parent_node : parent_nodes) {
                    SetParentChild(parent_node, child_node); // set parent and child relationship
                    Edge edge(parent_node, child_node, TAIL, ARROW);
                    vec_edges.push_back(edge);
                    ++num_edges;
                }
            } // otherwise, it is a node with no parent
        }

        getline(in_file, line);
    }
}


#pragma clang diagnostic pop