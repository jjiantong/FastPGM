#pragma clang diagnostic push
#pragma ide diagnostic ignored "openmp-use-default-none"
//
// Created by Linjian Li on 2018/11/29.
//

#include "Dataset.h"


Dataset::Dataset() {}


void Dataset::LoadLIBSVMDataAutoDetectConfig(string data_file_path, set<int> cont_vars) {

  class_var_index = 0;

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
  vector<Value> dataset_y_vector;
  vector<vector<VarVal>> dataset_X_vector;
  getline(in_file, sample);
  while (!in_file.eof()) {
    // There is a whitespace at the end of each line of
    // libSVM dataset format, which will cause a bug if we do not trim it.
    sample = TrimRight(sample);
    vector<string> parsed_sample = Split(sample, " ");
    int it = 0;   // The label is the first element.
    if (cont_vars.find(0)==cont_vars.end()) {
      map_disc_vars_possible_values[class_var_index].insert(stoi(parsed_sample.at(it)));
      Value v;
      v.SetInt(stoi(parsed_sample.at(it)));
      dataset_y_vector.push_back(v);
    } else {
      Value v;
      v.SetFloat(stof(parsed_sample.at(it)));
      dataset_y_vector.push_back(v);
    }

    vector<VarVal> single_sample_vector;
    for (++it; it < parsed_sample.size(); ++it) {
      // Each element is in the form of "feature_index:feature_value".
      string feature_val = parsed_sample.at(it);
      vector<string> parsed_feature_val = Split(feature_val, ":");
      int index = stoi(parsed_feature_val[0]);
      max_index_occurred = index>max_index_occurred ? index : max_index_occurred;
      Value v;
      if (cont_vars.find(index)==cont_vars.end()) {
        int value = stoi(parsed_feature_val[1]);
        v.SetInt(value);
        map_disc_vars_possible_values[index].insert(value);
      } else {
        float value = stof(parsed_feature_val[1]);
        v.SetFloat(value);
      }
      VarVal var_value(index, v);
      single_sample_vector.push_back(var_value);
    }
    dataset_X_vector.push_back(single_sample_vector);
    getline(in_file, sample);
  }

  vector_dataset_all_vars = dataset_X_vector;

  for (int i=0; i<vector_dataset_all_vars.size(); ++i) {
    vector_dataset_all_vars.at(i).insert(
            vector_dataset_all_vars.at(i).begin(),
            VarVal(class_var_index,dataset_y_vector.at(i))
    );
  }


  num_instance = vector_dataset_all_vars.size();
  num_vars = max_index_occurred + 1;
  is_vars_discrete.reserve(num_vars);
  num_of_possible_values_of_disc_vars = new int[num_vars];

  for (int i=0; i<num_vars; ++i) {
    if (i!=class_var_index) {
      if (cont_vars.find(i)==cont_vars.end()) {
        // Because features of LIBSVM format do not record the value of 0, we need to add it in.
        map_disc_vars_possible_values[i].insert(0);
      }
    }
    num_of_possible_values_of_disc_vars[i] = map_disc_vars_possible_values[i].size();
    is_vars_discrete.push_back(cont_vars.find(i)==cont_vars.end()); // For now, we can only process discrete variables.
  }

  if (cont_vars.empty()) {
    ConvertLIBSVMVectorDatasetIntoIntArrayDataset();
  }

  cout << "Finish loading data. " << '\n'
       << "Number of instances: " << num_instance << ". \n"
       << "Number of features (maximum feature index occurred): " << max_index_occurred << ". " << endl;

  in_file.close();

}


void Dataset::ConvertLIBSVMVectorDatasetIntoIntArrayDataset() {
  // Initialize to be all zero.
  dataset_all_vars = new int *[num_instance];
#pragma omp parallel for
  for (int s=0; s<num_instance; ++s) {
    dataset_all_vars[s] = new int[num_vars]();
    vector<VarVal> vec_instance = vector_dataset_all_vars.at(s);
    for (const VarVal &vv : vec_instance) {  // For each non-zero-value feature of this sample.
      dataset_all_vars[s][vv.first] = vv.second.GetInt();
    }
  }
}


void Dataset::ConvertCSVVectorDatasetIntoIntArrayDataset() {
  dataset_all_vars = new int *[num_instance];
#pragma omp parallel for
  for (int s=0; s<num_instance; ++s) {
    dataset_all_vars[s] = new int[num_vars]();  // The parentheses at end will initialize the array to be all zeros.
    for (const VarVal &vv : vector_dataset_all_vars.at(s)) {
      dataset_all_vars[s][vv.first] = vv.second.GetInt();
    }
  }
}


