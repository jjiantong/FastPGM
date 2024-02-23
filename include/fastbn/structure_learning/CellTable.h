//
// Created by jjt on 2021/7/2.
//

#ifndef BAYESIANNETWORK_CELLTABLE_H
#define BAYESIANNETWORK_CELLTABLE_H

#include "fastbn/Dataset.h"
#include "fastbn/Timer.h"
#include <omp.h>
#include <stdint.h>

/**----------------------------- implementations like bnlearn -----------------------------**/
/**
 * three-dimensional contingency table
 * @brief: this class is used for storing the counting of arbitrary dimension
 * provide a method for mapping each configuration of Z to a configuration index (an int)
 *                  and counting the counts for each configuration of X, Y and Z (N_{xyz})
 *                     (storing in the contingency table n[configuration index of Z][value of X][value of Y])
 *                  and generating the marginal counts from the contingency table (N_{+yz}, N_{x+z}, N_{++z})
 */
class Counts3D {
public:
    vector<int> cond_indices;
    int indexx;
    int indexy;

    vector<int> cond_dims;
    int dimx; // first dimension.
    int dimy; // second dimension
    int dimz; // third dimension.
    int *cum_levels;

    int *n; // contingency table.
    int *ni; // marginal counts for the first dimension.
    int *nj; // marginal counts for the second dimension.
    int *nk;  // marginal counts for the third dimension.

    Counts3D(int dimx, int dimy, int indexx, int indexy,
             const vector<int> &cond_dims, const vector<int> &cond_indices);
    ~Counts3D();

    // column-major vs. row-major problem in the following method:
    // https://stackoverflow.com/questions/68683273/access-efficiency-of-c-2d-array
    void FillTable(Dataset *dataset, Timer *timer);
    void CountLevel1(Dataset *dataset);
    void CountLevelN(Dataset *dataset);
};

/**
 * two-dimensional contingency table.
 */
class Counts2D {
public:
    int indexx;
    int indexy;
    int dimx; // first dimension.
    int dimy; // second dimension
    int *n;  // contingency table.
    int *ni;  // marginal counts for the first dimension.
    int *nj;  // marginal counts for the second dimension.

    Counts2D(int dimx, int dimy, int indexx, int indexy);
    ~Counts2D();

    void FillTable(Dataset *dataset, Timer *timer);
};

class Counts3DGroup {
public:
    vector<int> cond_indices;
    int indexx;
    int indexy;

    vector<int> cond_dims;
    int dimx; // first dimension.
    int dimy; // second dimension
    vector<int> dimz;
    int *cum_dims;

    int c_depth;
    int num_ci_tests;
    int *cum_levels;

    int *n; // contingency table.
    int *ni; // marginal counts for the first dimension.
    int *nj; // marginal counts for the second dimension.
    int *nk;  // marginal counts for the third dimension.

    Counts3DGroup(int dimx, int dimy, int indexx, int indexy,
                  const vector<int> &cond_dims, const vector<int> &cond_indices, int group_size);
    ~Counts3DGroup();

    void FillTableGroup(Dataset *dataset, int group_size, Timer *timer);
    void CountLevel1(Dataset *dataset, int group_size);
    void CountLevelN(Dataset *dataset, int group_size);
};

//int Is_exist (int item, const vector<int> &list);

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
