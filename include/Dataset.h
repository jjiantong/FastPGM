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

typedef set< pair<int, int> > Combination;

class Dataset {

 public:
  int num_instance;
  int num_vars;
  int class_var_index = -1;  // "-1" for no specified class variable.

  map<int,set<int>> map_vars_possible_values;
  int* num_of_possible_values_of_vars;
  bool* is_vars_discrete;
  int **dataset_all_vars;
  vector<vector<pair<int,int>>> vector_dataset_all_vars;


  Dataset();
  void LoadLIBSVMDataAutoDetectConfig(string data_file_path);
  void ConvertLIBSVMVectorDatasetIntoArrayDataset();
  void SamplesToLIBSVMFile(vector<Combination> &samples, string &file) const;

  void LoadCSVDataAutoDetectConfig(string data_file_path);
  void ConvertCSVVectorDatasetIntoArrayDataset();
  void SamplesToCSVFile(vector<Combination> &samples, string &file) const;
};


#endif //BAYESIANNETWORK_DATASET_H
