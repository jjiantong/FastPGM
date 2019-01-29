//
// Created by LinjianLi on 2019/1/25.
//

#include "CustomNetwork.h"


void CustomNetwork::ConstructCustomNetworkFromFile(string file_path) {

	ifstream in_file;
	in_file.open(file_path);
	if (!in_file.is_open()) {
		fprintf(stderr, "Unable to open file %s!", file_path.c_str());
		exit(1);
	}

	cout << "File opened. Begin to load data and construct custom network. " << endl;

	in_file >> n_nodes;

	for (int i=0; i<n_nodes; ++i) {
		Node *node_ptr = new Node();
		node_ptr->SetNodeIndex(i);
		set_node_ptr_container.insert(node_ptr);
	}

	string node_and_children;
	vector<string> parsed_node_and_children;
	vector<string>::iterator it;
	int index, value;

	// Load data.
	getline(in_file, node_and_children);
	getline(in_file, node_and_children);
	while (!in_file.eof()) {
		node_and_children = boost::algorithm::trim_right_copy(node_and_children);
		boost::algorithm::split(parsed_node_and_children, node_and_children, boost::algorithm::is_space());

		it=parsed_node_and_children.begin();
		int par_node_index = stoi(*it);
		for (it+=2; it!=parsed_node_and_children.end(); ++it) {
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


void CustomNetwork::StructLearnCompData(Trainer *) {
	// todo: implement
	return;
}


pair<int*, int> CustomNetwork::SimplifyDefaultElimOrd() {
	// todo: implement
	return pair<int*, int> (default_elim_ord, n_nodes-1);
}