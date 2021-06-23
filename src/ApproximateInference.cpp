//
// Created by jjt on 2021/6/16.
//

#include "ApproximateInference.h"

double ApproximateInference::EvaluateAccuracy(Dataset *dts, int num_samp, string alg, bool is_dense) {

    cout << "==================================================" << '\n'
         << "Begin testing the trained network." << endl;

    struct timeval start, end;
    double diff;
    gettimeofday(&start,NULL);

    int m = dts->num_instance;

    int class_var_index = dts->class_var_index;

    //TODO: difference, consistency
    vector<DiscreteConfig> samples;
    if (alg.compare("emm") == 0) {
        // draw "num_samp" samples
        samples = this->DrawSamplesByProbLogiSamp(num_samp);
        cout << "Finish drawing samples." << endl;
    }

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
    } else if (alg.compare("emm") == 0) {
        predictions = ApproxInferByProbLogiRejectSamp(evidences, class_var_index, samples);
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

/**
 * @brief: predice a label given a (part/full) evidence
 * @param e: evidence
 * @param node_index: target node index
 * @param num_samp: sample size
 * @return label
 */
int ApproximateInference::PredictUseLikelihoodWeighting(DiscreteConfig e, const int &node_index, const int &num_samp) {

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
vector<int> ApproximateInference::PredictUseLikelihoodWeighting(vector<DiscreteConfig> evidences,
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
vector<pair<DiscreteConfig, double>> ApproximateInference::SampleUseLikelihoodWeighting(const DiscreteConfig &evidence,
                                                                                        int num_samp) {
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
pair<DiscreteConfig, double> ApproximateInference::OneSampleUseLikelihoodWeighting(const DiscreteConfig &evidence) {
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
Factor ApproximateInference::GetMarginalProbabilitiesUseLikelihoodWeightingSamples(const vector<pair<DiscreteConfig, double>> &samples,
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
int ApproximateInference::ApproxInferByProbLogiRejectSamp(DiscreteConfig e, Node *node, vector<DiscreteConfig> &samples) {

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
int ApproximateInference::ApproxInferByProbLogiRejectSamp(DiscreteConfig e, int node_index,
                                               vector<DiscreteConfig> &samples) {
    return ApproxInferByProbLogiRejectSamp(e, network->FindNodePtrByIndex(node_index), samples);
}

/**
 *  @brief: reuse the samples for inference, while the evidence/observation may be different.
 *  it just repeats the function above multiple times, and print the progress at the meantime
 *  drawing a new set of instances is often expensive. This function makes use of reusing the pre-drawn instances.
 */
vector<int> ApproximateInference::ApproxInferByProbLogiRejectSamp(vector<DiscreteConfig> evidences,
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
vector<DiscreteConfig> ApproximateInference::DrawSamplesByProbLogiSamp(int num_samp) {
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
DiscreteConfig ApproximateInference::GenerateInstanceByProbLogicSampleNetwork() {
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


/**
 * @brief: obtain a value index given the Markov Blanket
 * @param node_ptr: target node
 * @param markov_blanket includes (i) direct parents, (ii) direct children and (iii) direct parents of direct children of the target node
 * @return value index of the target node
 */
int ApproximateInference::SampleNodeGivenMarkovBlanketReturnValIndex(Node *node_ptr, DiscreteConfig markov_blanket) {
    //use the Markov blanket to serve as the elimination order
    //int num_elim_ord = markov_blanket.size();
    vector<int> var_elim_ord;
    var_elim_ord.reserve(markov_blanket.size());
    for (auto &n_v : markov_blanket) {
        var_elim_ord.push_back(n_v.first);
    }

    // TODO: the same problem with "DiscreteNode::SampleNodeGivenParents"
    // TODO: the implementation of VE alg. in ExactInference has been changed,
    //  we need to change the implementation here, and move to maybe Inference?
    //obtain the marginal probabilities of the target node
    Factor f = network->VarElimInferReturnPossib(markov_blanket, node_ptr, var_elim_ord);

    //use the marginal probabilities of the target node for sampling
    vector<int> weights;
    for (int i = 0; i < dynamic_cast<DiscreteNode*>(node_ptr)->GetDomainSize(); ++i) {//for each possible value of the target node
        DiscreteConfig temp;
        temp.insert(pair<int,int>(node_ptr->GetNodeIndex(),
                                  dynamic_cast<DiscreteNode*>(node_ptr)->vec_potential_vals.at(i)));
        weights.push_back(f.map_potentials[temp]*10000);//the marginal probability is converted into int
    }

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    default_random_engine rand_gen(seed);
    discrete_distribution<int> this_distribution(weights.begin(),weights.end());
    return this_distribution(rand_gen); // randomly pick an index and return
}

/**
 * @brief: draw multiple instances
 * @param num_samp: the number of instances to draw
 * @param num_burn_in: a terminology in MCMC and Gibbs sampling; the number of instances drawn at the beginning to be ignored
 * @return a set of instances
 */
vector<DiscreteConfig> ApproximateInference::DrawSamplesByGibbsSamp(int num_samp, int num_burn_in) {

    vector<DiscreteConfig> samples;
    samples.reserve(num_samp);

    // randomly pick one sample
    DiscreteConfig single_sample = this->GenerateInstanceByProbLogicSampleNetwork();

    auto it_idx_node = network->map_idx_node_ptr.begin(); // begin at the first node

    // Need burning in.
//  #pragma omp parallel for
    for (int i = 1; i < num_burn_in + num_samp; ++i) {//draw instances

        Node *node_ptr = (*(it_idx_node++)).second;
        if (it_idx_node == network->map_idx_node_ptr.end()) {
            it_idx_node = network->map_idx_node_ptr.begin();
        }

        set<int> markov_blanket_node_index = network->GetMarkovBlanketIndexesOfNode(node_ptr);

        // construct the markov blanket from the picked "single_sample"
        // i.e., filter out the variable-values that are not in the markov blanket
        DiscreteConfig markov_blanket;
        for (auto &p : single_sample) { // for each variable-value of the picked "single_sample"
            // check if the variable is in the Markov Blanket
            if (markov_blanket_node_index.find(p.first) != markov_blanket_node_index.end()) {
                markov_blanket.insert(p);
            }
        }

        // obtain a value of a variable given the Markov Blanket
        int value_index = SampleNodeGivenMarkovBlanketReturnValIndex(node_ptr, markov_blanket);

        // replace the value of the previous instance with the new value
        for (auto p : single_sample) {
            if (p.first == node_ptr->GetNodeIndex()) {
                single_sample.erase(p);
                p.second = dynamic_cast<DiscreteNode*>(node_ptr)->vec_potential_vals.at(value_index);
                single_sample.insert(p);
                break;
            }
        }

        // After burning in, we can store the samples now.
#pragma omp critical
        { if (i >= num_burn_in) { samples.push_back(single_sample); } }
    }

    return samples;
}