#pragma clang diagnostic push
#pragma ide diagnostic ignored "openmp-use-default-none"

#include "Dataset.h"


Dataset::Dataset() {}

Dataset::~Dataset() {
    for (int i = 0; i < num_instance; ++i) {
        delete [] dataset_all_vars[i];
        dataset_all_vars[i] = nullptr;
    }
    delete [] dataset_all_vars;
    dataset_all_vars = nullptr;

    for (int i = 0; i < num_vars; ++i) {
        delete [] dataset_columns[i];
        dataset_columns[i] = nullptr;
    }
    delete [] dataset_columns;
    dataset_columns = nullptr;
}

/**
 * @brief: load data file with libsvm format
 * 1, read the data file and store with the representation of std::vector.
 * 2, convert the vector representation into array (does not erase the vector representation).
 * @param: data_file_path: path to the LIBSVM data file (like a1a)
 * @param: cont_vars: indexes for continuous variables; need to manually specify which variable is continuous
 */
void Dataset::LoadLIBSVMData(string data_file_path, set<int> cont_vars) {

    ifstream in_file;
    in_file.open(data_file_path);
    if (!in_file.is_open()) {
        fprintf(stderr, "Error in function %s!", __FUNCTION__);
        fprintf(stderr, "Unable to open file %s!", data_file_path.c_str());
        exit(1);
    }
    cout << "Data file opened. Begin to load data. " << endl;

    // the first element is the class variable in the LibSVN format
    class_var_index = 0;
    string sample;
    int max_index_occurred = -1;
    vector<Value> dataset_y_vector;
    vector<vector<VarVal>> dataset_X_vector; // VarVal: pair<int, Value>

    // 1, read the data file and store with the representation of std::vector.
    getline(in_file, sample);
    while (!in_file.eof()) {
        // There is a whitespace at the end of each line of
        // libSVM dataset format, which will cause a bug if we do not trim it.
        sample = TrimRight(sample);

        vector<string> parsed_sample = Split(sample, " ");
        int it = 0;   // id of the label is 0

        Value v; // to insert the value of label of one sample into "dataset_y_vector"
        // check whether the label is continuous
        if (cont_vars.find(0) == cont_vars.end()) {
            //the label is not continuous (i.e., classification task)
            int value = stoi(parsed_sample[it]); // the value of label
            v.SetInt(value);
            // insert the value of label of one sample into "map_disc_vars_possible_values"
            map_disc_vars_possible_values[class_var_index].insert(value); // map<int,set<int>>
        } else {
            //the label is continuous (i.e., regression task)
            float value = stof(parsed_sample[it]); // the value of label
            v.SetFloat(value);
        }
        dataset_y_vector.push_back(v); // insert the value of label into "dataset_y_vector"

        vector<VarVal> single_sample_vector; // one instance
        for (++it; it < parsed_sample.size(); ++it) {
            // Each element is in the form of "feature_index:feature_value".
            string feature_val = parsed_sample[it];

            // split the feature index and the feature value using ":"
            vector<string> parsed_feature_val = Split(feature_val, ":");

            int index = stoi(parsed_feature_val[0]);
            max_index_occurred = index > max_index_occurred ? index : max_index_occurred;

            //same as the processing of label
            Value v;
            if (cont_vars.find(index) == cont_vars.end()) {
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

    // vector_dataset_all_vars: vector<vector<VarVal>>; label + feature
    vector_dataset_all_vars = dataset_X_vector;

    //insert label to the beginning of each instance
    for (int i = 0; i < vector_dataset_all_vars.size(); ++i) {
        vector_dataset_all_vars[i].insert(
                vector_dataset_all_vars[i].begin(),
                VarVal(class_var_index, dataset_y_vector[i])
        );
    }

    num_instance = vector_dataset_all_vars.size();
    num_vars = max_index_occurred + 1;//the number of variables of the data set

    is_vars_discrete.reserve(num_vars);
    num_of_possible_values_of_disc_vars.reserve(num_vars);

    for (int i = 0; i < num_vars; ++i) {
        vec_var_names.push_back(to_string(i)); //the name of a variable is the "id" of the variable.
    }

    //discrete variable domain construction, whether a variable is continuous
    for (int i = 0; i < num_vars; ++i) {
        if (i != class_var_index) { // for each feature
            if (cont_vars.find(i) == cont_vars.end()) { // for each discrete feature
                // Because features of LIBSVM format do not record the value of 0, we need to add it in.
                map_disc_vars_possible_values[i].insert(0);
                map_disc_vars_possible_values[i].insert(1); // todo!!
//                map_disc_vars_possible_values[i].insert(2); // todo!!
//                map_disc_vars_possible_values[i].insert(3); // todo!!
//                map_disc_vars_possible_values[i].insert(4); // todo!!
//                map_disc_vars_possible_values[i].insert(5); // todo!!
//                map_disc_vars_possible_values[i].insert(6); // todo!!
//                map_disc_vars_possible_values[i].insert(7); // todo!!
            }
        }
        num_of_possible_values_of_disc_vars.push_back(map_disc_vars_possible_values[i].size());
        is_vars_discrete.push_back(cont_vars.find(i) == cont_vars.end());
    }

    // 2, convert vector "vector_dataset_all_vars" into array "dataset_all_vars" (does not erase "vector_dataset_all_vars").
    // TODO: check the two representations. remove?
    if (cont_vars.empty()) {//the data set only contains discrete variables.
        Vector2IntArray();
        RowMajor2ColumnMajor();
    }

    cout << "Finish loading data. " << '\n'
         << "Number of instances: " << num_instance << ". \n"
         << "Number of features: " << max_index_occurred << ". " << endl;

    in_file.close();
}

/**
 * @brief: load data file with csv format
 */
void Dataset::LoadCSVData(string data_file_path, bool header, bool str_val, int cls_var_id, set<int> cont_vars) {

    ifstream in_file;
    in_file.open(data_file_path);
    if (!in_file.is_open()) {
        fprintf(stderr, "Error in function %s!", __FUNCTION__);
        fprintf(stderr, "Unable to open file %s!", data_file_path.c_str());
        exit(1);
    }
    cout << "Data file opened. Begin to load data. " << endl;

    // we need to specify the variable interested for the CSV format
    this->class_var_index = cls_var_id;
    string sample;
    /**
     * 1, read and parse the first line
     * use the first line to detect the number of variables of the data set
     */
    getline(in_file, sample);
    // If there is a whitespace at the end of each line,
    // it will cause a bug if we do not trim it.
    sample = TrimRight(sample);
    vector<string> parsed_variable = Split(sample, ",");
    num_vars = parsed_variable.size(); // the number of variables of the data set
    is_vars_discrete.reserve(num_vars);
    num_of_possible_values_of_disc_vars.reserve(num_vars);

    if (header) { // the first line contains variable names, it is like the table header
        vec_var_names = parsed_variable;
        set<string> temp;
        temp.insert(parsed_variable.begin(), parsed_variable.end());
        // check whether there are duplicate variable names
        if (temp.size() != parsed_variable.size()) {
            fprintf(stderr, "Error in function [%s]\nDuplicate variable names in header!", __FUNCTION__);
            exit(1);
        }
        getline(in_file, sample);
        sample = TrimRight(sample);
    }

    /**
     * 2, read the data file and store with the representation of std::vector.
     */
    /**
     * are used if some discrete variables contain string values
     *   - map<int, set<string>> map_disc_vars_string_values
     *     key: discrete variable id; value: a set of possible string values
     *     value is set<string>, used to check whether the string is repeated or not, by just inserting into the set
     *   - vector<int> counter
     *     size = num_vars, used to map the string values with different numbers
     *   - vector<map<string, int>> map_string_values_numbers
     *     for each variable, use a map to map string values with different numbers
     *     todo: move to class variable if required for post-processing
     */
    map<int, set<string>> map_disc_vars_string_values;
    vector<int> counter(num_vars, -1);
    vector<map<string, int>> map_string_values_numbers;
    map_string_values_numbers.resize(num_vars);

    while (!in_file.eof()) { // for all instances
        vector<string> parsed_sample = Split(sample, ",");
        vector<VarVal> single_sample_vector;
        for (int i = 0; i < num_vars; ++i) {
            Value v;
            int value;
            // check whether the variable is continuous
            if (cont_vars.find(i) == cont_vars.end()) { //the label is discrete (i.e., classification task)
                if (str_val) { // if some discrete variables contain string values
                    string s_value = parsed_sample[i];
                    // insert successfully -- it is a new possible value
                    if (map_disc_vars_string_values[i].insert(s_value).second) {
                        value = ++counter[i]; // get a new int value
                        map_string_values_numbers[i].insert(pair<string, int> (s_value, value)); // map
                    } else { // failed to insert -- it is an old value
                        value = map_string_values_numbers[i][s_value];
                    }
                } else { // if no variable contains the string value
                    value = stoi(parsed_sample[i]); // the value of the variable
                }
                v.SetInt(value);
                // insert the value of one variable the mapped number of one variable of one sample
                map_disc_vars_possible_values[i].insert(value);
            } else { //the label is continuous (i.e., regression task)
                float value = stof(parsed_sample[i]); // the value of the variable
                v.SetFloat(value);
            }
            VarVal var_value(i, v);
            single_sample_vector.push_back(var_value);
        }
        vector_dataset_all_vars.push_back(single_sample_vector);
        getline(in_file, sample);
        sample = TrimRight(sample);
    }

    // handle the last sample: the same way for other samples -- copy from the while-loop
    vector<string> parsed_sample = Split(sample, ",");
    vector<VarVal> single_sample_vector;
    for (int i = 0; i < num_vars; ++i) {
        Value v;
        int value;
        // check whether the variable is continuous
        if (cont_vars.find(i) == cont_vars.end()) { //the label is discrete (i.e., classification task)
            if (str_val) { // if some discrete variables contain string values
                string s_value = parsed_sample[i];
                // insert successfully -- it is a new possible value
                if (map_disc_vars_string_values[i].insert(s_value).second) {
                    value = ++counter[i]; // get a new int value
                    map_string_values_numbers[i].insert(pair<string, int> (s_value, value)); // map
                } else { // failed to insert -- it is an old value
                    value = map_string_values_numbers[i][s_value];
                }
            } else { // if no variable contains the string value
                value = stoi(parsed_sample[i]); // the value of the variable
            }
            v.SetInt(value);
            // insert the value of one variable the mapped number of one variable of one sample
            map_disc_vars_possible_values[i].insert(value);
        } else { //the label is continuous (i.e., regression task)
            float value = stof(parsed_sample[i]); // the value of the variable
            v.SetFloat(value);
        }
        VarVal var_value(i, v);
        single_sample_vector.push_back(var_value);
    }
    vector_dataset_all_vars.push_back(single_sample_vector);

    num_instance = vector_dataset_all_vars.size();

    for (int i = 0; i < num_vars; ++i) {
        num_of_possible_values_of_disc_vars.push_back(map_disc_vars_possible_values[i].size());
        is_vars_discrete.push_back(cont_vars.find(i)==cont_vars.end());
    }

    /**
     * 3, convert vector "vector_dataset_all_vars" into array "dataset_all_vars"
     * (does not erase "vector_dataset_all_vars").
     */
    if (cont_vars.empty()) {
        Vector2IntArray();
        RowMajor2ColumnMajor();
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

/**
 * @brief: convert from vector ("vector_dataset_all_vars") to array ("dataset_all_vars")
 * this function is used if the data set only contains discrete variables
 */
void Dataset::Vector2IntArray() {//storing the data set using int only
    // Initialize to be all zero. (dataset_all_vars: int **)
    dataset_all_vars = new int* [num_instance];
//  dataset_all_vars = new uint8_t* [num_instance];
//#pragma omp parallel for
    for (int s = 0; s < num_instance; ++s) {
        dataset_all_vars[s] = new int [num_vars]();
        vector<VarVal> vec_instance = vector_dataset_all_vars[s];
        for (const VarVal &vv : vec_instance) {  // For each non-zero-value feature of this sample.
            //change the related value if it is non-zero value in the vector representation.
            dataset_all_vars[s][vv.first] = vv.second.GetInt();
        }
    }
}

/**
 * @brief: convert from row-major storage ("dataset_all_vars") to column-major storage ("dataset_columns")
 * this function is used if the data set only contains discrete variables
 */
void Dataset::RowMajor2ColumnMajor() {
    // Initialize to be all zero. (dataset_columns: int **)
    dataset_columns = new int* [num_vars];
//    dataset_columns = new uint8_t* [num_vars];
//#pragma omp parallel for
    for (int i = 0; i < num_vars; ++i) {
        dataset_columns[i] = new int [num_instance];
//        dataset_columns = new uint8_t* [num_vars];
        for (int j = 0; j < num_instance; ++j) {
            dataset_columns[i][j] = dataset_all_vars[j][i];
        }
    }
}
