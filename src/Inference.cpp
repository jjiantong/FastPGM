//
// Created by jjt on 2021/6/15.
//
#include "Inference.h"

/**
 * constructor of Inference
 * get evidences, query index and ground truths from the testing set
 * @param net the network used for inference
 * @param dts the testing set
 * @param is_dense whether we need to fill zero for the evidences
 */
Inference::Inference(bool classification, Network *net, Dataset *dts, bool is_dense):
            classification_mode(classification), network(net), num_instances(dts->num_instance),
            query_index(dts->class_var_index) {
    // TODO: double check this function if the dataset for inference problem is changed. e.g., we don't need \
    //  `query_index` or `ground_truth` for inference mode.

    evidences.reserve(num_instances);
    ground_truths.reserve(num_instances);

    for (int i = 0; i < num_instances; ++i) {  // For each sample in test set
        vector<VarVal> vec_instance = dts->vector_dataset_all_vars.at(i);

        // construct an evidence by removing the class variable
        DiscreteConfig e;
        pair<int, int> p;
        for (int j = 0; j < vec_instance.size(); ++j) {
            if (j == query_index) { // skip the class variable
                continue;
            }
            p.first = vec_instance.at(j).first;
            p.second = vec_instance.at(j).second.GetInt();
            e.insert(p);
        }

        if (is_dense) {
            e = Sparse2Dense(e, network->num_nodes);
        }
        evidences.push_back(e);

        // construct the ground truth
        int g = vec_instance.at(query_index).second.GetInt();
        if (classification_mode) {
            ground_truths.push_back(g);
        }
    }
}

Inference::Inference(Network *net): network(net) {}

double Inference::EvaluateAccuracy(string path, int num_threads) {
    if (!classification_mode) {
        LoadGroundTruthProbabilityTable(path);
    }

    cout << "==================================================" << '\n'
         << "Begin testing the trained network." << endl;
    vector<int> predictions = Predict(num_threads); // pure virtual function

    if (classification_mode) {
        double accuracy = Accuracy(predictions);
        return accuracy;
    } else {
        return -1;
    }
}

double Inference::Accuracy(vector<int> predictions) {
    int size = ground_truths.size(),
            num_of_correct = 0,
            num_of_wrong = 0;
    for (int i = 0; i < size; ++i) {
        int g = ground_truths.at(i),
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

/**
 * @brief: convert sparse to dense by filling zero
 */
DiscreteConfig Inference::Sparse2Dense(DiscreteConfig evidence, int num_nodes) {
    set<int> existing_index;
    for (auto &e: evidence) {
        existing_index.insert(e.first);
    }

    DiscreteConfig dense_instance = evidence;
    for (int i = 0; i < num_nodes; i++) {
        if (i == query_index) { // skip the class variable
            continue;
        }
        // if node index i is not in existing evidence, filling 0
        if (existing_index.find(i) == existing_index.end()) {
            pair<int, int> p;
            p.first = i;
            p.second = 0;
            dense_instance.insert(p);
        }
    }

    return dense_instance;
}

/**
 * find the index i that leads to the max array[i] over all elements in array
 */
int Inference::ArgMax(const vector<double> &array) {
    double max_prob = 0;
    int max_index;
    for (int i = 0; i < array.size(); ++i) { // traverse the potential table
        if (array[i] > max_prob) {
            max_prob = array[i];
            max_index = i;
        }
    }
    return max_index;
}

/**
 * load the ground truth probability table for each node
 * they are computed by JT and saved in a file
 */
void Inference::LoadGroundTruthProbabilityTable(string file_path) {
    ground_truth_probability_tables.resize(num_instances);
    for (int i = 0; i < num_instances; ++i) {
        ground_truth_probability_tables[i].resize(network->num_nodes);
        for (int j = 0; j < network->num_nodes; ++j) {
            int dim = dynamic_cast<DiscreteNode*>(network->FindNodePtrByIndex(j))->GetDomainSize();
            ground_truth_probability_tables[i][j].resize(dim);
        }
    }

    ifstream in_file;
    in_file.open(file_path);
    if (!in_file.is_open()) {
        fprintf(stderr, "Error in function %s!", __FUNCTION__);
        fprintf(stderr, "Unable to open file %s!", file_path.c_str());
        exit(1);
    }

    cout << "Data file opened. Begin to load ground truth probability tables computed by JT. " << endl;

    string line;
    for (int i = 0; i < num_instances; ++i) { // for each test case
        for (int j = 0; j < network->num_nodes; ++j) { // for each node
            // the file has #instances * #nodes lines in total
            getline(in_file, line);
            line = TrimRight(line);
            if (line.empty()) {
                ground_truth_probability_tables[i][j][0] = -1; // just mark for the evidence nodes
            } else {
                vector<string> parsed_line = Split(line, " ");

                int dim = dynamic_cast<DiscreteNode*>(network->FindNodePtrByIndex(j))->GetDomainSize();
                for (int k = 0; k < dim; ++k) {
                    ground_truth_probability_tables[i][j][k] = stod(parsed_line[k]);
                }
            }
        }
    }
}

/**
 * @brief: compute the MSE (Mean Square Error) for one instance
 * refer to AIS-BN paper for the MSE formula
 * @param approximate_distribution the approximate distribution
 */
double Inference::CalculateMSE(const vector<vector<double>> &approximate_distribution, int instance_index) {
    // the exact distribution
    vector<vector<double>> exact_distribution = ground_truth_probability_tables[instance_index];

    int num = 0;
    double error = 0.0;
    for (int i = 0; i < network->num_nodes; ++i) {
        if (exact_distribution[i][0] > 0) { // for non-evidence nodes
            int dim = dynamic_cast<DiscreteNode*>(network->FindNodePtrByIndex(i))->GetDomainSize();
            num += dim;
            for (int j = 0; j < dim; ++j) {
                error += pow((Round(approximate_distribution[i][j], 7) - exact_distribution[i][j]), 2);
            }
        }
    }
    return sqrt(error/num);
}

/**
 * @brief: compute the Hellinger's Distance for one instance
 * refer to EPIS-BN paper for the Hellinger's Distance formula
 * @param approximate_distribution the approximate distribution
 */
double Inference::CalculateHellingerDistance(const vector<vector<double>> &approximate_distribution,
                                             int instance_index) {
    // the exact distribution
    vector<vector<double>> exact_distribution = ground_truth_probability_tables[instance_index];

    int num = 0;
    double error = 0.0;
    for (int i = 0; i < network->num_nodes; ++i) {
        if (exact_distribution[i][0] > 0) { // for non-evidence nodes
            int dim = dynamic_cast<DiscreteNode*>(network->FindNodePtrByIndex(i))->GetDomainSize();
            num += dim;
            for (int j = 0; j < dim; ++j) {
                error += pow(sqrt(Round(approximate_distribution[i][j], 7)) - sqrt(exact_distribution[i][j]), 2);
            }
        }
    }
    return sqrt(error/num);
}

double Round(double number, unsigned int bits) {
	long long integerpart = number;
	number -= integerpart;
	for (unsigned int i = 0; i < bits; ++i) {
		number *= 10;
	}
	number = (long long)(number + 0.5);
	for (unsigned int i=0;i<bits;++i) {
		number /= 10;
	}
	return integerpart + number;
}