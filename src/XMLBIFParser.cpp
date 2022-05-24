#include "XMLBIFParser.h"

void XMLBIFParser::LoadFile(string &file) {
    xml_doc.LoadFile(file.c_str());
    xml_network_ptr = xml_doc.FirstChildElement("BIF")->FirstChildElement("NETWORK");
    xml_network_name_ptr = xml_network_ptr->FirstChildElement("NAME");

    XMLElement *xml_var_ptr = xml_network_ptr->FirstChildElement("VARIABLE");
    while (xml_var_ptr != nullptr) {
        vec_xml_vars_ptr.push_back(xml_var_ptr);
        xml_var_ptr = xml_var_ptr->NextSiblingElement("VARIABLE");
    }

    XMLElement *xml_prob_ptr = xml_network_ptr->FirstChildElement("PROBABILITY");
    while (xml_prob_ptr != nullptr) {
        vec_xml_probs_ptr.push_back(xml_prob_ptr);
        xml_prob_ptr = xml_prob_ptr->NextSiblingElement("PROBABILITY");
    }

    if (vec_xml_vars_ptr.size() != vec_xml_probs_ptr.size()) {
        fprintf(stderr, "Error in function %s! Numbers of variables and probabilities not match!", __FUNCTION__);
        exit(1);
    }
}

XMLBIFParser::XMLBIFParser(string &file) {
    LoadFile(file);
}

/**
 * @brief: get all the variables (without parent info)
 */
vector<Node*> XMLBIFParser::GetUnconnectedNodes() const {
    if (!xml_network_ptr) {
        fprintf(stderr, "Error in function %s! nullptr!", __FUNCTION__);
        exit(1);
    }
    if (vec_xml_vars_ptr.empty()) {
        fprintf(stderr, "Error in function %s! No variables!", __FUNCTION__);
        exit(1);
    }

    vector<Node*> vec_node_ptrs;
    for (auto &xvp : vec_xml_vars_ptr) { // for all variables (<VARIABLE>)
        // if the TYPE is "discrete"
        if (((string)xvp->FirstChildElement("TYPE")->GetText()) == "discrete") {
            string name = xvp->FirstChildElement("NAME")->GetText();
            DiscreteNode *n_p = new DiscreteNode(vec_node_ptrs.size(), name);

            XMLElement *xml_val_ptr = xvp->FirstChildElement("VALUE");
            while (xml_val_ptr != nullptr) {
                n_p->vec_str_potential_vals.push_back(((string)xml_val_ptr->GetText()));
                xml_val_ptr = xml_val_ptr->NextSiblingElement("VALUE");
            }
            n_p->SetDomainSize(n_p->vec_str_potential_vals.size());

            n_p->vec_potential_vals.resize(n_p->GetDomainSize());
            for (int i = 0; i < n_p->GetDomainSize(); ++i) {
                n_p->vec_potential_vals[i] = i;
            }
            vec_node_ptrs.push_back(n_p);
        } else {  // if the "TYPE" is "continuous"
//            // todo: implement continuous node
//            Node *n_p = new ContinuousNode();
//            n_p->node_name = xvp->FirstChildElement("NAME")->GetText();
//            fprintf(stderr, "Has not implemented function for continuous node!");
//            exit(1);
        }
    }
    return vec_node_ptrs;
}

/**
 * @brief: read the probabilities and construct parent-child relationship.
 */
