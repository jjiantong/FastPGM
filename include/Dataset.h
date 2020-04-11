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

  map<int,set<int>> map_disc_vars_possible_values;//[key,value]: key is the variable id; value is a set of the possible values of the variable.
  vector<int> num_of_possible_values_of_disc_vars;
  vector<bool> is_vars_discrete;
  vector<string> vec_var_names;

  int **dataset_all_vars;//a matrix storing the whole data set (need to be deleted)
  vector<vector<VarVal>> vector_dataset_all_vars;//a matrix storing the whole data set


  Dataset();
  void LoadLIBSVMDataAutoDetectConfig(string data_file_path, set<int> cont_vars={});
  void ConvertLIBSVMVectorDatasetIntoIntArrayDataset();
  void SamplesToLIBSVMFile(vector<DiscreteConfig> &samples, string &file) const;
  void SamplesToLIBSVMFile(vector<Configuration> &samples, string &file) const;

  void LoadCSVDataAutoDetectConfig(string data_file_path, bool header=true, int cls_var_id=-1, set<int> cont_vars={});
  void ConvertCSVVectorDatasetIntoIntArrayDataset();
  void SamplesToCSVFile(vector<DiscreteConfig> &samples, string &file, vector<string> header={}) const;
  void SamplesToCSVFile(vector<Configuration> &samples, string &file, vector<string> header={}) const;
};


#endif //BAYESIANNETWORK_DATASET_H
