//
// Created by llj on 3/17/19.
//

#include "XMLBIFParser.h"

void XMLBIFParser::LoadFile(string &file) {
  xml_doc.LoadFile(file.c_str());
  xml_network_ptr = xml_doc.FirstChildElement("BIF")->FirstChildElement("NETWORK");
  xml_network_name_ptr = xml_network_ptr->FirstChildElement("NAME");

  XMLElement *xml_var_ptr = xml_network_ptr->FirstChildElement("VARIABLE");
  while (xml_var_ptr!=nullptr) {
    vec_xml_vars_ptr.push_back(xml_var_ptr);
    xml_var_ptr = xml_var_ptr->NextSiblingElement("VARIABLE");
  }

  XMLElement *xml_prob_ptr = xml_network_ptr->FirstChildElement("PROBABILITY");
  while (xml_prob_ptr!=nullptr) {
    vec_xml_probs_ptr.push_back(xml_prob_ptr);
    xml_prob_ptr = xml_prob_ptr->NextSiblingElement("PROBABILITY");
  }

  if (vec_xml_vars_ptr.size()!=vec_xml_probs_ptr.size()) {
    fprintf(stderr, "Error in function %s! Numbers of variables and probabilities not match!", __FUNCTION__);
    exit(1);
  }
}

XMLBIFParser::XMLBIFParser(string &file) {
  LoadFile(file);
}

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
  for (auto &xvp : vec_xml_vars_ptr) {
    if (((string)xvp->FirstChildElement("TYPE")->GetText())=="discrete") {
      Node *n_p = new DiscreteNode();
      n_p->node_name = xvp->FirstChildElement("NAME")->GetText();
      n_p->is_discrete =
              ((string)xvp->FirstChildElement("TYPE")->GetText())=="discrete";

      XMLElement *xml_val_ptr = xvp->FirstChildElement("VALUE");
      while (xml_val_ptr!=nullptr) {
        n_p->vec_str_potential_vals.push_back(((string)xml_val_ptr->GetText()));
        xml_val_ptr = xml_val_ptr->NextSiblingElement("VALUE");
      }
      n_p->num_potential_vals = n_p->vec_str_potential_vals.size();
      n_p->potential_vals = new int[n_p->num_potential_vals];
      for (int i=0; i<n_p->num_potential_vals; ++i) {
        n_p->potential_vals[i] = i;
        n_p->vec_potential_vals.push_back(i);
      }
      n_p->SetNodeIndex(vec_node_ptrs.size());
      vec_node_ptrs.push_back(n_p);
    } else {
      // todo: implement continuous node
      fprintf(stderr, "Has not implemented function for continuous node!");
      exit(1);
    }

  }
  return vec_node_ptrs;
}

void XMLBIFParser::AssignProbsToNodes(vector<XMLElement*> vec_xml_elems_ptr, vector<Node*> vec_nodes_ptr) {
  for (auto &xpp : vec_xml_elems_ptr) { // Parse each "PROBABILITY" element.



    // Parse "FOR"

    string str_for = (xpp->FirstChildElement("FOR")->GetText());
    Node* for_np = nullptr;
    // Find the variable corresponding to this probability.
    for (auto &vnp : vec_nodes_ptr) {
      if (vnp->node_name==str_for) {
        for_np = vnp;
        break;
      }
    }
    if (for_np==nullptr) {
      fprintf(stderr, "Error in function %s! Probability does not match any variable!", __FUNCTION__);
      exit(1);
    }



    // Parse "GIVEN"

    // Store variables of all "GIVEN" in a vector.
    // Because, if the parameters are given in the form of "TABLE",
    // we need to do something like binary counting (change from right)
    // for the variables' values in "GIVEN".
    vector<Node*> vec_given_vars_ptrs;
    XMLElement *xg = xpp->FirstChildElement("GIVEN");
    while (xg!=nullptr) {
      string str_given = (string)(xg->GetText());
      Node *given_np = nullptr;
      for (auto &vnp : vec_nodes_ptr) {
        if (vnp->node_name==str_given) {
          given_np = vnp;
          break;
        }
      }
      if (given_np==nullptr) {
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

    for_np->GenDiscParCombs();



    // Parse "TABLE"

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
    vec_range_each_digit.reserve(num_given+1);

    // The first "digit" is for this node.
    vec_range_each_digit.push_back(for_np->num_potential_vals);

    // The following "digits" are for parents of this node.
    for (int i=0; i<num_given; ++i) {
      vec_range_each_digit.push_back(vec_given_vars_ptrs[i]->num_potential_vals);
    }

    vector<vector<int>> nary_counts = NaryCount(vec_range_each_digit);

    // Now, nary_counts and vec_db_table_entry should correspond on position.
    // So, they should have the same size.
    if (nary_counts.size()!=vec_db_table_entry.size()) {
      fprintf(stderr, "Error in function %s! Two vectors with different sizes!",__FUNCTION__);
      exit(1);
    }

    // Now, set the node's conditional probability map.
    for (int i=0; i<nary_counts.size(); ++i) {
      vector<int> &digits = nary_counts[i];

      // The first (left-most) digit is for this node.
      int query = for_np->vec_potential_vals[digits[0]];
      Combination comb;

      // The first (left-most) digit is for this node not the parents.
      // So, j start at 1.
      // Also, the number of parents is one smaller than the number of "digits".
      for (int j=1; j<digits.size(); ++j) {
        comb.insert(
                pair<int,int>(
                        vec_given_vars_ptrs[j-1]->GetNodeIndex(),
                        vec_given_vars_ptrs[j-1]->vec_potential_vals[digits[j]])
        );
      }

      if (digits.size()==1) {
        // If true, then this node does not have parent.
        for_np->map_marg_prob_table[query] = vec_db_table_entry[i];
      } else {
        for_np->map_cond_prob_table[query][comb] = vec_db_table_entry[i];
      }

    }
  }
}

vector<Node*> XMLBIFParser::GetConnectedNodes() {
  vector<Node*> unconnected_nodes = GetUnconnectedNodes();
  AssignProbsToNodes(vec_xml_probs_ptr,unconnected_nodes);
  vector<Node*> &connected_nodes = unconnected_nodes; // Just change a name.
  return connected_nodes;
}