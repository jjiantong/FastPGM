//
// Created by jjt on 6/10/22.
//
#include "VariableElimination.h"

VariableElimination::VariableElimination(Network *net, Dataset *dts, bool is_dense): Inference(net, dts, is_dense) {
    // TODO: if we need to specify an order, change this part...
    elimination_orderings = vector<vector<int>> (num_instances, vector<int>{});
}

double VariableElimination::EvaluateAccuracy(int num_threads) {
    cout << "==================================================" << '\n'
         << "Begin testing the trained network." << endl;

    Timer *timer = new Timer();
    // record time
    timer->Start("ve");

    // predict the labels of the test instances
    vector<int> predictions = PredictUseVEInfer(num_threads, timer);
    double accuracy = Accuracy(predictions);

    timer->Stop("ve");
    setlocale(LC_NUMERIC, "");

    cout << "==================================================";
    cout << endl; timer->Print("ve"); cout << endl;
    timer->Print("filter out"); cout << " (" << timer->time["filter out"] / timer->time["ve"] * 100 << "%)";
    timer->Print("load evidence"); cout << " (" << timer->time["load evidence"] / timer->time["ve"] * 100 << "%)";
    timer->Print("ve process"); cout << " (" << timer->time["ve process"] / timer->time["ve"] * 100 << "%)" << endl;

    SAFE_DELETE(timer);

    return accuracy;
}

void VariableElimination::InitializeCPTAndLoadEvidence(const vector<int> &left_nodes, const DiscreteConfig &evidence, int num_threads) {
    cpts.resize(left_nodes.size());
    for (int i = 0; i < left_nodes.size(); ++i) { // for each node
        // now we didn't remove irrelevant nodes, if remove, double check for this part TODO
        auto node_ptr = network->FindNodePtrByIndex(left_nodes[i]);
        if (node_ptr->is_discrete) {
            // add the factor that consists of this node and its parents
            cpts[i] = PotentialTable(dynamic_cast<DiscreteNode*>(node_ptr), this->network);
        }
    }

    // load evidence
    for (auto &e: evidence) { // for each observation of variable
        // we need the index of the the evidence and the value index
        int index = e.first;
        int value = e.second;
        if (index >= network->num_nodes) {
            cout << "!!!!" << endl;
            // todo: this is because the testing set has more features than the training set
            //  for this case, we just ignore such evidence in the current implementation
            continue;
        }

        for (int i = 0; i < left_nodes.size(); ++i) {
            if (cpts[i].related_variables.find(index) != cpts[i].related_variables.end()) {
                cpts[i].TableReduction(index, value, num_threads);
            }
        }
    }
}

/**
 * @brief: predict label given (partial or full observation) evidence
 * check "map_potentials", and the predict label is the one with maximum probability
 * @return label of the target variable
 */
int VariableElimination::PredictUseVEInfer(const DiscreteConfig &evid, int num_threads, Timer *timer, vector<int> elim_order) {
    // get the factor (marginal probability) of the target node given the evidences
    PotentialTable pt = GetMarginalProbabilitiesUseVE(evid, num_threads, timer, elim_order);

    // find the maximum probability
    // "pt" has only one related variable, which is exactly the query variable,
    // so the "max_index" exactly means which value of the query variable gets the max probability
    return ArgMax(pt.potentials);
}

/**
 * @brief: predict the labels given different evidences
 * it just repeats the function above multiple times, and print the progress at the meantime
 */
vector<int> VariableElimination::PredictUseVEInfer(int num_threads, Timer *timer) {
    cout << "Progress indicator: ";
    int every_1_of_20 = num_instances / 20; // used to print, print 20 times in total
    int progress = 0;

    vector<int> results(num_instances, 0);

    for (int i = 0; i < num_instances; ++i) {
        ++progress;

        if (progress % every_1_of_20 == 0) {
            string progress_percentage = to_string((double)progress/num_instances * 100) + "%...";
            fprintf(stdout, "%s\n", progress_percentage.c_str());
            fflush(stdout);
        }

        vector<int> elim_ord = elimination_orderings[i];
        int pred = PredictUseVEInfer(evidences.at(i), num_threads, timer, elim_ord);
        results.at(i) = pred;
    }
    return results;
}

/**
 * @brief: for inference given a target variable id and some evidences/observations.
 */
PotentialTable VariableElimination::GetMarginalProbabilitiesUseVE(const DiscreteConfig &evidence, int num_threads, Timer *timer, vector<int> elim_order) {

    timer->Start("filter out");
    // find the nodes to be removed, include barren nodes and m-separated nodes
    // filter out these nodes and obtain the left nodes
    vector<int> left_nodes = FilterOutIrrelevantNodes();
    timer->Stop("filter out");

    timer->Start("initialization");
    // "cpts" corresponds to all the nodes which are between the target node and the observation/evidence
    // because we have removed barren nodes and m-separated nodes TODO
    // and also load evidence: return a cpt list with fewer configurations
    InitializeCPTAndLoadEvidence(left_nodes, evidence, num_threads);

    if (elim_order.empty()) {
        elim_order = DefaultEliminationOrder(evidence, left_nodes);
    }
    timer->Stop("initialization");

    timer->Start("ve process");
    // compute the probability table of the target node
    PotentialTable target_node_table = SumProductVE(elim_order);
    timer->Stop("ve process");

    // renormalization
    target_node_table.Normalize();

    return target_node_table;
}

