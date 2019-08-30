//
// Created by LinjianLi on 2019/1/25.
//

#include "CustomNetwork.h"

CustomNetwork::CustomNetwork(bool pure_disc) {
  this->pure_discrete = pure_disc;
}

void CustomNetwork::LearnParmsKnowStructCompData(const Trainer *trn, bool print_params) {
  cout << "==================================================" << '\n'
       << "Begin learning parameters with known structure and complete data." << endl;

  struct timeval start, end;
  double diff;
  gettimeofday(&start,NULL);

  int num_cores = omp_get_num_procs();
  omp_set_num_threads(num_cores);
  int max_work_per_thread = (trn->num_vars+num_cores-1)/num_cores;
  #pragma omp parallel
  {
    // For every node.
    for (int i = max_work_per_thread*omp_get_thread_num();
         i < max_work_per_thread*(omp_get_thread_num()+1) && i < trn->num_vars;
         ++i) {
//    for (int i=1; i<trainer->num_vars; ++i) {
      Node *this_node = FindNodePtrByIndex(i);
      if (this_node->set_parents_ptrs.empty()) {

        map<int, double> *MPT = &(this_node->map_marg_prob_table);
        int denominator = 0;
        for (int s = 0; s < trn->num_instance; ++s) {
          denominator += 1;
          int query = trn->dataset_all_vars[s][i];
          (*MPT)[query] += 1;
        }
        for (int ii = 0; ii < this_node->num_potential_vals; ++ii) {
          int query = this_node->potential_vals[ii];
          (*MPT)[query] /= denominator;
        }

      } else {  // If the node has parents.

        map<int, map<Combination, double> > *CPT = &(this_node->map_cond_prob_table);
        set<Combination> *ptr_set_par_combs = &(this_node->set_discrete_parents_combinations);
        for (auto &par_comb : *ptr_set_par_combs) {    // For each column in CPT. Because the sum over column of CPT must be 1.
          int denominator = 0;
          for (int s = 0; s < trn->num_instance; ++s) {
            int compatibility = 1;  // We assume compatibility is 1,
            // and set it to 0 if we find that (*it_par_comb) is not compatible with (trainer->train_set[s]).
            // If we support learning with incomplete data,
            // the compatibility can be between 0 and 1.

            for (const auto &index_value : par_comb) {
              if (trn->dataset_all_vars[s][index_value.first] != index_value.second) {
                compatibility = 0;
                break;
              }
            }
            denominator += compatibility;
            int query = trn->dataset_all_vars[s][i];
            (*CPT)[query][par_comb] += compatibility;
          }
          // Normalize so that the sum is 1.
          for (int j = 0; j < this_node->num_potential_vals; ++j) {
            int query = this_node->potential_vals[j];
            (*CPT)[query][par_comb] /= denominator;
          }
        }

      }
    }
  }   // end of: #pragma omp parallel
  cout << "==================================================" << '\n'
       << "Finish training with known structure and complete data." << endl;

  if (print_params) {
    cout << "==================================================" << '\n'
         << "Each node's conditional probability table: " << endl;
    for (const auto &node_ptr : set_node_ptr_container) {  // For each node
      node_ptr->PrintProbabilityTable();
    }
  }

  gettimeofday(&end,NULL);
  diff = (end.tv_sec-start.tv_sec) + ((double)(end.tv_usec-start.tv_usec))/1.0E6;
  setlocale(LC_NUMERIC, "");
  cout << "==================================================" << '\n'
       << "The time spent to learn the parameters is " << diff << " seconds" << endl;
}

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