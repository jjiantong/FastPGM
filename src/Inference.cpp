//
// Created by jjt on 2021/6/15.
//

#include "Inference.h"

/**
 * @brief: convert sparse to dense by filling zero
 */
DiscreteConfig Inference::Sparse2Dense(DiscreteConfig evidence) {
    set<int> existing_index;
    for (auto it = evidence.begin(); it != evidence.end(); it++) {
        existing_index.insert(it->first);
    }
    for (int i = 1; i < network->num_nodes; i++) { // for all nodes except for the target node
        // if node index i is not in existing evidence, filling 0
        if (existing_index.find(i) == existing_index.end()) {
            pair<int, int> p;
            p.first = i;
            p.second = 0;
            evidence.insert(p);
        }
    }

    DiscreteConfig dense_instance = evidence;
    return dense_instance;
}

/**
 * @brief: predict label given (full) evidence E and target variable id
 * @return label of the target variable
 */
int Inference::PredictDirectly(DiscreteConfig E, int Y_index) {
    // get map "distribution"; key: possible value of Y_index; value: probability of evidence E and possible value of Y_index
    map<int, double> distribution = GetMarginalProbabilitiesDirectly(Y_index, E);
    // find the label which has the max probability
    int label_index = ArgMax(distribution);

    // convert index of the target value to label
    DiscreteNode* tempNode = ((DiscreteNode*)network->FindNodePtrByIndex(Y_index));
    int label_predict = tempNode->vec_potential_vals.at(label_index);
    return label_predict;
}

/**
 * @brief: predict the label of different evidences
 * it just repeats the function above multiple times, and print the progress at the meantime
 */
vector<int> Inference::PredictDirectly(vector<DiscreteConfig> evidences, int target_node_idx) {
    int size = evidences.size();

    cout << "Progress indicator: ";
    int every_1_of_20 = size / 20;
    int progress = 0;

    vector<int> results(size, 0);
#pragma omp parallel for
    for (int i = 0; i < size; ++i) {
#pragma omp critical
        { ++progress; }

        if (progress % every_1_of_20 == 0) {
            string progress_percentage = to_string((double)progress/size * 100) + "%...\n";
            fprintf(stdout, "%s\n", progress_percentage.c_str());
            fflush(stdout);
        }

        int pred = PredictDirectly(evidences.at(i), target_node_idx);
        results.at(i) = pred;
    }
    return results;
}

/**
 * @brief: for inference given a target variable id and an (full) evidence/observation.
 * @param evidence: full evidence/observation (i.e. a dense instance)
 * @return map: key is the possible value of the target node;
 *              value is the probability of the target node with a specific value
 * product of factors seems to compute the joint distribution, but we renormalize it in the end, so it actually the marginal distribution
 */
map<int, double> Inference::GetMarginalProbabilitiesDirectly(int target_var_index, DiscreteConfig evidence) {
    if (!network->pure_discrete) {
        fprintf(stderr, "Function [%s] only works on pure discrete networks!", __FUNCTION__);
        exit(1);
    }
    /**
     * Example: X --> Y, and X is the target node; X = {0, 1}, Y={0,1}.
     */

    map<int, double> result;

    DiscreteNode *target_node = (DiscreteNode*)network->FindNodePtrByIndex(target_var_index);
    vector<int> vec_complete_instance_values;
    vec_complete_instance_values.push_back(0);
    for (auto evi = evidence.begin(); evi != evidence.end(); evi++) {
        vec_complete_instance_values.push_back(evi->second);
    }

    // compute the probability of each possible value of the target node
    for (int i = 0; i < target_node->GetDomainSize(); ++i) { // for each possible value of the target node (e.g. X=0)
        // add the ith value of the target node into "vec_complete_instance_values"
        vec_complete_instance_values.at(target_var_index) = target_node->vec_potential_vals.at(i);

        // use chain rule to get the joint distribution (multiply "num_nodes" factors)
        result[i] = 0;
        for (int j = 0; j < network->num_nodes; ++j) { // for each node
            DiscreteNode *node_j = (DiscreteNode*)network->FindNodePtrByIndex(j);
            DiscreteConfig par_config = node_j->GetDiscParConfigGivenAllVarValue(vec_complete_instance_values);
            // compute the probability of node j given its parents
            double temp_prob = node_j->GetProbability(vec_complete_instance_values.at(j), par_config);
            // note: use log!! so it is not "+=", it is in fact "*="..
            result[i] += log(temp_prob);
        }
    }//end for each possible value of the target node

    for (int i = 0; i < target_node->GetDomainSize(); ++i) {
        result[i] = exp(result[i]); //the result[i] is computed in log scale
    }
    result = Normalize(result);
    return result;
}

