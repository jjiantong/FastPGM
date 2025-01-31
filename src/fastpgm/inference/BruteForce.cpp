////
//// Created by jjt on 6/10/22.
////
//#include "fastpgm/inference/BruteForce.h"
//
//double BruteForce::EvaluateAccuracy(string path, int num_threads) {
//    cout << "==================================================" << '\n'
//         << "Begin testing the trained network." << endl;
//
//    Timer *timer = new Timer();
//    // record time
//    timer->Start("bf");
//
//    // predict the labels of the test instances
//    vector<int> predictions = PredictDirectly();
//    double accuracy = Accuracy(predictions);
//
//    timer->Stop("bf");
//    setlocale(LC_NUMERIC, "");
//
//    cout << "==================================================";
//    cout << endl; timer->Print("bf"); cout << endl;
//
//    SAFE_DELETE(timer);
//
//    return accuracy;
//}
//
///**
// * @brief: predict label given (full) evidence E and target variable id
// * @return label of the target variable
// */
//int BruteForce::PredictDirectly(const DiscreteConfig &E) {
//    // get map "distribution"; key: possible value of query index; value: probability of evidence E and possible value of query index
//    map<int, double> distribution = GetMarginalProbabilitiesDirectly(E);
//    // find the label which has the max probability
//    double max_prob = 0;
//    int max_index;
//    for (int i = 0; i < distribution.size(); ++i) { // traverse the potential table
//        if (distribution[i] > max_prob) {
//            max_prob = distribution[i];
//            max_index = i;
//        }
//    }
//
//    // the "max_index" exactly means which value of the query variable gets the max probability
//    return max_index;
//}
//
///**
// * @brief: predict the label of different evidences
// * it just repeats the function above multiple times, and print the progress at the meantime
// */
//vector<int> BruteForce::PredictDirectly() {
//    cout << "Progress indicator: ";
//    int every_1_of_20 = num_instances / 20;
//    int progress = 0;
//
//    vector<int> results(num_instances, 0);
//
//    for (int i = 0; i < num_instances; ++i) {
//        ++progress;
//
//        if (progress % every_1_of_20 == 0) {
//            string progress_percentage = to_string((double)progress/num_instances * 100) + "%...";
//            fprintf(stdout, "%s\n", progress_percentage.c_str());
//            fflush(stdout);
//        }
//
//        int pred = PredictDirectly(evidences.at(i));
//        results.at(i) = pred;
//    }
//    return results;
//}
//
///**
// * @brief: for inference given a target variable id and an (full) evidence/observation.
// * @param evidence: full evidence/observation (i.e. a dense instance)
// * @return map: key is the possible value of the query node;
// *              value is the probability of the query node with a specific value
// * product of factors seems to compute the joint distribution, but we renormalize it in the end, so it actually the marginal distribution
// */
//map<int, double> BruteForce::GetMarginalProbabilitiesDirectly(const DiscreteConfig &evidence) {
//    /**
//     * Example: X --> Y, and X is the target node; X = {0, 1}, Y = {0, 1}.
//     */
//    map<int, double> result;
//    DiscreteNode *query_node = (DiscreteNode*)network->FindNodePtrByIndex(query_index);
//
//    vector<int> vec_complete_instance_values;
//    vec_complete_instance_values.resize(network->num_nodes);
//    vec_complete_instance_values[query_index] = 0; // a initialization
//    for (auto evi = evidence.begin(); evi != evidence.end(); evi++) {
//        vec_complete_instance_values[evi->first] = evi->second;
//    }
//
//    // compute the probability of each possible value of the target node
//    for (int i = 0; i < query_node->GetDomainSize(); ++i) { // for each possible value of the target node (e.g. X = 0)
//        // add the ith value of the target node into "vec_complete_instance_values"
//        vec_complete_instance_values.at(query_index) = i;
//
//        // use chain rule to get the joint distribution (multiply "num_nodes" factors)
//        result[i] = 0;
//        for (int j = 0; j < network->num_nodes; ++j) { // for each node
//            DiscreteNode *node_j = (DiscreteNode*)network->FindNodePtrByIndex(j);
//            DiscreteConfig par_config = node_j->GetDiscParConfigGivenAllVarValue(vec_complete_instance_values);
//            // compute the probability of node j given its parents
//            double temp_prob = node_j->GetProbability(vec_complete_instance_values.at(j), par_config);
//            // note: use log!! so it is not "+=", it is in fact "*="..
//            result[i] += log(temp_prob);
//        }
//    }//end for each possible value of the target node
//
//    // exp and norm
//    double denominator = 0;
//    for (int i = 0; i < query_node->GetDomainSize(); ++i) {
//        result[i] = exp(result[i]); //the result[i] is computed in log scale
//        denominator += result[i];
//    }
//
//    for (int i = 0; i < query_node->GetDomainSize(); ++i) {
//        result[i] /= denominator;
//    }
//
//    return result;
//}