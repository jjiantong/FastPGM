#ifndef BAYESIANNETWORK_DATASET_H
#define BAYESIANNETWORK_DATASET_H

#include <string>
#include <set>
#include <vector>
#include <map>
#include <algorithm>
#include <utility>
#include <iostream>
#include <fstream>
#include <cstdio>
#include "common.h"

using namespace std;

class Dataset {
public:
    int num_instance;
    int num_vars;
    int class_var_index = -1;  // "-1" for no specified class variable; used to specify which attribute/variable is class label.

    vector<string> vec_var_names;

    vector<map<string, int>> vars_possible_values_ids;
    vector<int> num_of_possible_values_of_disc_vars;

    int **dataset_all_vars;//a matrix storing the whole data set; [#samples][#features]
    int **dataset_columns; // column-major storage; it is the transposed matrix of "dataset_all_vars"
    vector<vector<VarVal>> vector_dataset_all_vars;//a vector storing the whole data set, label + features

    Dataset();
    Dataset(Dataset *dts);
    ~Dataset();

    void LoadLIBSVMTrainingData(string data_file_path, int cls_var_id, set<int> cont_vars={});
    void LoadLIBSVMTestingData(string data_file_path, int num_nodes, int cls_var_id, set<int> cont_vars={});
    void SamplesToLIBSVMFile(vector<DiscreteConfig> &samples, string &file) const;
    void SamplesToLIBSVMFile(vector<Configuration> &samples, string &file) const;

    void LoadCSVTrainingData(string data_file_path, bool header=true, bool str_val=true, int cls_var_id=-1, set<int> cont_vars={});
    void LoadCSVTestingData(string data_file_path, bool header=true, bool str_val=true, int cls_var_id=-1, set<int> cont_vars={});
    void StoreCSVData(string data_file_path, bool header=true, bool str_val=true, set<int> cont_vars={});
    void SamplesToCSVFile(vector<DiscreteConfig> &samples, string &file, vector<string> header={}) const;
    void SamplesToCSVFile(vector<Configuration> &samples, string &file, vector<string> header={}) const;

    void Vector2IntArray();
    void RowMajor2ColumnMajor();
};


#endif //BAYESIANNETWORK_DATASET_H
