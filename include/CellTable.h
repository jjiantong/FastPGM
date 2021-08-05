//
// Created by jjt on 2021/7/2.
//

#ifndef BAYESIANNETWORK_CELLTABLE_H
#define BAYESIANNETWORK_CELLTABLE_H

#include "Dataset.h"
#include <omp.h>

/**
 * two-dimensional contingency table.
 */
class Counts2D {
public:
    int dimx; // first dimension.
    int dimy; // second dimension
    int **n;  // contingency table.
    int *ni;  // marginal counts for the first dimension.
    int *nj;  // marginal counts for the second dimension.
    Counts2D(int llx, int lly);
    ~Counts2D();
};

/**
 * three-dimensional contingency table, as an array of two-dimensional tables
 * spanning the third dimension.
 */
class Counts3D {
public:
    int dimx; // first dimension.
    int dimy; // second dimension
    int dimz; // third dimension.
    int ***n; // contingency table.
    int **ni; // marginal counts for the first dimension.
    int **nj; // marginal counts for the second dimension.
    int *nk;  // marginal counts for the third dimension.
    Counts3D(int llx, int lly, int llz);
    ~Counts3D();
};

/**
 * Important note: rename like Tetrad: "cell" = "configuration"!!
 *
 * storing a cell count table of arbitrary dimension
 * provide methods for incrementing particular cells and for calculating marginals
 */
class CellTable {
public:
    vector<int> cond_dims;
    vector<int> dims;
    vector<int> cond_indices;
    vector<int> indices;

    Counts2D *table_2d;
    Counts3D *table_3d;
    int *configurations;

    CellTable(const vector<int> &dims, const vector<int> &test_index);
    ~CellTable();

    void FastConfig(Dataset *dataset);
    void FillTable3D(Dataset *dataset);
    void FillTable2D(Dataset *dataset);





//    Dataset *dataset; //todo
//    /**
//     * store a copy of config for temporary use (reused)
//     */
//    vector<int> config_copy;
//    /**
//     * key is the cell index (corresponds to one configuration)
//     * value is the counting value of the configuration
//     * the length of map = dims[0] * dims[1] ... * dims[dims.length - 1]
//     */
//    map<int, long> cells;
//
//    /**
//     * the number of cells in the table, = dims[0] * dims[1] ... * dims[dims.length - 1]
//     */
//    int num_cells;
//
//    /**
//     * an array whose length is the number of dimensions of the cell,
//     * and each value dims[i] is the numbers of values for the i'th dimension
//     * each of these dimensions must be an integer greater than zero
//     */
//    vector<int> dims;
//    int num_vars_tested;
//
//    CellTable(const vector<int> &dims);
//
//    void AddToTable(Dataset *dataset, const vector<int> &indices);
//    int inline GetCellIndex(const vector<int> &config);
//    long GetValue(const vector<int> &config);
//    long ComputeMargin(const vector<int> &config);
//    long ComputeMargin(const vector<int> &config, int* margin_vars, int margin_size);
};

#endif //BAYESIANNETWORK_CELLTABLE_H
