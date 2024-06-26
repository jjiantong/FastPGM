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
#include "Network.h"
#include "Node.h"

using namespace std;

class Network;
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

    Dataset(); // constructor for training datasets.
    Dataset(Dataset *dts); // constructor for testing datasets based on training datasets.
    Dataset(Network *net); // constructor for testing datasets based on known networks.
    ~Dataset();

    /**
     * libsvm & csv, traning and testing, all generate:
     *      `num_instance`, `class_var_index`, `vector_dataset_all_vars`
     * training:    training data must be complete data with no missing value.
     *              `num_of_possible_values_of_disc_vars`, `vars_possible_values_ids` are required.
     *              (so although they are meaningless for libsvm, we still need to generate them.)
     *              `num_vars`, `vars_possible_values_ids`, 2 arrays are also required.
     * testing:     copies from training: `num_vars`, `vars_possible_values_ids`.
     * IMPORTANT NOTE: if using libsvm format training data, the testing data can't be the csv format. because we don't
     * store the mappings of value and num in `vars_possible_values_ids` when loading libsvm format training data.
     */
    void LoadLIBSVMTrainingData(string data_file_path, int cls_var_id, set<int> cont_vars={});
    void LoadLIBSVMTestingData(string data_file_path, int cls_var_id, set<int> cont_vars={});
//    void SamplesToLIBSVMFile(vector<DiscreteConfig> &samples, string &file) const;
//    void SamplesToLIBSVMFile(vector<Configuration> &samples, string &file) const;

    void LoadCSVTrainingData(string data_file_path, bool str_val=true, int cls_var_id=-1, set<int> cont_vars={});
    void LoadCSVTestingData(string data_file_path, bool str_val=true, int cls_var_id=-1, set<int> cont_vars={});
    // todo: if using, remove header
//    void SamplesToCSVFile(vector<DiscreteConfig> &samples, string &file, vector<string> header={}) const;
//    void SamplesToCSVFile(vector<Configuration> &samples, string &file, vector<string> header={}) const;

    /**
     * store data file with the libsvm/csv format.
     * libsvm:  the data must be a complete dataset. use `vector_dataset_all_vars` to store.
     * csv:     the data can be complete or incomplete. use `vector_dataset_all_vars` to store.
     */
    void StoreLIBSVMData(string data_file_path, string data_file_path2, set<int> cont_vars={});
    void StoreCSVData(string data_file_path, bool str_val=true, set<int> cont_vars={});


    void Vector2IntArray();
    void RowMajor2ColumnMajor();
};


#endif //BAYESIANNETWORK_DATASET_H
