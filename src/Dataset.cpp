#pragma clang diagnostic push
#pragma ide diagnostic ignored "openmp-use-default-none"
//
// Created by Linjian Li on 2018/11/29.
//

#include "Dataset.h"


Dataset::Dataset() {}

/**
 * @brief: AutoDetect means to automatically find the possible values for each discrete variable
 * 1, read the data file and store with the representation of std::vector.
 * 2, convert the vector representation into array (does not erase the vector representation).
 * @param: data_file_path: path to the LIBSVM data file (like a1a)
 * @param: cont_vars: indexes for continuous variables; need to manually specify which variable is continuous
 */
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

  // 1, read the data file and store with the representation of std::vector.
  getline(in_file, sample);
  while (!in_file.eof()) {
    // There is a whitespace at the end of each line of
    // libSVM dataset format, which will cause a bug if we do not trim it.
    sample = TrimRight(sample);
    vector<string> parsed_sample = Split(sample, " ");
    int it = 0;   // id of the label is 0

    // check whether the label is continuous
    if (cont_vars.find(0) == cont_vars.end()) {
      //the label is not continuous (i.e., classification task)
      // insert the value of label of one sample into "map_disc_vars_possible_values"
      // "parsed_sample.at(it)" is the value of label
      map_disc_vars_possible_values[class_var_index].insert(stoi(parsed_sample.at(it)));
      // insert the value of label of one sample into "dataset_y_vector"
      Value v;
      v.SetInt(stoi(parsed_sample.at(it)));
      dataset_y_vector.push_back(v);
    }
    else {
      //the label is continuous (i.e., regression task)
      // insert the value of label of one sample into "dataset_y_vector"
      Value v;
      v.SetFloat(stof(parsed_sample.at(it)));
      dataset_y_vector.push_back(v);
    }//end storing the label of the data set

    vector<VarVal> single_sample_vector;
    for (++it; it < parsed_sample.size(); ++it) {
      // Each element is in the form of "feature_index:feature_value".
      string feature_val = parsed_sample.at(it);
      // split the feature index and the feature value using ":"
      vector<string> parsed_feature_val = Split(feature_val, ":");
      int index = stoi(parsed_feature_val[0]);
      max_index_occurred = index > max_index_occurred ? index : max_index_occurred;
      Value v;
      if (cont_vars.find(index) == cont_vars.end()) {//same as the processing of label TODO: maintain consistent, change the former code
        int value = stoi(parsed_feature_val[1]);
        v.SetInt(value);
        map_disc_vars_possible_values[index].insert(value);
      }
      else {
        float value = stof(parsed_feature_val[1]);
        v.SetFloat(value);
      }
      VarVal var_value(index, v);
      single_sample_vector.push_back(var_value);
    }
    dataset_X_vector.push_back(single_sample_vector);
    getline(in_file, sample);
  }

  // vector_dataset_all_vars: vector<vector<VarVal>>; label + feature
  vector_dataset_all_vars = dataset_X_vector;
  //insert label to the beginning of each instance
  for (int i=0; i<vector_dataset_all_vars.size(); ++i) {
    vector_dataset_all_vars.at(i).insert(
            vector_dataset_all_vars.at(i).begin(),
            VarVal(class_var_index,dataset_y_vector.at(i))
    );
  }

  num_instance = vector_dataset_all_vars.size();
  num_vars = max_index_occurred + 1;//the number of variables of the data set

  is_vars_discrete.reserve(num_vars);
  num_of_possible_values_of_disc_vars.reserve(num_vars);

  for (int i=0; i<num_vars; ++i) {
    vec_var_names.push_back(to_string(i));//the name of a variable is the "id" of the variable.
  }

  //discrete variable domain construction, whether a variable is continuous
  for (int i=0; i<num_vars; ++i) {
    // for features
    if (i!=class_var_index) {
      if (cont_vars.find(i)==cont_vars.end()) {
        // Because features of LIBSVM format do not record the value of 0, we need to add it in.
        map_disc_vars_possible_values[i].insert(0);
      }
    }
    num_of_possible_values_of_disc_vars.push_back(map_disc_vars_possible_values[i].size());
    is_vars_discrete.push_back(cont_vars.find(i)==cont_vars.end()); // TODO: For now, we can only process discrete variables.
  }

  // 2, convert vector "vector_dataset_all_vars" into array "dataset_all_vars" (does not erase "vector_dataset_all_vars").
  // TODO: check the two representations. remove?
  if (cont_vars.empty()) {//the data set only contains discrete variables.
    ConvertLIBSVMVectorDatasetIntoIntArrayDataset();
  }

  cout << "Finish loading data. " << '\n'
       << "Number of instances: " << num_instance << ". \n"
       << "Number of features (maximum feature index occurred): " << max_index_occurred << ". " << endl;

  in_file.close();

}

/*!
 * @brief: convert from vector ("vector_dataset_all_vars") to array ("dataset_all_vars")
 * this function is used if the data set only contains discrete variables
 */