void XMLBIFParser::AssignProbsToNodes(vector<Node*> vec_nodes_ptr) {
    for (auto &xpp : vec_xml_probs_ptr) { // Parse each "PROBABILITY" element.
        // Parse "FOR"
        string str_for = (xpp->FirstChildElement("FOR")->GetText());
        DiscreteNode* for_np = nullptr;
        // Find the variable corresponding to this probability.
        for (auto &vnp : vec_nodes_ptr) {
            if (vnp->node_name == str_for) {
                for_np = dynamic_cast<DiscreteNode*>(vnp);
                break;
            }
        }
        if (for_np == nullptr) {
            fprintf(stderr, "Error in function %s! Probability does not match any variable!", __FUNCTION__);
            exit(1);
        }

        // Parse "GIVEN" ==================================================
        // Store variables of all "GIVEN" in a vector.
        // Because, if the parameters are given in the form of "TABLE",
        // we need to do something like binary counting (change from right)
        // for the variables' values in "GIVEN".
        vector<Node*> vec_given_vars_ptrs;
        XMLElement *xg = xpp->FirstChildElement("GIVEN");
        while (xg != nullptr) { // find each of its parents
            string str_given = (string)(xg->GetText());
            Node *given_np = nullptr;
            for (auto &vnp : vec_nodes_ptr) {
                if (vnp->node_name == str_given) {
                    given_np = vnp;
                    break;
                }
            }
            if (given_np == nullptr) {
                fprintf(stderr, "Error in function %s! \"GIVEN %s\" does not match any variable!",
                        __FUNCTION__,str_given.c_str());
                exit(1);
            }
            vec_given_vars_ptrs.push_back(given_np);
            xg = xg->NextSiblingElement("GIVEN");
        }

        for (auto &gvp : vec_given_vars_ptrs) {
            for_np->AddParent(gvp);
            gvp->AddChild(for_np);
        }

        // Parse "TABLE" ==================================================
        string str_table = xpp->FirstChildElement("TABLE")->GetText();
        str_table = Trim(str_table);
        vector<string> vec_str_table_entry = Split(str_table," ");
        vector<double> vec_db_table_entry;
        vec_db_table_entry.reserve(vec_str_table_entry.size());
        for (auto &str : vec_str_table_entry) {
            vec_db_table_entry.push_back(stod(str));
        }

        // The following lines are to generate
        // all combinations of values of variables in "GIVEN".
        vector<int> vec_range_each_digit;
        int num_given = vec_given_vars_ptrs.size();
        vec_range_each_digit.reserve(num_given + 1);

        // The first "digit" is for this node.
        vec_range_each_digit.push_back(for_np->GetDomainSize());

        // The following "digits" are for parents of this node.
        for (int i = 0; i < num_given; ++i) {
            vec_range_each_digit.push_back(dynamic_cast<DiscreteNode*>(vec_given_vars_ptrs[i])->GetDomainSize());
        }

        vector<vector<int>> nary_counts = NaryCount(vec_range_each_digit);

        // Now, nary_counts and vec_db_table_entry should correspond on position.
        // So, they should have the same size.
        if (nary_counts.size() != vec_db_table_entry.size()) {
            fprintf(stderr, "Error in function %s! Two vectors have different sizes!",__FUNCTION__);
            exit(1);
        }

        // Now, set the node's conditional probability map.
        for (int i = 0; i < nary_counts.size(); ++i) {
            vector<int> &digits = nary_counts[i];

            // The first (left-most) digit is for this node.
            int query = for_np->vec_potential_vals[digits[0]];
            DiscreteConfig comb;

            // The first (left-most) digit is for this node not the parents.
            // So, j start at 1.
            // Also, the number of parents is one smaller than the number of "digits".
            for (int j = 1; j < digits.size(); ++j) {
                comb.insert(
                        pair<int,int>(
                                vec_given_vars_ptrs[j-1]->GetNodeIndex(),
                                dynamic_cast<DiscreteNode*>(vec_given_vars_ptrs[j-1])->vec_potential_vals.at(digits[j])));
            }

            // directly set probability based on the input file causes a problem when testing,
            // the problem happens if some probabilities are 0
            // so we use a "laplace_smooth" to avoid 0 probability
//            dynamic_cast<DiscreteNode*>(for_np)->SetProbability(query, comb, vec_db_table_entry.at(i));
            dynamic_cast<DiscreteNode*>(for_np)->AddCount(query, comb, vec_db_table_entry.at(i)*10000);
        }
    }
}

/**
 * @brief: first read all the nodes; then read probabilities and construct parent-child relationships.
 * @return
 */
vector<Node*> XMLBIFParser::GetConnectedNodes() {
    vector<Node*> unconnected_nodes = GetUnconnectedNodes();
    AssignProbsToNodes(unconnected_nodes);
    vector<Node*> &connected_nodes = unconnected_nodes; // Just change a name.

    /************************* data set generation *************************/
    /**
     * this part is used to randomly generate one possible value for each variable
     * we choose variable 0 as the class variable, only output value, but the value is not important!
     * save using the libsvm format, but the value of the class variable is not important!
     * the value of the class variable is wrong, is not the ground-truth! just for the format
     * we don't care the accuracy of the inference...!
     */
//    srand(time(NULL));
//    for (int i = 0; i < 20000; ++i) {
//        for (int j = 0; j < connected_nodes.size(); ++j) {
//            Node *node_ptr = connected_nodes[j];
//            int node_index = node_ptr->GetNodeIndex();
//            string node_name = node_ptr->node_name;
//            DiscreteNode* dis_node = dynamic_cast<DiscreteNode*>(node_ptr);
//            int num_vals = dis_node->GetNumPotentialVals();
//
//            int rand_num = rand() % num_vals;
//            if (node_index == 0) {
//                cout << dis_node->vec_potential_vals[rand_num] << " ";
//            } else {
//                cout << node_index << ":" << dis_node->vec_potential_vals[rand_num] << " ";
//            }
//        }
//        cout << endl;
//    }
    /************************* data set generation *************************/

    return connected_nodes;
}