/**
 * @brief: find the nodes to be removed, including the barren nodes and m-separated nodes
 * filter out these nodes and obtain the left nodes
 * suppose Y is the set of variables observed; X is the set of variables of interest
 * barren node: a leaf which is not in X and not in Y
 * moral graph: obtained by adding an edge between each pair of parents and dropping all directions
 * m-separated node: this node and X are separated by the set Y in the moral graph
 * The implementation is based on "A simple approach to Bayesian network computations" by Zhang and Poole, 1994.
 */
vector<int> VariableElimination::FilterOutIrrelevantNodes() { // TODO
    /// find the nodes to be removed TODO
    set<int> to_be_removed;

    /// filter out these nodes and obtain the left nodes
    vector<int> left_nodes;
    int num_of_left = network->num_nodes - to_be_removed.size();
    left_nodes.reserve(num_of_left);
    for (int i = 0; i < network->num_nodes; ++i) { // for each nodes in the network
        if (to_be_removed.find(i) == to_be_removed.end()) { // if this node does not need to be removed
            left_nodes.push_back(i);
        }
    }

    return left_nodes;
}

/**
 * @brief: default elimination order is based on the reverse topological order
 * simplified by removing the evidence and the target nodes from the left nodes
 * @param left_nodes is the left node set by removing barren nodes and m-separated nodes
 * The implementation is based on "A simple approach to Bayesian network computations" by Zhang and Poole, 1994.
 */
vector<int> VariableElimination::DefaultEliminationOrder(const DiscreteConfig &evidence, const vector<int> &left_nodes) {

    // based on the reverse topological order
    vector<int> vec_default_elim_ord = network->GetReverseTopoOrd();

    // to_be_removed contains: irrelevant nodes & evidence
    set<int> to_be_removed;
    // add the target
    to_be_removed.insert(query_index);
    // add the evidence
    for (auto p: evidence) { // for each index-value pair in the evidence
        to_be_removed.insert(p.first);
    }
    // add the irrelevant nodes
    for (int i = 0; i < network->num_nodes; ++i) { // for each node in the network
        if (find(left_nodes.begin(), left_nodes.end(), i) == left_nodes.end()) { // if i is not in "left_nodes"
            to_be_removed.insert(i); // add into "to_be_removed"
        }
    }

    vector<int> vec_simplified_order;
    for (int i = 0; i < network->num_nodes; ++i) {
        int ord = vec_default_elim_ord.at(i);
        if (to_be_removed.find(ord) == to_be_removed.end()) { // if it is not removed
            vec_simplified_order.push_back(ord);
        }
    }

    return vec_simplified_order;
}

/**
 * @brief: the main variable elimination (VE) process
 * gradually eliminate variables until only one (i.e. the target node) left
 */
PotentialTable VariableElimination::SumProductVE(vector<int> elim_order) {
    // at the beginning, let table list be the reduced CPTs of all the relevant nodes
    vector<PotentialTable> table_list = cpts;

    for (int i = 0; i < elim_order.size(); ++i) { // consider each node i according to the elimination order
        // the node to be eliminated now
        int node_index = elim_order[i];

        vector<PotentialTable> tmp_table_list;
        /**
         * Move every potential table that is related to the node elim_order[i] from table list to tmp table list
         * note that this for loop does not contain ++it in the parentheses
         *      when finding the node during the traverse, the iterator "it" points to this table
         *      we use erase to delete this element from table list via iterator "it"
         *      the function erase returns an iterator pointing to the next element of the delete element
         */
        for (auto it = table_list.begin(); it != table_list.end(); /** no ++it **/) {
            if ((*it).related_variables.find(node_index) != (*it).related_variables.end()) {
                // if the table "it" is related to the node (i.e., the node to be eliminated now)
                tmp_table_list.push_back(*it);
                table_list.erase(it);
                continue;
            } else {
                ++it;
            }
        }

        // merge all the factors in tmp table list into one potential table
        while(tmp_table_list.size() > 1) {
            // every time merge two factors into one
            PotentialTable tmp1, tmp2;
            tmp1 = tmp_table_list.back(); // get the last element
            tmp_table_list.pop_back();  // remove the last element
            tmp2 = tmp_table_list.back();
            tmp_table_list.pop_back();
            // note that after popping tmp1, tmp2, they are no use. so we can just do table multiplication and replace tmp1 with the result of production
            tmp1.TableMultiplicationTwoExtension(tmp2);
            tmp_table_list.push_back(tmp1);
        }

        // eliminate the node by marginalize the final potential table over the node
        PotentialTable new_table = tmp_table_list.back();
        new_table.TableMarginalization(node_index);

        // push the new table back to the table list
        table_list.push_back(new_table);
    } // finish eliminating variables and only one variable left

    // if the "table list" contains several potential tables, we need to multiply these several tables
    // for example, the case when we have a full evidence...
    // then "table list" contains "num_nodes" factor while "elim_order" is empty
    while (table_list.size() > 1) {
        PotentialTable tmp1, tmp2;
        tmp1 = table_list.back(); // get the last element
        table_list.pop_back();  // remove the last element
        tmp2 = table_list.back();
        table_list.pop_back();
        // note that after popping tmp1, tmp2, they are no use. so we can just do table multiplication and replace tmp1 with the result of production
        tmp1.TableMultiplicationTwoExtension(tmp2);
        table_list.push_back(tmp1);
    }

    // After all the processing shown above, the only remaining factor is the factor about Y.
    return table_list.back();
}