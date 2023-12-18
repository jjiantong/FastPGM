#pragma clang diagnostic push
#pragma ide diagnostic ignored "openmp-use-default-none"

#include "fastbo/CustomNetwork.h"

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
void CustomNetwork::LoadXMLBIFFile(string file_path, int alpha) {//XMLBIF is the xml file format; BIF: Interchange Format for Bayesian Network.

    // Check if the file exists.
    FILE *test_f_ptr = fopen(file_path.c_str(),"r");
    if (test_f_ptr==nullptr) {
        fprintf(stderr, "Error in function %s!", __FUNCTION__);
        fprintf(stderr, "Unable to open file %s!", file_path.c_str());
        exit(1);
    }
    fclose(test_f_ptr);

    XMLBIFParser xbp(file_path);
    vector<Node*> connected_nodes = xbp.GetConnectedNodes(alpha);

    network_name = xbp.xml_network_name_ptr->GetText();
    num_nodes = connected_nodes.size();
    for (const auto &node_ptr : connected_nodes) { // for each node
        map_idx_node_ptr[node_ptr->GetNodeIndex()] = node_ptr;
    }

    for (const auto &node_ptr : connected_nodes) { // for each node
        // find its children, add the edges
        for (const auto &child_idx: node_ptr->set_children_indexes) {
            Node *child_ptr = FindNodePtrByIndex(child_idx);
            Edge edge(node_ptr, child_ptr, TAIL, ARROW);
            vec_edges.push_back(edge);
            ++num_edges;
        }
    }
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

//    cout << "Data file opened. Begin to load network structure. " << endl;

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
     * 2. "variable", TODO: only support discrete variables now
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

    DiscreteNode *node_ptr = nullptr;
    while (parsed_line.at(0).compare("probability") != 0) { // it is about variable
        if (parsed_line.at(0).compare("variable") == 0) { // case 1: variable BirthAsphyxia {
            node_ptr = new DiscreteNode(node_idx);
            node_ptr->node_name = parsed_line.at(1);
            map_idx_node_ptr.insert(pair<int, Node*>(node_idx, node_ptr)); // todo
            node_idx++;
        } else if (parsed_line.at(0).compare("type") == 0) { // case 2: type discrete [ 2 ] { yes, no };
            if (parsed_line.at(1).compare("discrete") != 0) { // it is not a discrete variable
                fprintf(stderr, "Error in function [%s]\nContain continuous variable in BN!", __FUNCTION__);
                exit(1);
            }
            int num_values = stoi(parsed_line.at(3));
            for (int i = 6; i < 6 + num_values; ++i) {
                string value = TrimRightComma(parsed_line.at(i));
                node_ptr->possible_values_ids[value] = i - 6;
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
    vector<Node*> parent_nodes;
    Node *this_node;
    while (!in_file.eof()) {
        // these two lines have to be at the beginning of the while loop, not the end
        line = TrimLeft(line);
        parsed_line = Split(line, " ");

        if (parsed_line.at(0).compare("probability") == 0) {
            // probability ( LowerBodyO2 | HypDistrib, HypoxiaInO2 ) {
            if (parsed_line.at(1).compare("(") != 0) {
                fprintf(stderr, "Error in function [%s]\nError in format!", __FUNCTION__);
                exit(1);
            }

            // 1) get this node.
            this_node = FindNodePtrByName(parsed_line.at(2));
            parent_nodes.clear();

            if (parsed_line.at(3).compare(")") != 0) { // if this node has parent(s).
                // 2) get the parents of the node
                int pos = 4;
                while (parsed_line.at(pos).compare(")") != 0) {
                    string name = TrimRightComma(parsed_line.at(pos));
                    parent_nodes.push_back(FindNodePtrByName(name));
                    ++pos;
                }

                // 3) add an edge between one parent node -> this node.
                for (const auto &parent_node : parent_nodes) {
                    SetParentChild(parent_node, this_node); // set parent and child relationship
                    Edge edge(parent_node, this_node, TAIL, ARROW);
                    vec_edges.push_back(edge);
                    ++num_edges;
                }
            } // otherwise, it is a node with no parent
        } else if (parsed_line.at(0).compare("}") != 0) {
            // (Equal, Mild) 0.1, 0.3, 0.6; or: table 0.2, 0.8;
            int num_pars = parent_nodes.size(); // number of parents for this node
            int num_values = dynamic_cast<DiscreteNode *>(this_node)->possible_values_ids.size();
            int pos;
            DiscreteConfig comb;
            if (num_pars > 0) { // have parents.
                // construct `comb`: parent configuration
                for (int i = 0; i < num_pars; ++i) { // for each of its parents
                    // save each value of the parent
                    string v1 = TrimLeftParenthesis(parsed_line.at(i));
                    string v2 = TrimRightCommaAndParenthesis(v1);
                    // find the number of this value
                    DiscreteNode *par_ptr = dynamic_cast<DiscreteNode *>(parent_nodes[i]);
                    int num = par_ptr->possible_values_ids[v2];
                    comb.insert(
                            pair<int, int>(
                                    par_ptr->GetNodeIndex(), num));
                }
                pos = num_pars;

            } else { // no parent.
                pos = 1;
            }

            // set probabilities for different values of this node given parent config `comb`.
            for (int i = pos; i < pos + num_values; ++i) { // for each possible value of this node
                string v = TrimRightCommaAndSemicolon(parsed_line.at(i));
                double prob = stod(v);
                dynamic_cast<DiscreteNode*>(this_node)->AddCount(i - pos, comb, prob*10000);

//                cout << this_node->GetNodeIndex() << ", query = " << i - pos << "; ";
//                for (const auto &p: comb) {
//                    cout << p.first << "=" << p.second << ", ";
//                }
//                cout << "; " << prob*10000 << endl;
            }
        }

        getline(in_file, line);
    }
}


#pragma clang diagnostic pop