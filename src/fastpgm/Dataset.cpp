#pragma clang diagnostic push
#pragma ide diagnostic ignored "openmp-use-default-none"

#include "fastpgm/Dataset.h"


Dataset::Dataset() {
    dataset_all_vars = nullptr;
    dataset_columns = nullptr;
}

/**
 * construct a dataset object from a known object, used to construct the tester from the trainer.
 */
Dataset::Dataset(Dataset* dts): vars_possible_values_ids(dts->vars_possible_values_ids), num_vars(dts->num_vars) {
    dataset_all_vars = nullptr;
    dataset_columns = nullptr;
}

/**
 * construct a dataset object from a network object, used to construct the tester from a known network.
 */
Dataset::Dataset(Network* net): num_vars(net->num_nodes) {
    dataset_all_vars = nullptr;
    dataset_columns = nullptr;

    vars_possible_values_ids.resize(num_vars);
    for (int i = 0; i < num_vars; ++i) {
        Node *node = net->FindNodePtrByIndex(i);
        // todo: currently only support discrete nodes
        DiscreteNode *disc_node = (DiscreteNode*)node;
        vars_possible_values_ids[i] = disc_node->possible_values_ids;
    }
}

Dataset::~Dataset() {
    if (dataset_all_vars) {
        for (int i = 0; i < num_instance; ++i) {
            SAFE_DELETE_ARRAY(dataset_all_vars[i]);
        }
        SAFE_DELETE_ARRAY(dataset_all_vars);
    }

    if (dataset_columns) {
        for (int i = 0; i < num_vars; ++i) {
            SAFE_DELETE_ARRAY(dataset_columns[i]);
        }
        SAFE_DELETE_ARRAY(dataset_columns);
    }
}

/**
 * @brief: load training data file with libsvm format
 */
void Dataset::LoadLIBSVMTrainingData(string data_file_path, int cls_var_id, set<int> cont_vars) {

    ifstream in_file;
    in_file.open(data_file_path);
    if (!in_file.is_open()) {
        fprintf(stderr, "Error in function %s!", __FUNCTION__);
        fprintf(stderr, "Unable to open file %s!", data_file_path.c_str());
        exit(1);
    }
//    cout << "Data file opened. Begin to load data. " << endl;

    class_var_index = cls_var_id;

    string sample;
    int max_index_occurred = -1;

    /**
     * 1, read the data file and store with the representation of std::vector.
     */
    while (getline(in_file, sample)) {
        // There is a whitespace at the end of each line of
        // libSVM dataset format, which will cause a bug if we do not trim it.
        sample = TrimRight(sample);

        vector<string> parsed_sample = Split(sample, " ");

        int it = 0;   // id of the label is 0

        vector<VarVal> single_sample_vector; // one instance
        Value v;
        // check whether the label is continuous
        if (cont_vars.find(0) == cont_vars.end()) {
            //the label is not continuous (i.e., classification task)
            int value = stoi(parsed_sample[it]); // the value of label
            v.SetInt(value);
        } else {
            //the label is continuous (i.e., regression task)
            float value = stof(parsed_sample[it]); // the value of label
            v.SetFloat(value);
        }
        VarVal var_value(class_var_index, v);
        single_sample_vector.push_back(var_value);

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
            } else {
                float value = stof(parsed_feature_val[1]);
                v.SetFloat(value);
            }
            VarVal var_value(index, v);
            single_sample_vector.push_back(var_value);
        }
        // vector_dataset_all_vars: vector<vector<VarVal>>; label + feature
        vector_dataset_all_vars.push_back(single_sample_vector);
    }

    num_instance = vector_dataset_all_vars.size();
    num_vars = max_index_occurred + 1;//the number of variables of the data set

    for (int i = 0; i < num_vars; ++i) {
        vec_var_names.push_back(to_string(i)); //the name of a variable is the "id" of the variable.
    }

    /**
     * 2, convert vector `vector_dataset_all_vars` into array `dataset_all_vars` and `dataset_columns` (does not erase
     * `vector_dataset_all_vars`).
     */
    if (cont_vars.empty()) {//the data set only contains discrete variables.
        Vector2IntArray();
        RowMajor2ColumnMajor();
    }

    /**
     * 3, generate `num_of_possible_values_of_disc_vars` and `vars_possible_values_ids`.
     * for the libsvm format, we actually don't need them. however, they are used in structure learning.
     * `num_of_possible_values_of_disc_vars`: find from `dataset_columns` the maximum value for each variable.
     */
    num_of_possible_values_of_disc_vars.resize(num_vars);
    vars_possible_values_ids.resize(num_vars);
    for (int i = 0; i < num_vars; ++i) { // for each variable
        int max_value_occurred = -1;
        for (int j = 0; j < num_instance; ++j) { // for each instance
            // find the value of this variable in each instance
            int value = dataset_columns[i][j];
            max_value_occurred = value > max_value_occurred ? value : max_value_occurred;
        }
        num_of_possible_values_of_disc_vars[i] = max_value_occurred + 1;
    }

    cout << "Finish loading training data. "
         << "Number of instances: " << num_instance << ". "
         << "Number of variables: " << num_vars << ". " << endl;

    in_file.close();
}

