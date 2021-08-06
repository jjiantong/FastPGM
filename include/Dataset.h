//
// Created by Linjian Li on 2018/11/29.
//

#ifndef BAYESIANNETWORK_DATASET_H
#define BAYESIANNETWORK_DATASET_H

#include <string>
#include <set>
#include <vector>
#include <map>
#include <utility>
#include <iostream>
#include <fstream>
#include <cstdio>
#include "gadget.h"

using namespace std;

class Dataset {
public:
  int num_instance;
  int num_vars;
  int class_var_index = -1;  // "-1" for no specified class variable; used to specify which attribute/variable is class label.

  vector<bool> is_vars_discrete;
  vector<string> vec_var_names;

  // it is used if some discrete variables contain string values
  // use a map to map string values with different numbers
  vector<map<string, int>> map_string_values_numbers;
  // key: discrete variable id
  // value: a set of the possible values or mapped numbers of the variable
  map<int,set<int>> map_disc_vars_possible_values;
  vector<int> num_of_possible_values_of_disc_vars;

  int **dataset_all_vars;//a matrix storing the whole data set; [#samples][#features] TODO: the matrix is used; need to be replaced to improve quality
  int **dataset_columns; // column-major storage; it is the transposed matrix of "dataset_all_vars"
  vector<vector<VarVal>> vector_dataset_all_vars;//a vector storing the whole data set, label + features

  Dataset();
  ~Dataset();

  void LoadLIBSVMData(string data_file_path, set<int> cont_vars={});
  void SamplesToLIBSVMFile(vector<DiscreteConfig> &samples, string &file) const;
  void SamplesToLIBSVMFile(vector<Configuration> &samples, string &file) const;

  void LoadCSVData(string data_file_path, bool header=true, bool str_val=true, int cls_var_id=-1, set<int> cont_vars={});
  void SamplesToCSVFile(vector<DiscreteConfig> &samples, string &file, vector<string> header={}) const;
  void SamplesToCSVFile(vector<Configuration> &samples, string &file, vector<string> header={}) const;

  void Vector2IntArray();
  void RowMajor2ColumnMajor();
};


#endif //BAYESIANNETWORK_DATASET_H