void Dataset::ConvertLIBSVMVectorDatasetIntoIntArrayDataset() {//storing the data set using int only
  // Initialize to be all zero. (dataset_all_vars: int **)
  dataset_all_vars = new int *[num_instance];
#pragma omp parallel for
  for (int s=0; s<num_instance; ++s) {
    dataset_all_vars[s] = new int[num_vars]();
    vector<VarVal> vec_instance = vector_dataset_all_vars.at(s);
    for (const VarVal &vv : vec_instance) {  // For each non-zero-value feature of this sample.
      //change the related value if it is non-zero value in the vector representation.
      dataset_all_vars[s][vv.first] = vv.second.GetInt();
    }
  }
}

/**
 * @brief: convert a data set with float and int, to a data set with int only
 */
void Dataset::ConvertCSVVectorDatasetIntoIntArrayDataset() {//need to redesign this function
  dataset_all_vars = new int *[num_instance];
#pragma omp parallel for
  for (int s=0; s<num_instance; ++s) {
    dataset_all_vars[s] = new int[num_vars]();  // The parentheses at end will initialize the array to be all zeros.
    for (const VarVal &vv : vector_dataset_all_vars.at(s)) {
      dataset_all_vars[s][vv.first] = vv.second.GetInt();
    }
  }
}

/**
 * @brief: similar to Load Data from LibSVM: cf. LoadLIBSVMDataAutoDetectConfig.
 */
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
  num_of_possible_values_of_disc_vars.reserve(num_vars);


  if (header) {
    vec_var_names = parsed_variable;
    set<string> temp;
    temp.insert(parsed_variable.begin(), parsed_variable.end());
    if (temp.size() != parsed_variable.size()) {
      fprintf(stderr, "Error in function [%s]\nDuplicate variable names in header!", __FUNCTION__);
      exit(1);
    }
    getline(in_file, sample);
  }

  // Load data.
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
  num_of_possible_values_of_disc_vars.reserve(num_vars);

  for (int i=0; i<num_vars; ++i) {
    num_of_possible_values_of_disc_vars.push_back(map_disc_vars_possible_values[i].size());
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

/**
 * @brief: this function is to save the processed data into file, for ease debugging and also for approximate inference.
 * This function is for conversion only; SamplesToLIBSVMFile(smps, file) does all the file saving.
 * DiscreteConfig: set< pair<int, int> >
 */
void Dataset::SamplesToLIBSVMFile(vector<DiscreteConfig> &samples, string &file) const {
  vector<Configuration> smps;
  for (const auto &samp : samples) { // for each DiscreteConfig samp
    Configuration cfg;
    // for each <int, int> pair, store into cfg (pair<int, Value>)
    for (const auto &p : samp) {
      VarVal vv; // VarVal: pair<int, Value>
      vv.first = p.first;
      vv.second.SetInt(p.second);
      cfg.insert(vv);
    }
    smps.push_back(cfg);//
  }
  SamplesToLIBSVMFile(smps, file);
}

/**
 * @brief: the instances in this sample are generated by the learned network.
 * the file has exactly the same format as libsvm.
 * Configuration: set< pair<int, Value> >
 */
void Dataset::SamplesToLIBSVMFile(vector<Configuration> &samples, string &file) const {
  FILE *f;
  f = fopen(file.c_str(), "w");

  for (auto &smp : samples) { // for each Configuration smp
    string string_to_write = "";

    for (auto &var_val : smp) { // for each <int, Value> pair var_val
      // The following codes should not use "+=", because the order matters.
      int var = var_val.first;
      Value val = var_val.second;
      if(var == 0) {
        string label;
        if (val.UseInt()) {
          label = to_string(val.GetInt());
        }
        else {
          label = to_string(val.GetFloat());
        }
        string_to_write = label + " " + string_to_write;
      }
      else {
        string feature_value = "";
        if (val.UseInt()) {
          if (val.GetInt() != 0) { // Because features of LIBSVM format do not record the value of 0
            feature_value = to_string(var) + ":" + to_string(val.GetInt()) + " ";
          }
        }
        else {
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

/**
 * @brief: similar to SamplesToLIBSVMFile, but the file format is csv.
 */
void Dataset::SamplesToCSVFile(vector<DiscreteConfig> &samples, string &file, vector<string> header) const {
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
  SamplesToCSVFile(smps, file, header);
}

/**
 * @brief: similar to SamplesToLIBSVMFile, but the file format is csv.
 */
void Dataset::SamplesToCSVFile(vector<Configuration> &samples, string &file, vector<string> header) const {
  // Detect configuration.
  auto &c = samples.front();
  auto it = c.end();
  int max_index = (*(--it)).first;

  FILE *f;
  f = fopen(file.c_str(), "w");

  string first_line = "";   // The first line is like the table head.
  if (header.empty() || (vec_var_names.size() == max_index+1)) {  // Because index starts at 0.
    for (const auto &name : vec_var_names) {
      first_line += name + ',';
    }
  } else if (header.size() == max_index+1) {  // Because index starts at 0.
    for (const auto &name : header) {
      first_line += name + ',';
    }
  } else {
    fprintf(stderr, "Error in function [%s]!\nInvalid header size.", __FUNCTION__);
    exit(1);
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
