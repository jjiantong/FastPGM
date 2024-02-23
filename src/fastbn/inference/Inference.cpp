//
// Created by jjt on 2021/6/15.
//
#include "fastbn/inference/Inference.h"

/**
 * constructor of Inference
 * get evidences, query index and ground truths from the testing set.
 * @param classification true for classification mode, false for inference mode
 * @param net the network used for inference
 * @param dts the testing set
 * note: i avoid filling zero operations (if necessary) here, because it should be completed when loading data (e.g.
 * libsvm format)
 */
Inference::Inference(bool classification, Network *net, Dataset *dts):
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
    int size = 0;
    for (int i = 0; i < network->num_nodes; ++i) {
        size += dynamic_cast<DiscreteNode*>(network->FindNodePtrByIndex(i))->GetDomainSize();
    }

    ground_truth_probability_tables.resize(num_instances);
    for (int i = 0; i < num_instances; ++i) {
        ground_truth_probability_tables[i].resize(size);
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
        int l = 0;
        for (int j = 0; j < network->num_nodes; ++j) { // for each node
            // the file has #instances * #nodes lines in total
            getline(in_file, line);
            line = TrimRight(line);
            if (line.empty()) {
                ground_truth_probability_tables[i][l++] = -1; // just mark for the evidence nodes
            } else {
                vector<string> parsed_line = Split(line, " ");

                int dim = dynamic_cast<DiscreteNode*>(network->FindNodePtrByIndex(j))->GetDomainSize();
                for (int k = 0; k < dim; ++k) {
                    ground_truth_probability_tables[i][l++] = stod(parsed_line[k]);
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
double Inference::CalculateMSE(const vector<double> &approximate_distribution, int instance_index) {
    // the exact distribution
    vector<double> exact_distribution = ground_truth_probability_tables[instance_index];

    int num = 0;
    double error = 0.0;
    for (int i = 0; i < exact_distribution.size(); ++i) {
        if (exact_distribution[i] > 0) {
            num++;
            error += pow((Round(approximate_distribution[i], 7) - exact_distribution[i]), 2);
        }
    }
    return sqrt(error/num);
}

/**
 * @brief: compute the Hellinger's Distance for one instance
 * refer to EPIS-BN paper for the Hellinger's Distance formula
 * @param approximate_distribution the approximate distribution
 */
double Inference::CalculateHellingerDistance(const vector<double> &approximate_distribution,
                                             int instance_index) {
    // the exact distribution
    vector<double> exact_distribution = ground_truth_probability_tables[instance_index];

    int num = 0;
    double error = 0.0;
    for (int i = 0; i < exact_distribution.size(); ++i) {
        if (exact_distribution[i] > 0) {
            num++;
            error += pow(sqrt(Round(approximate_distribution[i], 7))
                    - sqrt(exact_distribution[i]), 2);
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