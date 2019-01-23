//
// Created by Linjian Li on 2018/11/29.
//

#include "Trainer.h"

Trainer::Trainer() {}

void Trainer::LoadLIBSVMDataAutoDetectConfig(string data_file_path) {

  int max_index_occurred = -1;

  ifstream in_file;
  in_file.open(data_file_path);
  if (!in_file.is_open()) {
	  fprintf(stderr, "Unable to open file %s!", data_file_path.c_str());
    exit(1);
  }

  cout << "Data file opened. Begin to load data. " << endl;

  string sample;
  vector<string> parsed_sample;
  vector<string> parsed_feature;
  vector<string>::iterator it;
  int index, value;

  // Load data.
  getline(in_file, sample);
  while (!in_file.eof()) {
    // There is a whitespace at the end of each line of libSVM data set format, which will will cause a bug.
    sample = boost::algorithm::trim_right_copy(sample);
    boost::algorithm::split(parsed_sample, sample, boost::algorithm::is_space());
    it=parsed_sample.begin();
    set_label_possible_values.insert(stoi(*it));
    train_set_y_vector.push_back(stoi(*it));
    ++it;
    vector<pair<int,int>> single_sample_vector;
    for (/*pass*/ ; it!=parsed_sample.end(); ++it) {
      boost::algorithm::split(parsed_feature, *it, boost::algorithm::is_any_of(":"));
      index = stoi(parsed_feature[0]);
      value = stoi(parsed_feature[1]);
      max_index_occurred = index>max_index_occurred ? index : max_index_occurred;
      map_feature_possible_values[index].insert(value);
      pair<int,int> pair_feature_value;
      pair_feature_value.first = index;
      pair_feature_value.second = value;
      single_sample_vector.push_back(pair_feature_value);
    }
    train_set_X_vector.push_back(single_sample_vector);
    getline(in_file, sample);
  }

  n_train_instance = train_set_y_vector.size();
  n_feature = max_index_occurred;
  is_features_discrete = new bool[n_feature];
  num_of_possible_values_of_features = new int[n_feature];
  num_of_possible_values_of_label = set_label_possible_values.size();

  for (int i=1; i<=n_feature; ++i) {  // Note that the feature indexes start at 1 in LIBSVM.
    map_feature_possible_values[i].insert(0);	// Because feature of LIBSVM format do not record the value of 0, we need to add it in.
    num_of_possible_values_of_features[i] = map_feature_possible_values[i].size();
    is_features_discrete[i] = true; // For now, we can only process discrete features.
  }

  ConvertVectorDatasetIntoArrayDataset();

  cout << "Finish loading data. " << '\n'
       << "Number of training samples: " << n_train_instance << ". \n"
       << "Number of features (maximum feature index occurred): " << n_feature << ". " << endl;

  in_file.close();

}


void Trainer::ConvertVectorDatasetIntoArrayDataset() {
	// Initialize train_set to be all zero.
	train_set_y = new int [n_train_instance];
	train_set_X = new int *[n_train_instance];
	train_set_y_X = new int *[n_train_instance];
	for (int s=0; s<n_train_instance; ++s) {
		train_set_y[s] = train_set_y_vector[s];

		train_set_X[s] = new int[n_feature]();	// The parentheses at end will initialize the array to be all zeros.
		vector<pair<int,int>> x_vector = train_set_X_vector[s];
		for (pair<int,int> p : x_vector) {	// For each non-zero-value feature of this sample.
			train_set_X[s][p.first-1] = p.second;
		}

		train_set_y_X[s] = new int[n_feature+1]();
		train_set_y_X[s][0] = train_set_y[s];
		for (int i=1; i<=n_feature; ++i) {
			train_set_y_X[s][i] = train_set_X[s][i-1];
		}
	}
}