void Dataset::LoadLIBSVMTestingData(string data_file_path, int cls_var_id, set<int> cont_vars) {

    ifstream in_file;
    in_file.open(data_file_path);
    if (!in_file.is_open()) {
        fprintf(stderr, "Error in function %s!", __FUNCTION__);
        fprintf(stderr, "Unable to open file %s!", data_file_path.c_str());
        exit(1);
    }
//    cout << "Data file opened. Begin to load data. " << endl;

    class_var_index = cls_var_id;
    string sample;

    // 1, read the data file and store with the representation of std::vector.
    getline(in_file, sample);
    while (!in_file.eof()) {
        // There is a whitespace at the end of each line of
        // libSVM dataset format, which will cause a bug if we do not trim it.
        sample = TrimRight(sample);

        vector<string> parsed_sample = Split(sample, " ");
        vector<VarVal> single_sample_vector(num_vars); // one instance
        Value v;

        // initialization.
        for (int i = 0; i < num_vars; ++i) {
            if (cont_vars.find(i) == cont_vars.end()) {
                v.SetInt(0);
            } else {
                v.SetFloat(0);
            }
            single_sample_vector[i] = make_pair(i, v);
        }

        int it = 0;   // id of the label is 0

        // check whether the label is continuous
        if (cont_vars.find(0) == cont_vars.end()) {
            //the label is not continuous (i.e., classification task)
            int value = stoi(parsed_sample[it]); // the value of label
            v.SetInt(value);
        } else {
            //the label is continuous (i.e., regression task)
            float value = stof(parsed_sample[it]); // the value of label
            v.SetFloat(value);
        }
        VarVal var_value(class_var_index, v);
        single_sample_vector[class_var_index] = var_value;

        for (++it; it < parsed_sample.size(); ++it) {
            // Each element is in the form of `feature_index:feature_value`.
            string feature_val = parsed_sample[it];

            // split the feature index and the feature value using ":"
            vector<string> parsed_feature_val = Split(feature_val, ":");

            int index = stoi(parsed_feature_val[0]);
            if (index >= num_vars) {
                // if it is an unknown feature, just omit it and go to the next feature index-value pair.
                // todo: haven't check
                continue;
            }

            // same as the processing of label.
            // todo: haven't consider the case of unknown values (it may be addressed in the inference?)
            if (cont_vars.find(index) == cont_vars.end()) {
                int value = stoi(parsed_feature_val[1]);
                v.SetInt(value);
            } else {
                float value = stof(parsed_feature_val[1]);
                v.SetFloat(value);
            }
            VarVal var_value(index, v);
            single_sample_vector[index] = var_value;
        }

//        /************** version of not filling 0 (wrong) ***************/
//        vector<VarVal> single_sample_vector; // one instance
//        Value v;
//
//        int it = 0;   // id of the label is 0
//
//        // check whether the label is continuous
//        if (cont_vars.find(0) == cont_vars.end()) {
//            //the label is not continuous (i.e., classification task)
//            int value = stoi(parsed_sample[it]); // the value of label
//            v.SetInt(value);
//        } else {
//            //the label is continuous (i.e., regression task)
//            float value = stof(parsed_sample[it]); // the value of label
//            v.SetFloat(value);
//        }
//        VarVal var_value(class_var_index, v);
//        single_sample_vector.push_back(var_value);
//
//        for (++it; it < parsed_sample.size(); ++it) {
//            // Each element is in the form of `feature_index:feature_value`.
//            string feature_val = parsed_sample[it];
//
//            // split the feature index and the feature value using ":"
//            vector<string> parsed_feature_val = Split(feature_val, ":");
//
//            int index = stoi(parsed_feature_val[0]);
//            if (index >= num_vars) {
//                // if it is an unknown feature, just omit it and go to the next feature index-value pair.
//                // todo: haven't check
//                continue;
//            }
//
//            // same as the processing of label.
//            // todo: haven't consider the case of unknown values (it may be addressed in the inference?)
//            if (cont_vars.find(index) == cont_vars.end()) {
//                int value = stoi(parsed_feature_val[1]);
//                v.SetInt(value);
//            } else {
//                float value = stof(parsed_feature_val[1]);
//                v.SetFloat(value);
//            }
//            VarVal var_value(index, v);
//            single_sample_vector.push_back(var_value);
//        }
//        /************** version of not filling 0 (wrong) ***************/

        vector_dataset_all_vars.push_back(single_sample_vector);

        getline(in_file, sample);
    }

    num_instance = vector_dataset_all_vars.size();

    cout << "Finish loading testing data. "
         << "Number of instances: " << num_instance << ". " << endl;

    in_file.close();
}

