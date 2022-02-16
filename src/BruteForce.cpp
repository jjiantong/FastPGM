//
// Created by jjt on 2021/6/16.
//

#include "BruteForce.h"

double BruteForce::EvaluateAccuracy(Dataset *dts, int num_samp, string alg, bool is_dense) {

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
        for (int j = 0; j < vec_instance.size(); ++j) {
            if (j == class_var_index) { // skip the class variable
                continue;
            }
            p.first = vec_instance.at(j).first;
            p.second = vec_instance.at(j).second.GetInt();
            e.insert(p);
        }

        if (!is_dense) {
            cout << "ERROR with brute-force exact inference! (must be dense)" << endl;
            exit(0);
        }

        e = Sparse2Dense(e, network->num_nodes, class_var_index);
        evidences.push_back(e);

        // construct the ground truth
        int g = vec_instance.at(class_var_index).second.GetInt();
//    int g = dts->dataset_all_vars[i][class_var_index];
        ground_truths.push_back(g);
    }

    // predict the labels of the test instances
    vector<int> predictions = PredictDirectly(evidences, class_var_index);

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
 * @brief: predict label given (full) evidence E and target variable id
 * @return label of the target variable
 */
int BruteForce::PredictDirectly(DiscreteConfig E, int Y_index) {
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
vector<int> BruteForce::PredictDirectly(vector<DiscreteConfig> evidences, int target_node_idx) {
    int size = evidences.size();

    cout << "Progress indicator: ";
    int every_1_of_20 = size / 20;
    int progress = 0;

    vector<int> results(size, 0);
//#pragma omp parallel for
    for (int i = 0; i < size; ++i) {
//#pragma omp critical
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
map<int, double> BruteForce::GetMarginalProbabilitiesDirectly(int target_var_index, DiscreteConfig evidence) {
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