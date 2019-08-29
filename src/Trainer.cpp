//
// Created by Linjian Li on 2018/11/29.
//

#include "Trainer.h"


Trainer::Trainer() {}


void Trainer::LoadLIBSVMDataAutoDetectConfig(string data_file_path) {

  ifstream in_file;
  in_file.open(data_file_path);
  if (!in_file.is_open()) {
    fprintf(stderr, "Error in function %s!", __FUNCTION__);
    fprintf(stderr, "Unable to open file %s!", data_file_path.c_str());
    exit(1);
  }

  cout << "Data file opened. Begin to load data. " << endl;

  string sample;
  int max_index_occurred = -1;
  // Load data.
  getline(in_file, sample);
  while (!in_file.eof()) {
    // There is a whitespace at the end of each line of
    // libSVM dataset format, which will cause a bug if we do not trim it.
    sample = TrimRight(sample);
    vector<string> parsed_sample = Split(sample, " ");
    auto it = parsed_sample.begin();   // The label is the first element.
    set_label_possible_values.insert(stoi(*it));
    train_set_y_vector.push_back(stoi(*it));

    vector<pair<int,int>> single_sample_vector;
    for (++it; it!=parsed_sample.end(); ++it) {
      // Each element is in the form of "feature_index:feature_value".
      vector<string> parsed_feature = Split(*it, ":");
      int index = stoi(parsed_feature[0]);
      int value = stoi(parsed_feature[1]);
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

  num_instance = train_set_y_vector.size();
  num_feature = max_index_occurred;
  num_vars = num_feature + 1;
  is_features_discrete = new bool[num_feature];
  num_of_possible_values_of_features = new int[num_feature];
  num_of_possible_values_of_label = set_label_possible_values.size();

  for (int i=0; i<num_feature; ++i) {   // Note that the feature indexes start at 1 in LIBSVM.

    // Because features of LIBSVM format do not record the value of 0, we need to add it in.
    map_feature_possible_values[i+1].insert(0);
    num_of_possible_values_of_features[i] = map_feature_possible_values[i+1].size();
    is_features_discrete[i] = true; // For now, we can only process discrete features.
  }

  ConvertLIBSVMVectorDatasetIntoArrayDataset();

  cout << "Finish loading data. " << '\n'
       << "Number of instances: " << num_instance << ". \n"
       << "Number of features (maximum feature index occurred): " << num_feature << ". " << endl;

  in_file.close();

}


void Trainer::ConvertLIBSVMVectorDatasetIntoArrayDataset() {
  // Initialize train_set to be all zero.
  train_set_y = new int [num_instance];
  train_set_X = new int *[num_instance];
  train_set_y_X = new int *[num_instance];
  #pragma omp parallel for
  for (int s=0; s<num_instance; ++s) {
    train_set_y[s] = train_set_y_vector[s];

    train_set_X[s] = new int[num_feature]();  // The parentheses at end will initialize the array to be all zeros.
    vector<pair<int,int>> x_vector = train_set_X_vector[s];
    for (pair<int,int> p : x_vector) {  // For each non-zero-value feature of this sample.
      train_set_X[s][p.first-1] = p.second;
    }

    train_set_y_X[s] = new int[num_vars]();
    train_set_y_X[s][0] = train_set_y[s];
    for (int i=1; i<num_vars; ++i) {
      train_set_y_X[s][i] = train_set_X[s][i-1];
    }
  }
}


void Trainer::ConvertCSVVectorDatasetIntoArrayDataset() {
  // todo: test correctness
  dataset_all_vars = new int *[num_instance];
  #pragma omp parallel for
  for (int s=0; s<num_instance; ++s) {
    dataset_all_vars[s] = new int[num_vars]();  // The parentheses at end will initialize the array to be all zeros.
    for (pair<int,int> p : vector_train_set_all_vars.at(s)) {
      dataset_all_vars[s][p.first] = p.second;
    }
  }
}


void Trainer::LoadCSVDataAutoDetectConfig(string data_file_path) {
  // todo: test correctness
  ifstream in_file;
  in_file.open(data_file_path);
  if (!in_file.is_open()) {
    fprintf(stderr, "Error in function %s!", __FUNCTION__);
    fprintf(stderr, "Unable to open file %s!", data_file_path.c_str());
    exit(1);
  }
  cout << "Data file opened. Begin to load data. " << endl;

  string sample;
  getline(in_file, sample);   // The first line is like the table head.
  sample = TrimRight(sample);
  vector<string> parsed_variable = Split(sample, ",");
  num_vars = parsed_variable.size();
  num_of_possible_values_of_vars = new int[num_vars]();

  // Load data.
  getline(in_file, sample);
  while (!in_file.eof()) {
    // If there is a whitespace at the end of each line,
    // it will cause a bug if we do not trim it.
    sample = TrimRight(sample);
    vector<string> parsed_sample = Split(sample, ",");
    vector<pair<int,int>> single_sample_vector;
    for (int i=0; i<num_vars; ++i) {
      int value = stoi(parsed_sample.at(i));
      map_vars_possible_values[i].insert(value);
      pair<int,int> pair_feature_value(i, value);
      single_sample_vector.push_back(pair_feature_value);
    }
    vector_train_set_all_vars.push_back(single_sample_vector);
    getline(in_file, sample);
  }

  num_instance = vector_train_set_all_vars.size();
  is_vars_discrete = new bool[num_vars];
  num_of_possible_values_of_vars = new int[num_vars];

  for (int i=0; i<num_vars; ++i) {
    num_of_possible_values_of_vars[i] = map_vars_possible_values[i].size();
    is_vars_discrete[i] = true; // For now, we can only process discrete features.
  }

  ConvertCSVVectorDatasetIntoArrayDataset();

  cout << "Finish loading data. " << '\n'
       << "Number of instances: " << num_instance << ".\n"
       << "Number of variables: " << num_feature << "." << endl;

  in_file.close();
}


void Trainer::SamplesToLIBSVMFile(vector<Combination> &samples, string &file) const {
  FILE *f;
  f = fopen(file.c_str(), "w");
  for (auto &smp : samples) {
    string string_to_write = "";

    for (auto &var_and_val : smp) {
      // The following codes should not use "+=", because the order matters.
      if(var_and_val.first==0) {
        string label = var_and_val.second==1 ? "+1" : "-1";
        string_to_write = label + " " + string_to_write;
      } else {
        if (var_and_val.second!=0) {
          string_to_write = string_to_write
                            + to_string(var_and_val.first) + ":"
                            + to_string(var_and_val.second) + " ";
        }
      }
    }
    fprintf(f, "%s\n", string_to_write.c_str());
  }
  fclose(f);
}

void Trainer::SamplesToCSVFile(vector<Combination> &samples, string &file) const {
  // Detect configuration.
  Combination &c = samples.front();
  auto it = c.end();
  int max_index = (*(--it)).first;

  FILE *f;
  f = fopen(file.c_str(), "w");

  string first_line = "";   // The first line is like the table head.
  for (int i=0; i<max_index; ++i) {
    first_line += to_string(i) + ',';
  }
  first_line = first_line.substr(0, first_line.size()-1);   // No comma at the last character.
  fprintf(f, "%s\n", first_line.c_str());

  for (auto &smp : samples) {
    string string_to_write = "";
    for (auto &var_and_val : smp) {
      string_to_write += to_string(var_and_val.second) + ',';
    }
    string_to_write = string_to_write.substr(0, string_to_write.size()-1);   // No comma at the last character.
    fprintf(f, "%s\n", string_to_write.c_str());
  }

  fclose(f);
}