/**
 * @brief: store data file with libsvm format, used to tranform another format to the csv format.
 * the dataset should be a complete dataset. here we use `vector_dataset_all_vars`, so the size of each instance should
 * equal `num_vars`.
 * for the libsvm format, if the transformed number of a possible value is 0, then omit storing it.
 */
void Dataset::StoreLIBSVMData(string data_file_path, string data_file_path2, set<int> cont_vars) {
    /**
     * 1, write all the samples into the dataset file.
     * todo: currently we only consider purely discrete cases here.
     */
    ofstream out_file(data_file_path);
    if (!out_file.is_open()) {
        fprintf(stderr, "Error in function %s!", __FUNCTION__);
        fprintf(stderr, "Unable to open file %s!", data_file_path.c_str());
        exit(1);
    }

    for (int i = 0; i < num_instance; ++i) {
        vector<VarVal> vec_instance = vector_dataset_all_vars[i];
        if (vec_instance.size() != num_vars) {
            cout << "A complete dataset is required for storing with the libsvm format. " << endl;
            exit(1);
        }

        string sample;

        // label.
        int label_pos = -1;
        int label_num = -1;
        // find the position of the label and its transformed number.
        for (int j = 0; j < num_vars; ++j) {
            if (vec_instance[j].first == class_var_index) {
                label_pos = j;
                label_num = vec_instance[j].second.GetInt();
                break;
            }
        }
        // remove the label element from `vec_instance`.
        vec_instance.erase(vec_instance.begin() + label_pos);
        sample = to_string(label_num) + " ";

        // features.
        for (int j = 0; j < num_vars - 1; ++j) {
            int var_id = vec_instance[j].first;
            int var_num = vec_instance[j].second.GetInt();

            if (var_num != 0) {
                sample += to_string(var_id) + ":" + to_string(var_num) + " ";
            }
        }

        out_file << sample << endl;
    }

    cout << "Finish storing data with libsvm format. "
         << "Number of instances: " << num_instance << ". "
         << "Number of variables: " << num_vars << "." << endl;
    out_file.close();

    /**
     * 2, store the relationships regarding variables and possible values of variables.
     */
    ofstream out_file2(data_file_path2);
    if (!out_file2.is_open()) {
        fprintf(stderr, "Error in function %s!", __FUNCTION__);
        fprintf(stderr, "Unable to open file %s!", data_file_path2.c_str());
        exit(1);
    }

    for (int i = 0; i < num_vars; ++i) {
        // 1, variable and its number.
        out_file2 << vec_var_names[i] << "=" << i << endl;
        // 2, possible values of the variable and its number.
        map<string, int> val_num = vars_possible_values_ids[i];
        for (const auto &v: val_num) {
            out_file2 << "  " << v.first << "=" << v.second << endl;
        }
    }

    cout << "Finish storing the mappings. " << endl;
    out_file2.close();
}

/**
 * @brief: load data file with csv format (complete dataset), used for loading training datasets.
 * TODO: currently only support complete training data (i.e. no missing value)
 */
