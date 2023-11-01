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

    int num_cores = omp_get_num_procs();
    omp_set_num_threads(num_cores);
    int max_work_per_thread = (dts->num_vars + num_cores - 1) / num_cores;
    int thread_id = omp_get_thread_num();

//    // a thread for one or more nodes
//    for (int i = max_work_per_thread * thread_id;
//         i < max_work_per_thread * (thread_id + 1) && i < dts->num_vars;
//         ++i) {
    for (int i=0; i<dts->num_vars; ++i) {
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
            this_node->AddInstanceOfVarVal(instance);//an instance affects all the nodes in the network, because the instance here is dense.
        }
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