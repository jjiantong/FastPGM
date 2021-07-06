//
// Created by jjt on 2021/7/2.
//

#ifndef BAYESIANNETWORK_CELLTABLE_H
#define BAYESIANNETWORK_CELLTABLE_H

#include "Dataset.h"

/**
 * Important note: rename like Tetrad: "cell" = "configuration"!!
 *
 * storing a cell count table of arbitrary dimension
 * provide methods for incrementing particular cells and for calculating marginals
 */
class CellTable {
public:
    Dataset *dataset;
    /**
     * store a copy of config for temporary use (reused)
     */
    vector<int> config_copy;
    /**
     * key is the cell index (corresponds to one configuration)
     * value is the counting value of the configuration
     * the length of map = dims[0] * dims[1] ... * dims[dims.length - 1]
     */
    map<int, int> cells;

    /**
     * the number of cells in the table, = dims[0] * dims[1] ... * dims[dims.length - 1]
     */
    int num_cells;

    /**
     * an array whose length is the number of dimensions of the cell,
     * and each value dims[i] is the numbers of values for the i'th dimension
     * each of these dimensions must be an integer greater than zero
     */
    vector<int> dims;
    int num_vars_tested;

    CellTable(){};
    CellTable(vector<int> dims);

    void Reset(vector<int> dims);
    void AddToTable(Dataset *dataset, vector<int> indices);
    int Increment(vector<int> config, int value);
    int GetCellIndex(vector<int> config);
    int GetValue(vector<int> config);
    int ComputeMargin(vector<int> config);
    int ComputeMargin(vector<int> config, vector<int> margin_vars);
};

#endif //BAYESIANNETWORK_CELLTABLE_H