void Dataset::LoadCSVTrainingData(string data_file_path, bool str_val, int cls_var_id, set<int> cont_vars) {

    ifstream in_file;
    in_file.open(data_file_path);
    if (!in_file.is_open()) {
        fprintf(stderr, "Error in function %s!", __FUNCTION__);
        fprintf(stderr, "Unable to open file %s!", data_file_path.c_str());
        exit(1);
    }
//    cout << "Data file opened. Begin to load data. " << endl;

    // we need to specify the variable interested for the CSV format
    this->class_var_index = cls_var_id;
    string sample;

    /**
     * 1, read and parse the first line
     * use the first line to detect the number of variables of the data set
     */
    getline(in_file, sample);
    num_vars = count(sample.begin(), sample.end(), ',') + 1;
    num_of_possible_values_of_disc_vars.reserve(num_vars);

    // the first line contains variable names, it is like the table header
    // If there is a whitespace at the end of each line,
    // it will cause a bug if we do not trim it.
    sample = TrimRight(sample);
    vector<string> parsed_variable = Split(sample, ",");

    vec_var_names = parsed_variable;
    set<string> temp;
    temp.insert(parsed_variable.begin(), parsed_variable.end());
    // check whether there are duplicate variable names
    if (temp.size() != parsed_variable.size()) {
        fprintf(stderr, "Error in function [%s]\nDuplicate variable names in header!", __FUNCTION__);
        exit(1);
    }

    getline(in_file, sample);

    /**
     * 2, read the data file and store with the representation of std::vector.
     */
    /**
     * if some discrete variables contain string values (i.e., str_val == true), use:
     *   - vector<int> counter:
     *     size = num_vars. counter is used to map the string values with different numbers.
     *   - vector<map<string, int>> vars_possible_values_ids:
     *     for each variable, use a map to map string values with different ids.
     * VarVal: pair<int, Value>; Value is a struct to support both discrete and continuous cases.
     * for each instance, traverse each variable to get an `var_value`, which is variable id-value pair; push all the
     * pairs to `single_sample_vector` to construct the vector for one instance; push all the vectors to
     * `vector_dataset_all_vars` to record the whole dataset
     */
    map<int, set<string>> map_disc_vars_string_values;
    vector<int> counter(num_vars, -1);
    vars_possible_values_ids.resize(num_vars);

    do { // for all instances
        // If there is a whitespace at the end of each line,
        // it will cause a bug if we do not trim it.
        sample = TrimRight(sample);

        vector<string> parsed_sample = Split(sample, ",");
        vector<VarVal> single_sample_vector;
        for (int i = 0; i < num_vars; ++i) { // for each variable
            Value v;
            int value;
            // check whether the variable is continuous
            if (cont_vars.find(i) == cont_vars.end()) { //the label is discrete (i.e., classification task)
                if (str_val) { // if some discrete variables contain string values
                    string s_value = parsed_sample[i];
                    // insert successfully -- it is a new possible value
                    if (map_disc_vars_string_values[i].insert(s_value).second) {
                        value = ++counter[i]; // get a new int value for the new possible value
                        vars_possible_values_ids[i].insert(pair<string, int> (s_value, value)); // map
                    } else { // failed to insert -- it is an old value
                        value = vars_possible_values_ids[i][s_value];
                    }
                } else { // if no variable contains the string value
                    value = stoi(parsed_sample[i]); // the value of the variable
                }
                v.SetInt(value);
                // insert the value of one variable the mapped number of one variable of one sample
            } else { //the label is continuous (i.e., regression task)
                float value = stof(parsed_sample[i]); // the value of the variable
                v.SetFloat(value);
            }
            VarVal var_value(i, v);
            single_sample_vector.push_back(var_value);
        }
        vector_dataset_all_vars.push_back(single_sample_vector);

    } while (getline(in_file, sample));

    num_instance = vector_dataset_all_vars.size();

    for (int i = 0; i < num_vars; ++i) {
        num_of_possible_values_of_disc_vars.push_back(vars_possible_values_ids[i].size());
    }

    /**
     * 3, convert vector "vector_dataset_all_vars" into array "dataset_all_vars". (`vector_dataset_all_vars` remains.)
     */
    if (cont_vars.empty()) {
        Vector2IntArray();
        RowMajor2ColumnMajor();
    }

    cout << "Finish loading training data. "
         << "Number of instances: " << num_instance << ". "
         << "Number of variables: " << num_vars << "." << endl;

    in_file.close();
}

