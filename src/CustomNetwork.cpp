//
// Created by LinjianLi on 2019/1/25.
//

#include "CustomNetwork.h"


void CustomNetwork::ConstructCustomNetworkStructFromFile(string file_path) {

	ifstream in_file;
	in_file.open(file_path);
	if (!in_file.is_open()) {
		fprintf(stderr, "Unable to open file %s!", file_path.c_str());
		exit(1);
	}

	cout << "File opened. Begin to load data and construct custom network. " << endl;

	string sentinel;
	getline(in_file, sentinel);
	while(sentinel.substr(0,18)!="BEGIN_OF_STRUCTURE") {
		if (in_file.eof()) {
			fprintf(stderr, "Unable to find \"BEGIN_OF_STRUCTURE\"!");
			exit(1);
		}
		getline(in_file, sentinel);
	}

	in_file >> n_nodes;

	for (int i=0; i<n_nodes; ++i) {
		Node *node_ptr = new Node();
		node_ptr->SetNodeIndex(i);
		set_node_ptr_container.insert(node_ptr);
	}

	string node_and_children;
	vector<string> vec_parsed_node_and_children;
	vector<string>::iterator it;

	// Load data.
	getline(in_file, node_and_children);
	getline(in_file, node_and_children);

	while (node_and_children.substr(0,16)!="END_OF_STRUCTURE") {
		if (in_file.eof()) {
			fprintf(stderr, "Unable to find \"END_OF_STRUCTURE\"!");
			exit(1);
		}

		node_and_children = boost::algorithm::trim_right_copy(node_and_children);
		boost::algorithm::split(vec_parsed_node_and_children, node_and_children, boost::algorithm::is_space());

		it=vec_parsed_node_and_children.begin();
		int par_node_index = stoi(*it);
		for (it+=2; it!=vec_parsed_node_and_children.end(); ++it) {
			SetParentChild(par_node_index, stoi(*it));
		}
		getline(in_file, node_and_children);
	}


	cout << "Finish constructing custom network. " << endl;
	in_file.close();

	cout << "=======================================================================" << '\n'
	     << "Each node's parents: " << endl;
	for (auto ptr_this_node : set_node_ptr_container) {
		cout << ptr_this_node->GetNodeIndex() << ":\t";
		for (auto ptr_par_node : ptr_this_node->set_parents_pointers) {
			cout << ptr_par_node->GetNodeIndex() << '\t';
		}
		cout << endl;
	}

	cout << "=======================================================================" << '\n'
	     << "Each node's children: " << endl;
	for (auto ptr_this_node : set_node_ptr_container) {
		cout << ptr_this_node->GetNodeIndex() << ":\t";
		for (auto ptr_child_node : ptr_this_node->set_children_pointers) {
			cout << ptr_child_node->GetNodeIndex() << '\t';
		}
		cout << endl;
	}
}

