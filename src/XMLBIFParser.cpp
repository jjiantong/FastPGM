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
    // todo: do something
    vec_xml_vars_ptr.push_back(xml_var_ptr);
    xml_var_ptr = xml_var_ptr->NextSiblingElement("VARIABLE");
  }

  XMLElement *xml_prob_ptr = xml_network_ptr->FirstChildElement("PROBABILITY");
  while (xml_prob_ptr!=nullptr) {
    // todo: do something
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

vector<Node*> XMLBIFParser::GetNodes() {
  if (!xml_network_ptr) {
    fprintf(stderr, "Error in function %s! nullptr!", __FUNCTION__);
    exit(1);
  }
  if (vec_xml_vars_ptr.empty()) {
    fprintf(stderr, "Error in function %s! No variables!", __FUNCTION__);
    exit(1);
  }
  vector<Node*> vec_ndoe_ptrs;
  for (auto &xvp : vec_xml_vars_ptr) {
    Node *n_p = new Node();
    n_p->node_name = xvp->FirstChildElement("NAME")->GetText();
    n_p->is_discrete =
            ((string)xvp->FirstChildElement("NAME")->GetText())=="discrete";

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
    vec_ndoe_ptrs.push_back(n_p);
  }
  return vec_ndoe_ptrs;
}

void XMLBIFParser::AssignProbsToNodes(vector<XMLElement*> vec_xml_elems_ptr, vector<Node*> vec_nodes_ptr) {
  for (auto &xpp : vec_xml_elems_ptr) {
    string str_for = (string)(xpp->FirstChildElement("FOR")->GetText());
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

    // Store variables of all "GIVEN" in a vector.
    // Because, if the parameters are given in the form of table,
    // we need to do something like binary counting (change from right)
    // for the variables' values in "GIVEN".
    vector<Node*> vec_given_vars;
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
      vec_given_vars.push_back(given_np);
      xg = xg->NextSiblingElement("GIVEN");
    }
    int num_given = vec_given_vars.size();
    int *num_range_givens = new int[num_given];
    for (int i=0; i<num_given; ++i) {
      num_range_givens[i] = vec_given_vars[i]->num_potential_vals;
    }


  }
}