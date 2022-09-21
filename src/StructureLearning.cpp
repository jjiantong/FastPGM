//
// Created by jjt on 2021/6/17.
//

#include "StructureLearning.h"

/**
 * @brief: add each node in the network, contains:
 * index, name, domain size, possible values
 */
void StructureLearning::AssignNodeInformation(Dataset *dts) {
    network->num_nodes = dts->num_vars;

    // Assign an index for each node.
    for (int i = 0; i < network->num_nodes; ++i) {
        // construct a node in the network
        DiscreteNode *node_ptr = new DiscreteNode(i);

        // give this node a name, mainly for print
        if (dts->vec_var_names.size() == network->num_nodes) {
            node_ptr->node_name = dts->vec_var_names[i];
        } else {
            node_ptr->node_name = to_string(i);//use id as name
        }

        //set the potential values for this node
        int domain_size = dts->num_of_possible_values_of_disc_vars[i];
        node_ptr->SetDomainSize(domain_size);
        network->map_idx_node_ptr[i] = node_ptr;
    }
}

/**
 * @brief: assign an order of the nodes; the order will be used in structure learning
 */
vector<int> StructureLearning::AssignNodeOrder(string topo_ord_constraint) {

    vector<int> ord;
    ord.reserve(network->num_nodes);
    for (int i = 0; i < network->num_nodes; ++i) {
        ord.push_back(i);   // the nodes are created the same order as in the dataset.
    }

    cout << "topo_ord_constraint: " << topo_ord_constraint << endl;
    //choose an order to serve as a constraint during learning
    if (topo_ord_constraint == "dataset-ord") {
        // Do nothing.
    }
    else if (topo_ord_constraint == "random") {
        std::srand(unsigned(std::time(0)));
        std::random_shuffle(ord.begin(), ord.end());
    }
    else if (topo_ord_constraint == "best") {
        ord = vector<int> {};//no order is provided (i.e. no constraint)
    }
    else {
        fprintf(stderr, "Error in function [%s]!\nInvalid topological ordering restriction!", __FUNCTION__);
        exit(1);
    }

    return ord;
}