//
// Created by jjt on 2021/7/2.
//

#ifndef BAYESIANNETWORK_CELLTABLE_H
#define BAYESIANNETWORK_CELLTABLE_H

#include "Dataset.h"
#include <omp.h>

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
    map<int, long> cells;

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

    CellTable(const vector<int> &dims);

    void AddToTable(Dataset *dataset, int* indices, int size);
    int inline GetCellIndex(const vector<int> &config);
    long GetValue(const vector<int> &config);
    long ComputeMargin(const vector<int> &config);
    long ComputeMargin(const vector<int> &config, int* margin_vars, int margin_size);
};

#endif //BAYESIANNETWORK_CELLTABLE_H