/**
 * @brief: load data file with csv format (complete or incomplete dataset), used for loading testing datasets.
 * the network is known, so we need all the class variables, e.g., `vars_possible_values_ids`: the mappings of possible
 * values and indexes for the variables. they should be obtained from either the training dataset or a network file.
 * we also assume that the variable ordering in the csv file is correct.
 */
void Dataset::LoadCSVTestingData(string data_file_path, bool str_val, int cls_var_id,
                                 set<int> cont_vars) {
    if (str_val == true && vars_possible_values_ids.empty()) {
        cout << "vars_possible_values_ids is required when loading testing sets." << endl;
        exit(1);
    }

    ifstream in_file;
    in_file.open(data_file_path);
    if (!in_file.is_open()) {
        fprintf(stderr, "Error in function %s!", __FUNCTION__);
        fprintf(stderr, "Unable to open file %s!", data_file_path.c_str());
        exit(1);
    }
//    cout << "Data file opened. Begin to load data. " << endl;

    // we need to specify the variable interested for the CSV format
    this->class_var_index = cls_var_id;
    string sample;

    /**
     * 1, read and parse the first line
     * the first line is the header, do nothing, just getting the next line. because we have parse the header when
     * loading training data.
     */
    getline(in_file, sample);

    getline(in_file, sample);

    /**
     * 2, read the data file and store with the representation of std::vector.
     */
    /**
     * VarVal: pair<int, Value>; Value is a struct to support both discrete and continuous cases.
     * for each instance, traverse each variable to get an `var_value`, which is variable id-value pair; push all the
     * pairs to `single_sample_vector` to construct the vector for one instance; push all the vectors to
     * `vector_dataset_all_vars` to record the whole dataset
     */
    do { // for all instances
        // If there is a whitespace at the end of each line,
        // it will cause a bug if we do not trim it.
        sample = TrimRight(sample);

        vector<string> parsed_sample = Split(sample, ",");
        vector<VarVal> single_sample_vector;
        for (int i = 0; i < num_vars; ++i) { // for each variable
            Value v;
            int value;
            // check whether the variable is continuous
            if (cont_vars.find(i) == cont_vars.end()) { //the label is discrete (i.e., classification task)
                string s_value = parsed_sample[i];
                if (!s_value.empty()) {
                    // since the dataset can be incomplete, s_value can be empty. if s_value is not empty:
                    if (str_val) {
                        // if some discrete variables contain string values, we need to find its transformed number.
                        // this can be done through `vars_possible_values_ids`.
                        if (vars_possible_values_ids[i].find(s_value) != vars_possible_values_ids[i].end()) {
                            // if s_value can be found in `vars_possible_values_ids`
                            value = vars_possible_values_ids[i][s_value];
                        } else {
                            // if s_value can't be found, it means this value doesn't exist in the training dataset.
                            // so we just omit it and go to the next variable.
                            continue;
                        }
                    } else { // if no variable contains the string value
                        value = stoi(parsed_sample[i]); // the value of the variable
                    }
                    v.SetInt(value);
                    // insert the value of one variable the mapped number of one variable of one sample
                } else {
                    // if s_value is empty, it means we don't know the value of this variable. so we just omit it and
                    // go to the next variable.
                    continue;
                }

            } else { //the label is continuous (i.e., regression task)
                float value = stof(parsed_sample[i]); // the value of the variable
                v.SetFloat(value);
            }
            VarVal var_value(i, v);
            single_sample_vector.push_back(var_value);
        }
        vector_dataset_all_vars.push_back(single_sample_vector);

    } while (getline(in_file, sample));

    num_instance = vector_dataset_all_vars.size();

    cout << "Finish loading testing data. "
         << "Number of instances: " << num_instance << ". " << endl;

    in_file.close();
}

/**
 * @brief: store data file with csv format, used to transform another format to the csv format.
 * @requirement: need the class variables, particularly:
 *               1. need `vec_var_names`, the names of the variables in order;
 *               2. need `vars_possible_values_ids`, the mappings of possible values and indexes for the variables.
 *               They should be obtained from either a csv-format dataset or a network file.
 * here we use `vector_dataset_all_vars`. so whether the generated csv dataset is complete or incomplete depends on
 * whether `vector_dataset_all_vars` contains all variables or part of the variables.
 */
