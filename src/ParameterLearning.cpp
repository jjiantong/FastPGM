//
// Created by jjt on 2021/6/23.
//

#include "ParameterLearning.h"

/**
 * @brief: get each node's conditional probability table
 */
void ParameterLearning::LearnParamsKnowStructCompData(const Dataset *dts, int alpha, int verbose){
    if (verbose > 0) {
        cout << "==================================================" << '\n'
             << "Begin learning parameters, Laplace smoothing param alpha = " << alpha << endl;
    }

    Timer *timer = new Timer();
    timer->Start("parameter_learning");

    int root_idx = (network->num_nodes == dts->num_vars) ? -1: dts->num_vars;
    for (int i = 0; i < dts->num_vars; ++i) {
        // for each variable/node, update probability table of (node | parent configurations)
        DiscreteNode *this_node = dynamic_cast<DiscreteNode*>(network->FindNodePtrByIndex(i));   // todo: support continuous node
        this_node->SetLaplaceSmooth(alpha);

        for (int s = 0; s < dts->num_instance; ++s) { // for each instance
            // create the vector "values" by copying the array "dts->dataset_all_vars[s]"
            vector<int> values = vector<int>(dts->dataset_all_vars[s], dts->dataset_all_vars[s] + dts->num_vars);
            // convert an instance to discrete configuration
            DiscreteConfig instance; //set<pair<int, int> >
            for (int j = 0; j < values.size(); ++j) { // for each variable of this instance
                instance.insert(pair<int, int>(j, values.at(j)));
            }
            this_node->AddInstanceOfVarVal(instance, root_idx);
            //an instance affects all the nodes in the network, because the instance here is dense.
        }
    }

    // separately handle the virtual ROOT if it exists.
    if (root_idx != -1) {
        // ROOT has no parent. suppose that ROOT has two values, each of which has the same probability.
        DiscreteConfig empty_config;
        DiscreteNode *root = dynamic_cast<DiscreteNode*>(network->FindNodePtrByIndex(root_idx));
        root->AddCount(0, empty_config, 0.5 * dts->num_instance);
        root->AddCount(1, empty_config, 0.5 * dts->num_instance);
    }


//    if (verbose > 1) {
//        cout << "==================================================" << '\n'
//             << "Each node's conditional probability table: " << endl;
//        for (const auto &id_node_ptr : network->map_idx_node_ptr) {  // For each node
//            dynamic_cast<DiscreteNode*>(id_node_ptr.second)->PrintProbabilityTable();
//        }
//    }

    timer->Stop("parameter_learning");
    timer->Print("parameter_learning");
}