void CustomNetwork::SetCustomNetworkParamsFromFile(string file_path) {
	ifstream in_file;
	in_file.open(file_path);
	if (!in_file.is_open()) {
		fprintf(stderr, "Unable to open file %s!", file_path.c_str());
		exit(1);
	}

	cout << "Begin to set parameters. " << endl;

	string sentinel;
	getline(in_file, sentinel);
	while(sentinel.substr(0,19)!="BEGIN_OF_PARAMETERS") {
		if (in_file.eof()) {
			fprintf(stderr, "Unable to find \"BEGIN_OF_PARAMETERS\"!");
			exit(1);
		}
		getline(in_file, sentinel);
	}


	string node_and_params;
	vector<string> vec_parsed_node_and_params,
	               vec_parsed_query_and_cond_prob,
	               vec_parsed_cond_and_prob,
	               vec_parsed_conds,
	               vec_parsed_single_cond;
	vector<string>::iterator it;
	int index, value;

	// Set num_of_potential_values and potential_values
	for (int i=0; i<n_nodes; ++i) {
		getline(in_file, node_and_params);
		node_and_params = boost::algorithm::trim_right_copy(node_and_params);
		boost::algorithm::split(vec_parsed_node_and_params, node_and_params, boost::algorithm::is_space());
		Node *node_ptr = GivenIndexToFindNodePointer(stoi(vec_parsed_node_and_params[0]));
		node_ptr->is_discrete = true;
		node_ptr->num_of_potential_values = vec_parsed_node_and_params.size()-2;
		node_ptr->potential_values = new int[node_ptr->num_of_potential_values];
		for (int j=0,k=2; j<node_ptr->num_of_potential_values; ++j,++k) {
			node_ptr->potential_values[j] = stoi(vec_parsed_node_and_params[k]);
		}
	}

	// Set set_parents_combinations
	for (int i=0; i<n_nodes; ++i) {
		Node *node_ptr = GivenIndexToFindNodePointer(i);
		node_ptr->GenParCombs();
	}

	// Set probability table
	for (int i=0; i<n_nodes; ++i) {
		getline(in_file, node_and_params);
		node_and_params = boost::algorithm::trim_right_copy(node_and_params);
		boost::algorithm::split(vec_parsed_node_and_params, node_and_params, boost::algorithm::is_space());
		Node *node_ptr = GivenIndexToFindNodePointer(stoi(vec_parsed_node_and_params[0]));
		// todo: implement
		if (vec_parsed_node_and_params[1]=="--marg") {
			map<int, double> *MPT = &(node_ptr->map_marg_prob_table);

			vec_parsed_node_and_params.erase(vec_parsed_node_and_params.begin(), vec_parsed_node_and_params.begin()+2);
			// For convenience.
			// Now, vec_parsed_node_and_params does not contain the node index and "--marg" or "--cond".
			// It only contain query and conditions and probability.

			for (auto &parm : vec_parsed_node_and_params) {
				boost::algorithm::split(vec_parsed_query_and_cond_prob, parm, boost::algorithm::is_any_of("@"));
				int query_value = stoi(vec_parsed_query_and_cond_prob[0]);
				double posibility = stod(vec_parsed_query_and_cond_prob[1]);
				(*MPT)[query_value] = posibility;
			}

		} else if (vec_parsed_node_and_params[1]=="--cond") {
			map<int, map<Combination, double> >* CPT = &(node_ptr->map_cond_prob_table);
			set<Combination>* ptr_set_par_combs = &(node_ptr->set_parents_combinations);

			vec_parsed_node_and_params.erase(vec_parsed_node_and_params.begin(), vec_parsed_node_and_params.begin()+2);
			// For convenience.
			// Now, vec_parsed_node_and_params does not contain the node index and "--marg" or "--cond".
			// It only contain query and conditions and probability.

			for (auto &parm : vec_parsed_node_and_params) {  // For each entry in probability table.
				boost::algorithm::split(vec_parsed_query_and_cond_prob, parm, boost::algorithm::is_any_of("|"));
				int query_value = stoi(vec_parsed_query_and_cond_prob[0]);

				vec_parsed_query_and_cond_prob.erase(vec_parsed_query_and_cond_prob.begin());
				// For convenience.
				// Now, vec_parsed_query_and_cond_prob does not contain the query value.
				// It only contain conditions and probability.

				boost::algorithm::split(vec_parsed_cond_and_prob, vec_parsed_query_and_cond_prob[0], boost::algorithm::is_any_of("@"));
				double probability = stod(vec_parsed_cond_and_prob.back());

				vec_parsed_cond_and_prob.pop_back();
				// For convenience.
				// Now, vec_parsed_cond_and_prob does not contain probability.
				// It only contain conditions.

				boost::algorithm::split(vec_parsed_conds, vec_parsed_cond_and_prob[0], boost::algorithm::is_any_of(","));
				Combination comb_condition;
				for (auto &cond : vec_parsed_conds) {
					boost::algorithm::split(vec_parsed_single_cond, cond, boost::algorithm::is_any_of(":"));
					index = stoi(vec_parsed_single_cond[0]);
					value = stoi(vec_parsed_single_cond[1]);
					comb_condition.insert(pair<int,int>(index,value));
				}
				if (ptr_set_par_combs->find(comb_condition)==ptr_set_par_combs->end()) {
					fprintf(stderr, "Error in function %s! \nParents combination do not exist!", __FUNCTION__);
					fprintf(stderr, "Line in file: %s \n", node_and_params.c_str());
					for (auto &p : comb_condition) {
						cerr << p.first << ':' << p.second << ", ";
					}
					cerr << endl;
					exit(1);
				}
				(*CPT)[query_value][comb_condition] = probability;
			}
		}
	}

	while (node_and_params.substr(0,17)!="END_OF_PARAMETERS") {
		if (in_file.eof()) {
			fprintf(stderr, "Unable to find \"END_OF_PARAMETERS\"!");
			exit(1);
		}
		getline(in_file, node_and_params);
		node_and_params = boost::algorithm::trim_right_copy(node_and_params);
	}

	cout << "Finish setting parameters. " << endl;

}


void CustomNetwork::StructLearnCompData(Trainer *) {
	// todo: implement
	return;
}


pair<int*, int> CustomNetwork::SimplifyDefaultElimOrd() {
	// todo: implement
	return pair<int*, int> (default_elim_ord, n_nodes-1);
}