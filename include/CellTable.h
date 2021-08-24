//
// Created by jjt on 2021/7/2.
//

#ifndef BAYESIANNETWORK_CELLTABLE_H
#define BAYESIANNETWORK_CELLTABLE_H

#include "Dataset.h"
#include "Timer.h"
#include <omp.h>

/**----------------------------- implementations like bnlearn -----------------------------**/
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
 * @brief: this class is used for storing the counting of arbitrary dimension
 * provide methods for mapping each configuration of Z to a configuration index (an int)
 *             and for counting the counts for each configuration of X, Y and Z (N_{xyz})
 *                     (storing in the contingency table table_3d->n[configuration index of Z][value of X][value of Y])
 *             and for generating the marginal counts from the contingency table (N_{+yz}, N_{x+z}, N_{++z})
 * also, N_{xy}, N_{x_}, N_{+y} for the case of tests X and Y given empty conditioning set (N_{++} = # of instances)
 */
class CellTable {
public:
    vector<int> cond_dims;
    vector<int> cond_indices;
    vector<int> indices;

    Counts2D *table_2d;
    Counts3D *table_3d;

    CellTable(const vector<int> &dims, const vector<int> &test_index);
    ~CellTable();

    // column-major vs. row-major problem in the following 3 methods:
    // https://stackoverflow.com/questions/68683273/access-efficiency-of-c-2d-array
//    void FastConfig(Dataset *dataset);
    void FillTable3D(Dataset *dataset, Timer *timer);
    void FillTable2D(Dataset *dataset, Timer *timer);
};
/**----------------------------- implementations like bnlearn -----------------------------**/

/**----------------------------- implementations like Tetrad -----------------------------**/
//class CellTable {
//public:
//    Dataset *dataset; // todo: remove, no-use
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
//};
/**----------------------------- implementations like Tetrad -----------------------------**/

#endif //BAYESIANNETWORK_CELLTABLE_H
