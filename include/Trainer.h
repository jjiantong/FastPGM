//
// Created by Linjian Li on 2018/11/29.
//

#ifndef BAYESIANNETWORK_TRAINER_H
#define BAYESIANNETWORK_TRAINER_H

#include <string>
#include <set>
#include <vector>
#include <map>
#include <utility>
#include <iostream>
#include <fstream>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;

typedef set< pair<int, int> > Combination;

class Trainer {

public:
	int n_train_instance;
	int n_feature;
	bool* is_features_discrete;
	int* num_of_possible_values_of_features;
	map<int,set<int>> map_feature_possible_values;
	int num_of_possible_values_of_label;
	set<int> set_label_possible_values;
	int *train_set_y;
	int **train_set_X;
	int **train_set_y_X;  // This is for the convenience of calculating mutual information.
	vector<vector<pair<int,int>>> train_set_X_vector;
	vector<int> train_set_y_vector;
	// incomp_data;
	// incomp_data_expectation;

	Trainer();
	void LoadLIBSVMDataAutoDetectConfig(string);
	void ConvertVectorDatasetIntoArrayDataset();
};


#endif //BAYESIANNETWORK_TRAINER_H