void Dataset::LoadCSVDataAutoDetectConfig(string data_file_path, bool header, int cls_var_id, set<int> cont_vars) {
  this->class_var_index = cls_var_id;
  ifstream in_file;
  in_file.open(data_file_path);
  if (!in_file.is_open()) {
    fprintf(stderr, "Error in function %s!", __FUNCTION__);
    fprintf(stderr, "Unable to open file %s!", data_file_path.c_str());
    exit(1);
  }
  cout << "Data file opened. Begin to load data. " << endl;

  string sample;
  // Use the first line to detect the dataset attributes. It is like the table head.
  getline(in_file, sample);
  sample = TrimRight(sample);
  vector<string> parsed_variable = Split(sample, ",");
  num_vars = parsed_variable.size();
  is_vars_discrete.reserve(num_vars);
  num_of_possible_values_of_disc_vars = new int[num_vars]();

  // Load data.
  if (header) {
    getline(in_file, sample);
  }
  while (!in_file.eof()) {
    // If there is a whitespace at the end of each line,
    // it will cause a bug if we do not trim it.
    sample = TrimRight(sample);
    vector<string> parsed_sample = Split(sample, ",");
    vector<VarVal> single_sample_vector;
    for (int i=0; i<num_vars; ++i) {
      Value v;
      if (cont_vars.find(i)==cont_vars.end()) {
        int value = stoi(parsed_sample.at(i));
        v.SetInt(value);
        map_disc_vars_possible_values[i].insert(value);
      } else {
        float value = stof(parsed_sample.at(i));
        v.SetFloat(value);
      }

      VarVal var_value(i, v);
      single_sample_vector.push_back(var_value);
    }
    vector_dataset_all_vars.push_back(single_sample_vector);
    getline(in_file, sample);
  }

  num_instance = vector_dataset_all_vars.size();
  num_of_possible_values_of_disc_vars = new int[num_vars];

  for (int i=0; i<num_vars; ++i) {
    num_of_possible_values_of_disc_vars[i] = map_disc_vars_possible_values[i].size();
    is_vars_discrete.push_back(cont_vars.find(i)==cont_vars.end()); // For now, we can only process discrete features.
  }

  if (cont_vars.empty()) {
    ConvertCSVVectorDatasetIntoIntArrayDataset();
  }

  cout << "Finish loading data. " << '\n'
       << "Number of instances: " << num_instance << ".\n"
       << "Number of variables: " << num_vars << "." << endl;

  in_file.close();
}


void Dataset::SamplesToLIBSVMFile(vector<DiscreteConfig> &samples, string &file) const {
  vector<Configuration> smps;
  for (const auto &samp : samples) {
    Configuration cfg;
    for (const auto &p : samp) {
      VarVal vv;
      vv.first = p.first;
      vv.second.SetInt(p.second);
      cfg.insert(vv);
    }
    smps.push_back(cfg);
  }
  SamplesToLIBSVMFile(smps, file);
}


void Dataset::SamplesToLIBSVMFile(vector<Configuration> &samples, string &file) const {
  FILE *f;
  f = fopen(file.c_str(), "w");
  for (auto &smp : samples) {
    string string_to_write = "";

    for (auto &var_val : smp) {
      // The following codes should not use "+=", because the order matters.
      int var = var_val.first;
      Value val = var_val.second;
      if(var == 0) {
        string label;
        if (val.UseInt()) {
          label = to_string(val.GetInt());
        } else {
          label = to_string(val.GetFloat());
        }
        string_to_write = label + " " + string_to_write;
      } else {
        string feature_value = "";
        if (val.UseInt()) {
          if (val.GetInt() != 0) {
            feature_value = to_string(var) + ":" + to_string(val.GetInt()) + " ";
          }
        } else {
          if (val.GetFloat() != 0) {
            feature_value = to_string(var) + ":" + to_string(val.GetFloat()) + " ";
          }
        }
        string_to_write += feature_value;
      }
    }
    fprintf(f, "%s\n", string_to_write.c_str());
  }
  fclose(f);
}


void Dataset::SamplesToCSVFile(vector<DiscreteConfig> &samples, string &file) const {
  vector<Configuration> smps;
  for (const auto &samp : samples) {
    Configuration cfg;
    for (const auto &p : samp) {
      VarVal vv;
      vv.first = p.first;
      vv.second.SetInt(p.second);
      cfg.insert(vv);
    }
    smps.push_back(cfg);
  }
  SamplesToCSVFile(smps, file);
}


void Dataset::SamplesToCSVFile(vector<Configuration> &samples, string &file) const {
  // Detect configuration.
  auto &c = samples.front();
  auto it = c.end();
  int max_index = (*(--it)).first;

  FILE *f;
  f = fopen(file.c_str(), "w");

  string first_line = "";   // The first line is like the table head.
  for (int i=0; i<=max_index; ++i) {
    first_line += to_string(i) + ',';
  }
  first_line = first_line.substr(0, first_line.size()-1);   // No comma at the last character.
  fprintf(f, "%s\n", first_line.c_str());

  for (auto &smp : samples) {
    string string_to_write = "";
    for (auto &var_and_val : smp) {
      if (var_and_val.second.UseInt()) {
        string_to_write += to_string(var_and_val.second.GetInt()) + ',';
      } else {
        string_to_write += to_string(var_and_val.second.GetFloat()) + ',';
      }
    }
    string_to_write = string_to_write.substr(0, string_to_write.size()-1);   // No comma at the last character.
    fprintf(f, "%s\n", string_to_write.c_str());
  }

  fclose(f);
}