void Dataset::StoreCSVData(string data_file_path, bool str_val, set<int> cont_vars) {
    if (vec_var_names.empty() || (str_val == true && vars_possible_values_ids.empty())) {
        cout << "vec_var_names and vars_possible_values_ids are required for the transformation." << endl;
        exit(1);
    }

    if (num_vars != vec_var_names.size()) {
        cout << "more or less variables found than expected." << endl;
        exit(1);
    }

    ofstream out_file(data_file_path);
    if (!out_file.is_open()) {
        fprintf(stderr, "Error in function %s!", __FUNCTION__);
        fprintf(stderr, "Unable to open file %s!", data_file_path.c_str());
        exit(1);
    }

     /**
     * 1, write the first line
     */
    // the first line contains variable names, it is like the table header
    string names;
    for (int i = 0; i < num_vars; ++i) {
        names += vec_var_names[i] + ",";
    }
    names.pop_back();
    out_file << names << endl;

    /**
     * 2, write all the samples
     * todo: currently we only consider purely discrete cases here
     */
    for (int i = 0; i < num_instance; ++i) {
        string sample;
        // initialize the possible value for each variable to empty string.
        vector<string> value(num_vars, "");

        vector<VarVal> vec_instance = vector_dataset_all_vars[i];
        for (const VarVal &vv: vec_instance) { // for each non-zero-value feature of this sample.
            int var_id = vv.first; // the variable id.
            int var_num = vv.second.GetInt(); // the transformed number of the variable.

            if (str_val) {
                // if some discrete variables contain string values, we need to find the original variable value (type
                // string) given its transformed number. this can be done through `vars_possible_values_ids`.
                string key = "";
                for (const auto &val_num: vars_possible_values_ids[var_id]) {
                    if (val_num.second == var_num) {
                        key = val_num.first;
                        break;
                    }
                }
                if (key.empty()) {
                    cout << "can't find the variable number from the corresponding `vector_dataset_all_vars`." << endl;
                    exit(1);
                }

                value[var_id] = key;
            } else {
                // if all the discrete variables use values with integers, then just transform integer to string.
                value[var_id] = to_string(var_num);
            }
        }

        // after obtaining all the values, construct the string of this sample.
        for (int j = 0; j < num_vars; ++j) {
            sample += value[j] + ",";
        }
        sample.pop_back();
        out_file << sample << endl;
    }

    cout << "Finish storing data with csv format. "
         << "Number of instances: " << num_instance << ". "
         << "Number of variables: " << num_vars << "." << endl;

    out_file.close();
}