/**
 * @brief: predict label given (partial or full observation) evidence
 * check "map_potentials", and the predict label is the one with maximum probability
 * @return label of the target variable
 */
int Inference::PredictUseVEInfer(DiscreteConfig evid, int target_node_idx, vector<int> elim_order) {

    // get the factor (marginal probability) of the target node given the evidences
    Factor F = GetMarginalProbabilitiesUseVE(target_node_idx, evid, elim_order);

    // find the configuration with the maximum probability TODO: function ArgMax for Factor
    double max_prob = 0;
    DiscreteConfig comb_predict;
    for (auto &comb : F.set_disc_config) { // for each configuration of the related variables
        if (F.map_potentials[comb] > max_prob) {
            max_prob = F.map_potentials[comb];
            comb_predict = comb;
        }
    }
    int label_predict = comb_predict.begin()->second;
    return label_predict;
}

/**
 * @brief: predict the labels given different evidences
 * it just repeats the function above multiple times, and print the progress at the meantime
 * @param elim_orders: elimination order which may be different given different evidences due to the simplification of elimination order
 */
vector<int> Inference::PredictUseVEInfer(vector<DiscreteConfig> evidences, int target_node_idx,
                                         vector<vector<int>> elim_orders) {
    int size = evidences.size();

    cout << "Progress indicator: ";
    int every_1_of_20 = size / 20; // used to print, print 20 times in total
    int progress = 0;

    if (elim_orders.empty()) {
        // Vector of size "size". Each element is an empty vector.
        elim_orders = vector<vector<int>> (size, vector<int>{});
    }

    vector<int> results(size, 0);
#pragma omp parallel for
    for (int i = 0; i < size; ++i) {
#pragma omp critical
        { ++progress; }

        if (progress % every_1_of_20 == 0) {
            string progress_percentage = to_string((double)progress/size * 100) + "%...\n";
            fprintf(stdout, "%s\n", progress_percentage.c_str());
            fflush(stdout);
        }

        DiscreteConfig evidence = evidences.at(i);
        vector<int> elim_ord = elim_orders.at(i);
        int pred = PredictUseVEInfer(evidence, target_node_idx, elim_ord);
        results.at(i) = pred;
    }
    return results;
}

/**
 * @brief: for inference given a target variable id and some evidences/observations.
 */
Factor Inference::GetMarginalProbabilitiesUseVE(int target_var_index, DiscreteConfig evidence,
                                                vector<int> elim_order) {

    // find the nodes to be removed, include barren nodes and m-separated nodes
    // filter out these nodes and obtain the left nodes
    vector<int> left_nodes = FilterOutIrrelevantNodes();

    Node *n = nullptr;
    // "factorsList" corresponds to all the nodes which are between the target node and the observation/evidence
    // because we have removed barren nodes and m-separated nodes
    vector<Factor> factor_list = network->ConstructFactors(left_nodes, n);

    set<int> all_related_vars;
    all_related_vars.insert(target_var_index);
    for (int i = 0; i < left_nodes.size(); ++i) {
        all_related_vars.insert(left_nodes.at(i));
    }
    // load evidence function below returns a factorsList with fewer configurations.
    network->LoadEvidenceIntoFactors(&factor_list, evidence, all_related_vars);

    if (elim_order.empty()) {
        // call "ChowLiuTree::SimplifyDefaultElimOrd"; "elim_order" is the reverse topological order removing barren nodes and m-separated nodes
        elim_order = network->SimplifyDefaultElimOrd2(evidence, left_nodes); //TODO
    }

    // compute the probability table of the target node
    Factor target_node_factor = SumProductVarElim(factor_list, elim_order);

    // renormalization
    target_node_factor.Normalize();

    return target_node_factor;
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
vector<int> Inference::FilterOutIrrelevantNodes() { // TODO
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
 * @brief: the main variable elimination (VE) process
 * gradually eliminate variables until only one (i.e. the target node) left
 */
Factor Inference::SumProductVarElim(vector<Factor> factor_list, vector<int> elim_order) {
    for (int i = 0; i < elim_order.size(); ++i) { // consider each node i according to the elimination order
        vector<Factor> temp_factor_list;
        Node* nodePtr = network->FindNodePtrByIndex(elim_order.at(i));

        // Move every factor that is related to the node elim_order[i] from factors_list to tempFactorsList.
        /*
         * Note: This for loop does not contain "++it" in the parentheses.
         *      When finding "elim_order[i]" during the traverse, the iterator "it" points to this vector.
         *      We use "erase" to delete this element from "factors_list" via iterator "it";
         *      the function "erase" returns an iterator pointing to the next element of the delete element.
         */
        for (auto it = factor_list.begin(); it != factor_list.end(); /* no ++it */) {
            // if the factor "it" is related to the node "elim_order[i]" (i.e., the node to be eliminated now)
            if ((*it).related_variables.find(nodePtr->GetNodeIndex()) != (*it).related_variables.end()) {
                temp_factor_list.push_back(*it);
                factor_list.erase(it);
                continue;
            }
            else {
                ++it;
            }
        }

        // merge all the factors in tempFactorsList into one factor
        while(temp_factor_list.size() > 1) {
            // every time merge two factors into one
            Factor temp1, temp2, product;
            temp1 = temp_factor_list.back(); // get the last element
            temp_factor_list.pop_back();  // remove the last element
            temp2 = temp_factor_list.back();
            temp_factor_list.pop_back();

            product = temp1.MultiplyWithFactor(temp2);
            temp_factor_list.push_back(product);
        }

        // eliminate variable "nodePtr" by summation of the factor "tempFactorsList.back()" over "nodePtr"
        Factor newFactor = temp_factor_list.back().SumOverVar(dynamic_cast<DiscreteNode*>(nodePtr));
        factor_list.push_back(newFactor);
    } // finish eliminating variables and only one variable left

    // if the "factor_list" contains several factors, we need to multiply these several factors
    // for example, the case when we have a full evidence...
    // then "factor_list" contains "num_nodes" factor while "elim_order" is empty
    while (factor_list.size() > 1) {
        Factor temp1, temp2, product;
        temp1 = factor_list.back(); // get the last element
        factor_list.pop_back();  // remove the last element
        temp2 = factor_list.back();
        factor_list.pop_back();

        product = temp1.MultiplyWithFactor(temp2);
        factor_list.push_back(product);
    }

    // After all the processing shown above, the only remaining factor is the factor about Y.
    return factor_list.back();
}

/**
 * @brief: predice a label given a (part/full) evidence
 * @param e: evidence
 * @param node_index: target node index
 * @param num_samp: sample size
 * @return label
 */
int Inference::PredictUseLikelihoodWeighting(DiscreteConfig e, const int &node_index, const int &num_samp) {

    vector<pair<DiscreteConfig, double>> samples_weight = this->SampleUseLikelihoodWeighting(e, num_samp);
    Factor f = GetMarginalProbabilitiesUseLikelihoodWeightingSamples(samples_weight, node_index);

    // Find the argmax.
    DiscreteConfig c;
    double max = -1;
    for (const auto &kv : f.map_potentials) {
        if (kv.second > max) { // find the max probability
            c = kv.first; // mark the configuration
            max = kv.second; // mark the probability
        }
    }
    return (*c.begin()).second; // return one value of the target node that has the largest probability
}

/**
 * @brief: approximate inference given different evidences
 * it just repeats the function above multiple times, and print the progress at the meantime
 * @return a vector of labels for each evidence
 */
vector<int> Inference::PredictUseLikelihoodWeighting(vector<DiscreteConfig> evidences,
                                                     const int &target_node_idx, const int &num_samp) {
    int size = evidences.size();

    cout << "Progress indicator: ";
    int every_1_of_20 = size / 20;
    int progress = 0;

    vector<int> results(size, 0);
#pragma omp parallel for
    for (int i = 0; i < size; ++i) {
#pragma omp critical
        { ++progress; }

        if (progress % every_1_of_20 == 0) {
            string progress_percentage = to_string((double)progress/size * 100) + "%...\n";
            fprintf(stdout, "%s\n", progress_percentage.c_str());
            fflush(stdout);
        }

        int pred = PredictUseLikelihoodWeighting(evidences.at(i), target_node_idx, num_samp);
        results.at(i) = pred;
    }
    return results;
}

/**
 * @brief: draw multiple instances for approximate inference
 * it repeats the function above "num_samp" times to select "num_samp" samples
 */
vector<pair<DiscreteConfig, double>> Inference::SampleUseLikelihoodWeighting(const DiscreteConfig &evidence, int num_samp) {
    vector<pair<DiscreteConfig, double>> results;
#pragma omp parallel for
    for (int i=0; i<num_samp; ++i) {
        auto samp = OneSampleUseLikelihoodWeighting(evidence);
#pragma omp critical
        { results.push_back(samp); }
    }
    return results;
}

/**
 * @brief: for approximate inference; this function generate an instance using the network.
 * @param evidence: this parameter is optional.
 * @return an instance and a weight based on likelihood
 */
// TODO: can be merge with ""GenerateInstanceByProbLogicSampleNetwork""?
pair<DiscreteConfig, double> Inference::OneSampleUseLikelihoodWeighting(const DiscreteConfig &evidence) {
    DiscreteConfig instance;
    double weight = 1;
    // SHOULD NOT use OpenMP, because must draw samples in the topological ordering.
    for (const auto &index : network->GetTopoOrd()) {  // For each node.
        Node *n_p = network->FindNodePtrByIndex(index);
        bool observed = false;
        for (const auto &var_val : evidence) {  // Find if this variable node is in evidence
            if (index == var_val.first) { // if "n_p" is in "evidence"
                observed = true;
                // Set the sample value to be the same as the evidence.
                instance.insert(pair<int, int>(index, var_val.second));

                // todo: check the correctness of this implementation for the 4 lines below
                // TODO: check "weight"
                // Update the weight.
                DiscreteConfig parents_config = dynamic_cast<DiscreteNode*>(n_p)->GetDiscParConfigGivenAllVarValue(instance);
                double cond_prob = dynamic_cast<DiscreteNode*>(n_p)->GetProbability(var_val.second, parents_config);
                weight *= cond_prob;
                break;
            }
        }
        if (!observed) { // if "n_p" is not in "evidence"
            int drawn_value = dynamic_cast<DiscreteNode*>(n_p)->SampleNodeGivenParents(instance);   // todo: Consider continuous nodes
            instance.insert(pair<int,int>(index, drawn_value));
        }
    }
    return pair<DiscreteConfig, double>(instance, weight);
}

/**
 * @brief: perform inference based on the drawn sample with weights.
 * @param node_index: target node which needs to compute marginal probabilities
 */
Factor Inference::GetMarginalProbabilitiesUseLikelihoodWeightingSamples(const vector<pair<DiscreteConfig, double>> &samples,
                                                                        const int &node_index) {
    map<int, double> value_weight;
    DiscreteNode *target_node = dynamic_cast<DiscreteNode*>(network->FindNodePtrByIndex(node_index));

    // Initialize the map.
    for (int i=0; i<target_node->GetDomainSize(); ++i) {
        value_weight[target_node->vec_potential_vals.at(i)] = 0;
    }

    // Calculate the sum of weight for each value. Un-normalized.
    for (const auto &samp : samples) { // for each sample
        for (const auto &feature_value : samp.first) { // for each variable-value in the sample
            if (node_index == feature_value.first) { // find the target variable
                // accumulate the weight of each possible value of the target value
                value_weight[feature_value.second] += samp.second;
                break;
            }
        }
    }

    // Normalization. TODO: it seems to have another function
    double denominator = 0;
    for (const auto &kv : value_weight) {
        denominator += kv.second;
    }
    for (auto &kv : value_weight) {
        kv.second /= denominator;
    }

    // Construct a factor to return
    // 1. related variables; is the target node
    set<int> rv;
    rv.insert(node_index);
    // 2. all the configurations of the related variables
    set<DiscreteConfig> sc;
    for (int i=0; i<target_node->GetDomainSize(); ++i) {
        DiscreteConfig c;
        c.insert(pair<int, int>(node_index, target_node->vec_potential_vals.at(i)));
        sc.insert(c);
    }
    // 3. map<DiscreteConfig, double>; the weight/potential of each discrete config
    map<DiscreteConfig, double> mp;
    for (const auto &c : sc) {
        int value = (*c.begin()).second;
        mp[c] = value_weight[value];
    }
    Factor f(rv, sc, mp);
    return f;
}


/**
 * @brief: given a set of instances, evidence; the same sample with different evidence can lead to different probabilities
 * @param e: evidence
 * @param node: target node
 * @param samples: input
 * @return label of the target node
 */
int Inference::ApproxInferByProbLogiRejectSamp(DiscreteConfig e, Node *node, vector<DiscreteConfig> &samples) {

    // obtain the possible discrete configuration of the target node
    // it just converts "vec_potential_vals" (vector<int>) into "possb_value" (set<pair<int, int> >)
    // where the first "int" is always the node id of the target node
    DiscreteConfig possb_values;
    for (int i = 0; i < dynamic_cast<DiscreteNode*>(node)->GetDomainSize(); ++i) {
        possb_values.insert(pair<int,int>(node->GetNodeIndex(),
                                          dynamic_cast<DiscreteNode*>(node)->vec_potential_vals.at(i)));
    }

    // compute the statistics of the instances which are consistent to the target node.
    int num_possible_value = possb_values.size();
    assert(dynamic_cast<DiscreteNode*>(node)->GetDomainSize() == num_possible_value);
    int *count_each_value = new int[num_possible_value](); // 1d array of size "num_possible_value"
    int num_valid_sample = 0;
    for (const auto &samp : samples) { // for each sample
        if(!Conflict(&e, &samp)) { // if the sample is consistent to the evidence
            ++num_valid_sample; //  it is a valid sample
            // for each possible value of the target node, check which one is consistent to the sample
            for (const auto &pv : possb_values) {
                if (samp.find(pv) != samp.end()) { // find the value of the target node of the sample
                    ++count_each_value[pv.second]; // update the counter of the value that the sample has
                    break;
                }
            }
        }
    }

    // If there is no valid sample, just take a random guess.
    if (num_valid_sample==0) {
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        default_random_engine rand_gen(seed);
        uniform_int_distribution<int> this_distribution(0,dynamic_cast<DiscreteNode*>(node)->GetDomainSize()-1);
        return dynamic_cast<DiscreteNode*>(node)->vec_potential_vals.at(this_distribution(rand_gen));
    }

    // Find the argmax.; the label which appears most frequently in the input set of instances. // TODO: move, like other functions
    int label_index_predict = -1;
    int max_occurred = 0;
    for (int i=0; i<dynamic_cast<DiscreteNode*>(node)->GetDomainSize(); ++i) {
        if (label_index_predict == -1 || count_each_value[i] > max_occurred) {
            label_index_predict = i; // mark the index
            max_occurred = count_each_value[i]; // mark the counter
        }
    }

    // Return the predicted label instead of the index.
    return dynamic_cast<DiscreteNode*>(node)->vec_potential_vals.at(label_index_predict);
}

//refer to the same function with node_ptr as input
int Inference::ApproxInferByProbLogiRejectSamp(DiscreteConfig e, int node_index,
                                               vector<DiscreteConfig> &samples) {
    return ApproxInferByProbLogiRejectSamp(e, network->FindNodePtrByIndex(node_index), samples);
}

/**
 *  @brief: reuse the samples for inference, while the evidence/observation may be different.
 *  it just repeats the function above multiple times, and print the progress at the meantime
 *  drawing a new set of instances is often expensive. This function makes use of reusing the pre-drawn instances.
 */
vector<int> Inference::ApproxInferByProbLogiRejectSamp(vector<DiscreteConfig> evidences,
                                                       int node_idx, vector<DiscreteConfig> &samples) {
    int size = evidences.size();

    cout << "Progress indicator: ";
    int every_1_of_20 = size / 20;
    int progress = 0;

    vector<int> results(size, 0);
#pragma omp parallel for
    for (int i = 0; i < size; ++i) {
#pragma omp critical
        { ++progress; }

        if (progress % every_1_of_20 == 0) {
            string progress_percentage = to_string((double)progress/size * 100) + "%...\n";
            fprintf(stdout, "%s\n", progress_percentage.c_str());
            fflush(stdout);
        }

        int pred = ApproxInferByProbLogiRejectSamp(evidences.at(i), node_idx, samples);
        results.at(i) = pred;
    }
    return results;
}


/**
 * @brief: draw multiple instances
 * it repeats the function above "num_samp" times to select "num_samp" samples
 */
vector<DiscreteConfig> Inference::DrawSamplesByProbLogiSamp(int num_samp) {
    vector<DiscreteConfig> samples;
    samples.reserve(num_samp);
#pragma omp parallel for
    for (int i = 0; i < num_samp; ++i) {
        DiscreteConfig samp = this->GenerateInstanceByProbLogicSampleNetwork();
#pragma omp critical
        { samples.push_back(samp); }
    }
    return samples;
}

/**
 * @brief: for approximate inference; this function generate an instance using the network
 * @return an instance
 */
DiscreteConfig Inference::GenerateInstanceByProbLogicSampleNetwork() {
    // Probabilistic logic sampling is a method
    // proposed by Max Henrion at 1988: "Propagating uncertainty in Bayesian networks by probabilistic logic sampling" TODO: double-check

    DiscreteConfig instance;
    // Cannot use OpenMP, because must draw samples in the topological ordering.
    // TODO: if we directly use "value", or use index to randomly pick one possible value of "n_p",
    // TODO: then we do not need to use the topological ordering...
    for (const auto &index : network->GetTopoOrd()) { // for each node following the topological ordering
        Node *n_p = network->FindNodePtrByIndex(index);
        int drawn_value = dynamic_cast<DiscreteNode*>(n_p)->SampleNodeGivenParents(instance); // todo: support continuous nodes
        instance.insert(pair<int,int>(index, drawn_value));
    }
    return instance;
}


double Inference::EvaluateExactInferenceAccuracy(Dataset *dts, string alg, bool is_dense) {

    cout << "==================================================" << '\n'
         << "Begin testing the trained network." << endl;

    struct timeval start, end;
    double diff;
    gettimeofday(&start,NULL);

    int m = dts->num_instance;

    int class_var_index = dts->class_var_index;

    // construct the test data set with labels
    vector<int> ground_truths;
    vector<DiscreteConfig> evidences;
    evidences.reserve(m);
    ground_truths.reserve(m);

    for (int i = 0; i < m; ++i) { // for each instance in the data set
        vector<VarVal> vec_instance = dts->vector_dataset_all_vars.at(i);

        // construct a test data set by removing the class variable
        DiscreteConfig e;
        pair<int, int> p;
        for (int j = 1; j < vec_instance.size(); ++j) { // skip the class variable (which is at the beginning of the vector)
            p.first = vec_instance.at(j).first;
            p.second = vec_instance.at(j).second.GetInt();
            e.insert(p);
        }
        if (is_dense) {
            e = Sparse2Dense(e);
        }
        evidences.push_back(e);

        // construct the ground truth
        int g = vec_instance.at(0).second.GetInt();
//    int g = dts->dataset_all_vars[i][class_var_index];
        ground_truths.push_back(g);
    }

    // predict the labels of the test instances
    vector<int> predictions;
    if (alg.compare("direct") == 0) {
        predictions = PredictDirectly(evidences, class_var_index);
    } else if (alg.compare("ve") == 0) {
        predictions = PredictUseVEInfer(evidences, class_var_index);
    } else {
        cout << "ERROR with exact inference algorithm!" << endl;
        exit(0);
    }
    double accuracy = Accuracy(ground_truths, predictions);
    cout << '\n' << "Accuracy: " << accuracy << endl;


    gettimeofday(&end,NULL);
    diff = (end.tv_sec-start.tv_sec) + ((double)(end.tv_usec-start.tv_usec))/1.0E6;
    setlocale(LC_NUMERIC, "");
    cout << "==================================================" << '\n'
         << "The time spent to test the accuracy is " << diff << " seconds" << endl;

    return accuracy;
}

double Inference::EvaluateApproximateInferenceAccuracy(Dataset *dts, int num_samp, string alg, bool is_dense) {

    cout << "==================================================" << '\n'
         << "Begin testing the trained network." << endl;

    struct timeval start, end;
    double diff;
    gettimeofday(&start,NULL);

    int m = dts->num_instance;

    int class_var_index = dts->class_var_index;

//    // draw "num_samp" samples TODO: difference
//    vector<DiscreteConfig> samples = this->DrawSamplesByProbLogiSamp(num_samp);
//    cout << "Finish drawing samples." << endl;

    // construct the test data set with labels
    vector<int> ground_truths;
    vector<DiscreteConfig> evidences;
    evidences.reserve(m);
    ground_truths.reserve(m);

    for (int i = 0; i < m; ++i) { // for each instance in the data set
        vector<VarVal> vec_instance = dts->vector_dataset_all_vars.at(i);

        // construct a test data set by removing the class variable
        DiscreteConfig e;
        pair<int, int> p;
        for (int j = 1; j < vec_instance.size(); ++j) { // skip the class variable (which is at the beginning of the vector)
            p.first = vec_instance.at(j).first;
            p.second = vec_instance.at(j).second.GetInt();
            e.insert(p);
        }
        if (is_dense) {
            e = Sparse2Dense(e);
        }
        evidences.push_back(e);

        // construct the ground truth
        int g = vec_instance.at(0).second.GetInt();
//    int g = dts->dataset_all_vars[i][class_var_index];
        ground_truths.push_back(g);
    }

    // predict the labels of the test instances
    vector<int> predictions;
    if (alg.compare("likelihood") == 0) {
        predictions = PredictUseLikelihoodWeighting(evidences, class_var_index, num_samp);
    } else {
        cout << "ERROR with approximate inference algorithm!" << endl;
        exit(0);
    }
    double accuracy = Accuracy(ground_truths, predictions);
    cout << '\n' << "Accuracy: " << accuracy << endl;


    gettimeofday(&end,NULL);
    diff = (end.tv_sec-start.tv_sec) + ((double)(end.tv_usec-start.tv_usec))/1.0E6;
    setlocale(LC_NUMERIC, "");
    cout << "==================================================" << '\n'
         << "The time spent to test the accuracy is " << diff << " seconds" << endl;

    return accuracy;
}

// TODO: poor performance - check the algorithm.
//  then merge into EvaluateApproximateInferenceAccuracy - consistency especially for sampling part
double Inference::EvaluateApproxInferAccuracy(Dataset *dts, int num_samp, bool is_dense) {

    cout << "==================================================" << '\n'
         << "Begin testing the trained network." << endl;

    struct timeval start, end;
    double diff;
    gettimeofday(&start,NULL);

    int m = dts->num_instance;

    int class_var_index = dts->class_var_index;

    // draw "num_samp" samples TODO: difference, consistency
    vector<DiscreteConfig> samples = this->DrawSamplesByProbLogiSamp(num_samp);
    cout << "Finish drawing samples." << endl;

    // construct the test data set with labels
    vector<int> ground_truths;
    vector<DiscreteConfig> evidences;
    evidences.reserve(m);
    ground_truths.reserve(m);

    for (int i = 0; i < m; ++i) { // for each instance in the data set
        vector<VarVal> vec_instance = dts->vector_dataset_all_vars.at(i);

        // construct a test data set by removing the class variable
        DiscreteConfig e;
        pair<int, int> p;
        for (int j = 1; j < vec_instance.size(); ++j) { // skip the class variable (which is at the beginning of the vector)
            p.first = vec_instance.at(j).first;
            p.second = vec_instance.at(j).second.GetInt();
            e.insert(p);
        }
        if (is_dense) {
            e = Sparse2Dense(e);
        }
        evidences.push_back(e);

        // construct the ground truth
        int g = vec_instance.at(0).second.GetInt();
//    int g = dts->dataset_all_vars[i][class_var_index];
        ground_truths.push_back(g);
    }

    // predict the labels of the test instances TODO: difference in function
    vector<int> predictions = ApproxInferByProbLogiRejectSamp(evidences, class_var_index, samples);
    double accuracy = Accuracy(ground_truths, predictions);
    cout << '\n' << "Accuracy: " << accuracy << endl;

    gettimeofday(&end,NULL);
    diff = (end.tv_sec-start.tv_sec) + ((double)(end.tv_usec-start.tv_usec))/1.0E6;
    setlocale(LC_NUMERIC, "");
    cout << "==================================================" << '\n'
         << "The time spent to test the accuracy is " << diff << " seconds" << endl;

    return accuracy;
}

double Inference::Accuracy(vector<int> ground_truth, vector<int> predictions) {
    int size = ground_truth.size(),
            num_of_correct = 0,
            num_of_wrong = 0;
    for (int i = 0; i < size; ++i) {
        int g = ground_truth.at(i),
                p = predictions.at(i);
        if (g == p) {
            ++num_of_correct;
        } else {
            ++num_of_wrong;
        }
    }
    double accuracy = num_of_correct / (double)(num_of_correct+num_of_wrong);
    return accuracy;
}