///**
// * @brief: this function is to save the processed data into file, for ease debugging and also for approximate inference.
// * This function is for conversion only; SamplesToLIBSVMFile(smps, file) does all the file saving.
// * DiscreteConfig: set< pair<int, int> >
// */
//void Dataset::SamplesToLIBSVMFile(vector<DiscreteConfig> &samples, string &file) const {
//  vector<Configuration> smps;
//  for (const auto &samp : samples) { // for each DiscreteConfig samp
//    Configuration cfg;
//    // for each <int, int> pair, store into cfg (pair<int, Value>)
//    for (const auto &p : samp) {
//      VarVal vv; // VarVal: pair<int, Value>
//      vv.first = p.first;
//      vv.second.SetInt(p.second);
//      cfg.insert(vv);
//    }
//    smps.push_back(cfg);//
//  }
//  SamplesToLIBSVMFile(smps, file);
//}
//
///**
// * @brief: the instances in this sample are generated by the learned network.
// * the file has exactly the same format as libsvm.
// * Configuration: set< pair<int, Value> >
// */
//void Dataset::SamplesToLIBSVMFile(vector<Configuration> &samples, string &file) const {
//  FILE *f;
//  f = fopen(file.c_str(), "w");
//
//  for (auto &smp : samples) { // for each Configuration smp
//    string string_to_write = "";
//
//    for (auto &var_val : smp) { // for each <int, Value> pair var_val
//      // The following codes should not use "+=", because the order matters.
//      int var = var_val.first;
//      Value val = var_val.second;
//      if(var == 0) {
//        string label;
//        if (val.UseInt()) {
//          label = to_string(val.GetInt());
//        }
//        else {
//          label = to_string(val.GetFloat());
//        }
//        string_to_write = label + " " + string_to_write;
//      }
//      else {
//        string feature_value = "";
//        if (val.UseInt()) {
//          if (val.GetInt() != 0) { // Because features of LIBSVM format do not record the value of 0
//            feature_value = to_string(var) + ":" + to_string(val.GetInt()) + " ";
//          }
//        }
//        else {
//          if (val.GetFloat() != 0) {
//            feature_value = to_string(var) + ":" + to_string(val.GetFloat()) + " ";
//          }
//        }
//        string_to_write += feature_value;
//      }
//    }
//    fprintf(f, "%s\n", string_to_write.c_str());
//  }
//  fclose(f);
//}
//
///**
// * @brief: similar to SamplesToLIBSVMFile, but the file format is csv.
// */
//void Dataset::SamplesToCSVFile(vector<DiscreteConfig> &samples, string &file, vector<string> header) const {
//  vector<Configuration> smps;
//  for (const auto &samp : samples) {
//    Configuration cfg;
//    for (const auto &p : samp) {
//      VarVal vv;
//      vv.first = p.first;
//      vv.second.SetInt(p.second);
//      cfg.insert(vv);
//    }
//    smps.push_back(cfg);
//  }
//  SamplesToCSVFile(smps, file, header);
//}
//
///**
// * @brief: similar to SamplesToLIBSVMFile, but the file format is csv.
// */
//void Dataset::SamplesToCSVFile(vector<Configuration> &samples, string &file, vector<string> header) const {
//  // Detect configuration.
//  auto &c = samples.front();
//  auto it = c.end();
//  int max_index = (*(--it)).first;
//
//  FILE *f;
//  f = fopen(file.c_str(), "w");
//
//  string first_line = "";   // The first line is like the table head.
//  if (header.empty() || (vec_var_names.size() == max_index+1)) {  // Because index starts at 0.
//    for (const auto &name : vec_var_names) {
//      first_line += name + ',';
//    }
//  } else if (header.size() == max_index+1) {  // Because index starts at 0.
//    for (const auto &name : header) {
//      first_line += name + ',';
//    }
//  } else {
//    fprintf(stderr, "Error in function [%s]!\nInvalid header size.", __FUNCTION__);
//    exit(1);
//  }
//  first_line = first_line.substr(0, first_line.size()-1);   // No comma at the last character.
//  fprintf(f, "%s\n", first_line.c_str());
//
//  for (auto &smp : samples) {
//    string string_to_write = "";
//    for (auto &var_and_val : smp) {
//      if (var_and_val.second.UseInt()) {
//        string_to_write += to_string(var_and_val.second.GetInt()) + ',';
//      } else {
//        string_to_write += to_string(var_and_val.second.GetFloat()) + ',';
//      }
//    }
//    string_to_write = string_to_write.substr(0, string_to_write.size()-1);   // No comma at the last character.
//    fprintf(f, "%s\n", string_to_write.c_str());
//  }
//
//  fclose(f);
//}

/**
 * @brief: convert from vector ("vector_dataset_all_vars") to array ("dataset_all_vars")
 * this function is used if the data set only contains discrete variables. note that we avoid using it due to
 * inefficiency, here we keep it only for comparison purposes.
 */
void Dataset::Vector2IntArray() {//storing the data set using int only
    // Initialize to be all zero. (dataset_all_vars: int **)
    dataset_all_vars = new int* [num_instance];
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
 * @brief: convert from row-major storage ("dataset_all_vars") to column-major storage ("dataset_columns").
 * Reference: "Fast parallel Bayesian network structure learning." (IPDPS 2022).
 * Implements the optimization "Using  a cache-friendly data storage" described in Section 4.3.
 * this function is used if the data set only contains discrete variables. note that we always use `dataset_columns`
 * rather than `dataset_all_vars`.
 */
void Dataset::RowMajor2ColumnMajor() {
    // Initialize to be all zero. (dataset_columns: int **)
    dataset_columns = new int* [num_vars];
    for (int i = 0; i < num_vars; ++i) {
        dataset_columns[i] = new int [num_instance];
        for (int j = 0; j < num_instance; ++j) {
            dataset_columns[i][j] = dataset_all_vars[j][i];
        }
    }
}

