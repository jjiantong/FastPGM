//
// Created by jjt on 2021/7/2.
//

#include "CellTable.h"

/**----------------------------- implementations like bnlearn -----------------------------**/
/**
 * construct a new counting table using the given (fixed) dimensions
 * each dimension must be an integer greater than zero
 * all cells are reset to zero
 * @param dims: an array of length > 0, each element specifies the number of possible values of that dimension (> 0)
 */
Counts3D::Counts3D(int dimx, int dimy, int indexx, int indexy,
                   const vector<int> &cond_dims, const vector<int> &cond_indices) {
    this->indexx = indexx;
    this->indexy = indexy;
    this->dimx   = dimx;
    this->dimy   = dimy;
    this->cond_indices = cond_indices;
    this->cond_dims    = cond_dims;

//    // compute the number of possible configurations of z
//    int num_cells = 1;
//    for (int i = 0; i < cond_dims.size(); ++i) {
//        num_cells *= cond_dims[i];
//    }
//    dimz = num_cells;

    if (cond_dims.size() > 1) {
        cum_levels = new int[cond_dims.size() - 1];
        // set the right-most one ... (in fact omit the right-most one which is 1)
        cum_levels[cond_dims.size() - 2] = cond_dims[cond_dims.size() - 1];
        // ... then compute the left ones
        for (int i = cond_dims.size() - 3; i >= 0; --i) {
            cum_levels[i] = cum_levels[i + 1] * cond_dims[i + 1];
        }
        // compute the number of possible configurations of z
        dimz = cum_levels[0] * cond_dims[0];
    } else {
        dimz = cond_dims[0];
    }

    n = new int**[dimz];
    for (int i = 0; i < dimz; ++i) {
        n[i] = new int*[dimx];
        for (int j = 0; j < dimx; ++j) {
            n[i][j] = new int[dimy];
            for (int k = 0; k < dimy; ++k) {
                n[i][j][k] = 0;
            }
        }
    }

    ni = new int*[dimz];
    for (int i = 0; i < dimz; ++i) {
        ni[i] = new int[dimx];
        for (int j = 0; j < dimx; ++j) {
            ni[i][j] = 0;
        }
    }

    nj = new int*[dimz];
    for (int i = 0; i < dimz; ++i) {
        nj[i] = new int[dimy];
        for (int j = 0; j < dimy; ++j) {
            nj[i][j] = 0;
        }
    }

    nk = new int[dimz];
    for (int i = 0; i < dimz; ++i) {
        nk[i] = 0;
    }
}

Counts3D::~Counts3D() {
    if (cond_dims.size() > 1) {
        delete[] cum_levels;
        cum_levels = nullptr;
    }

    for (int i = 0; i < dimz; i++) {
        for (int j = 0; j < dimx; j++) {
            delete[] n[i][j];
            n[i][j] = nullptr;
        }
        delete[] n[i];
        n[i] = nullptr;
    }
    delete[] n;
    n = nullptr;

    for (int i = 0; i < dimz; ++i) {
        delete [] ni[i];
        ni[i] = nullptr;
    }
    delete[] ni;
    ni = nullptr;

    for (int i = 0; i < dimz; ++i) {
        delete [] nj[i];
        nj[i] = nullptr;
    }
    delete[] nj;
    nj = nullptr;

    delete[] nk;
    nk = nullptr;
}

Counts3DGroup::Counts3DGroup(int dimx, int dimy, int indexx, int indexy,
                             const vector<int> &cond_dims, const vector<int> &cond_indices, int group_size) {
    this->indexx = indexx;
    this->indexy = indexy;
    this->dimx   = dimx;
    this->dimy   = dimy;
    this->cond_indices = cond_indices; // group
    this->cond_dims    = cond_dims;    // group
    this->c_depth    = cond_dims.size() / group_size;
    this->group_size = group_size;

    dimz.reserve(group_size);
    if (c_depth == 1) {
        dimz = cond_dims;
    } else {
        cum_levels = new int[cond_dims.size()];
        for (int j = 0; j < group_size; ++j) { // for each group
            // set the right-most one of each group to 1 ...
            cum_levels[(j + 1) * c_depth - 1] = 1;
            // ... then compute the left ones
            for (int i = (j + 1) * c_depth - 2; i >= j * c_depth; --i) {
                cum_levels[i] = cum_levels[i + 1] * cond_dims[i + 1];
            }
            // compute the number of possible configurations of z
//            dimz[j] = cum_levels[j * c_depth] * cond_dims[j * c_depth];
            dimz.push_back(cum_levels[j * c_depth] * cond_dims[j * c_depth]);
        }
    }

    n = new int*[group_size];
    for (int m = 0; m < group_size; ++m) {
        n[m] = new int[dimz[m] * dimx * dimy];
        for (int i = 0; i < dimz[m] * dimx * dimy; ++i) {
            n[m][i] = 0;
        }
    }

    ni = new int*[group_size];
    for (int m = 0; m < group_size; ++m) {
        ni[m] = new int[dimz[m] * dimx];
        for (int i = 0; i < dimz[m] * dimx; ++i) {
            ni[m][i] = 0;
        }
    }

    nj = new int*[group_size];
    for (int m = 0; m < group_size; ++m) {
        nj[m] = new int[dimz[m] * dimy];
        for (int i = 0; i < dimz[m] * dimy; ++i) {
            nj[m][i] = 0;
        }
    }

    nk = new int*[group_size];
    for (int m = 0; m < group_size; ++m) {
        nk[m] = new int[dimz[m]];
        for (int i = 0; i < dimz[m]; ++i) {
            nk[m][i] = 0;
        }
    }
}

Counts3DGroup::~Counts3DGroup() {
    if (c_depth > 1) {
        delete[] cum_levels;
        cum_levels = nullptr;
    }

    for (int m = 0; m < group_size; ++m) {
        delete[] n[m];
        n[m] = nullptr;
    }
    delete[] n;
    n = nullptr;

    for (int m = 0; m < group_size; ++m) {
        delete[] ni[m];
        ni[m] = nullptr;
    }
    delete[] ni;
    ni = nullptr;

    for (int m = 0; m < group_size; ++m) {
        delete[] nj[m];
        nj[m] = nullptr;
    }
    delete[] nj;
    nj = nullptr;

    for (int m = 0; m < group_size; ++m) {
        delete[] nk[m];
        nk[m] = nullptr;
    }
    delete[] nk;
    nk = nullptr;
}

Counts2D::Counts2D(int dimx, int dimy, int indexx, int indexy) {
    this->indexx = indexx;
    this->indexy = indexy;
    this->dimx   = dimx;
    this->dimy   = dimy;

    n = new int*[dimx];
    for (int i = 0; i < dimx; ++i) {
        n[i] = new int[dimy];
        for (int j = 0; j < dimy; ++j) {
            n[i][j] = 0;
        }
    }

    ni = new int[dimx];
    for (int i = 0; i < dimx; ++i) {
        ni[i] = 0;
    }

    nj = new int[dimy];
    for (int i = 0; i < dimy; ++i) {
        nj[i] = 0;
    }
}

Counts2D::~Counts2D() {
    for (int i = 0; i < dimx; ++i) {
        delete [] n[i];
        n[i] = nullptr;
    }
    delete[] n;
    n = nullptr;

    delete[] ni;
    ni = nullptr;

    delete[] nj;
    nj = nullptr;
}

/**
 * @brief: 1) get a mapped index z of each conditioning set z1, z2 ..., then
 *         2) count and generate a three-dimensional contingency table and the marginals
 * @example of 1): 3 features, dims = {2, 3, 2}
 *  cell table: 0 0 0 (row 0)
 *              0 0 1
 *              0 1 0
 *              0 1 1
 *              0 2 0
 *              0 2 1
 *              1 0 0
 *              1 0 1
 *              1 1 0
 *              1 1 1 (row 9)
 *              1 2 0
 *              1 2 1 (row 11 = 2 * 3 * 2 - 1)
 *  given config (x, y, z), the cell index should be (((0 * 2) + x) * 3 + y) * 2 + z
 *  given config (1, 1, 1), the cell index = (((0 * 2) + 1) * 3 + 1) * 2 + 1 = 9
 */
void Counts3D::FillTable(Dataset *dataset, Timer *timer) {
    /**
     * compute the joint frequency of x, y, and z (Nxyz)
     */
    timer->Start("config + count");

    for (int k = 0; k < dataset->num_instance; ++k) {
//        int x = dataset->dataset_all_vars[k][indices[0]];
//        int y = dataset->dataset_all_vars[k][indices[1]];
        int x = dataset->dataset_columns[indexx][k];
        int y = dataset->dataset_columns[indexy][k];
        /**
         * map z1, z2 ... to z
         */
        int z = dataset->dataset_columns[cond_indices[cond_dims.size() - 1]][k];
//#pragma omp simd reduction(+:z)
        for (int j = 0; j < cond_dims.size() - 1; ++j) {
            z += dataset->dataset_columns[cond_indices[j]][k] * cum_levels[j];
        }

//        int z = dataset->dataset_columns[cond_indices[0]][k];
//        for (int j = 1; j < cond_dims.size(); ++j) {
//            z *= cond_dims[j];
////            z += dataset->dataset_all_vars[k][cond_indices[j]];
//            z += dataset->dataset_columns[cond_indices[j]][k];
//        }
        n[z][x][y]++;
    }
    timer->Stop("config + count");

    /**
     * compute the marginals (Nx+z, N+yz, N++z)
     */
    timer->Start("marginals");
    for (int k = 0; k < dimz; k++) {
        for (int i = 0; i < dimx; i++) {
            for (int j = 0; j < dimy; j++) {
                ni[k][i] += n[k][i][j];
                nj[k][j] += n[k][i][j];
                nk[k] += n[k][i][j];
            }
        }
    }
    timer->Stop("marginals");
}

void Counts3DGroup::FillTableGroup(Dataset *dataset, Timer *timer) {
    timer->Start("config + count");
    /**
     * for the second time, just traverse the partial data set
     * do: 1 config; 2 count
     */
//#pragma omp parallel for num_threads(2)
    for (int i = 0; i < group_size; ++i) { // for each ci test
        for (int k = 0; k < dataset->num_instance; ++k) {
//            int x = dataset->dataset_all_vars[k][indices[0]];
//            int y = dataset->dataset_all_vars[k][indices[1]];
            int x = dataset->dataset_columns[indexx][k];
            int y = dataset->dataset_columns[indexy][k];

            /**
             * map each group of z1, z2 ... to z
             */
            int z = 0;
            if (c_depth == 1) {
//                z = dataset->dataset_all_vars[k][cond_indices[i]];
                z = dataset->dataset_columns[cond_indices[i]][k];
            } else {
                for (int j = 0; j < c_depth; ++j) {
//                    z += dataset->dataset_all_vars[k][cond_indices[i * c_depth + j]] * cum_levels[i * c_depth + j];
                    z += dataset->dataset_columns[cond_indices[i * c_depth + j]][k] * cum_levels[i * c_depth + j];
                }
            }
            n[i][z * dimx * dimy + x * dimy + y]++; //n[i][z][x][y]++;
        }
    }

//    int group_per_thread = 4;
//    int tmp = (group_size + 3) / group_per_thread;
//#pragma omp parallel for num_threads(2)
//    for (int m = 0; m < tmp; ++m) { // for each ci test
//        for (int k = 0; k < dataset->num_instance; ++k) {
////            int x = dataset->dataset_all_vars[k][indices[0]];
////            int y = dataset->dataset_all_vars[k][indices[1]];
//            int x = dataset->dataset_columns[indexx][k];
//            int y = dataset->dataset_columns[indexy][k];
//
//            for (int nn = 0; nn < group_per_thread; ++nn) {
//                if (m * group_per_thread + nn < group_size) {
//                    int z = 0;
//                    if (c_depth == 1) {
////                z = dataset->dataset_all_vars[k][cond_indices[i]];
//                        z = dataset->dataset_columns[cond_indices[m * group_per_thread + nn]][k];
//                    } else {
//                        for (int j = 0; j < c_depth; ++j) {
////                    z += dataset->dataset_all_vars[k][cond_indices[i * c_depth + j]] * cum_levels[i * c_depth + j];
//                            z += dataset->dataset_columns[cond_indices[(m * group_per_thread + nn) * c_depth + j]][k] * cum_levels[(m * group_per_thread + nn) * c_depth + j];
//                        }
//                    }
//                    n[m * group_per_thread + nn][z * dimx * dimy + x * dimy + y]++; //n[i][z][x][y]++;
//                }
//            }
//        }
//    }

    timer->Stop("config + count");

    /**
     * compute the marginals (Nx+z, N+yz, N++z)
     */
    timer->Start("marginals");
    for (int m = 0; m < group_size; ++m) {
        for (int k = 0; k < dimz[m]; k++) {
            for (int i = 0; i < dimx; i++) {
                for (int j = 0; j < dimy; j++) {
                    ni[m][k * dimx + i] += n[m][k * dimx * dimy + i * dimy + j]; //ni[m][k][i] += n[m][k][i][j];
                    nj[m][k * dimy + j] += n[m][k * dimx * dimy + i * dimy + j]; //nj[m][k][j] += n[m][k][i][j];
                    nk[m][k] += n[m][k * dimx * dimy + i * dimy + j]; //nk[m][k] += n[m][k][i][j];
                }
            }
        }
    }
    timer->Stop("marginals");
}

void Counts2D::FillTable(Dataset *dataset, Timer *timer) {
    /**
     * compute the joint frequency of x, y, and z (Nxyz)
     */
    timer->Start("config + count");
    for (int k = 0; k < dataset->num_instance; ++k) {
//        int x = dataset->dataset_all_vars[k][indices[0]];
//        int y = dataset->dataset_all_vars[k][indices[1]];
        int x = dataset->dataset_columns[indexx][k];
        int y = dataset->dataset_columns[indexy][k];
        n[x][y]++;
    }
    timer->Stop("config + count");

    /**
     * compute the marginals (Nx+, N+y)
     */
    timer->Start("marginals");
    for (int i = 0; i < dimx; i++) {
        for (int j = 0; j < dimy; j++) {
            ni[i] += n[i][j];
            nj[j] += n[i][j];
        }
    }
    timer->Stop("marginals");
}
/**----------------------------- implementations like bnlearn -----------------------------**/

/**----------------------------- implementations like Tetrad -----------------------------**/
///**
// * construct a new cell table using the given (fixed) dimensions
// * each dimension must be an integer greater than zero
// * all cells are reset to zero
// * @param dims: an array of length > 0, each element specifies the number of possible values of that dimension (> 0)
// */
//CellTable::CellTable(const vector<int> &dims) {
//    if (!dims.empty()) {
//        num_vars_tested = dims.size();
//
//        // calculate length of cells[] array
//        num_cells = 1;
//        for (int i = 0; i < dims.size(); ++i) {
//            num_cells *= dims[i];
//        }
//
//        // reset cells array
//        for (int i = 0; i < num_cells; ++i) {
//            // set each counting value to 0
//            cells.insert(pair<int, int>(i, 0));
//        }
//
//        // store the dimensions
//        this->dims = dims;
//    }
//}
//
///**
// * construct a new cell for given indices, and traverse the data set to count the occurrence numbers
// * @param indices indices of a subset of features (nodes in the network structure) of the data set
// */
//void CellTable::AddToTable(Dataset *dataset, const vector<int> &indices) {
//    vector<int> config;
//    config.resize(indices.size());
//
////    omp_set_num_threads(2);
////#pragma omp parallel for firstprivate(config) schedule(static, 5000)
//    for (int i = 0; i < dataset->num_instance; ++i) { // for each instance
//        for (int j = 0; j < indices.size(); ++j) { // for each feature in indices
//            config[j] = dataset->dataset_all_vars[i][indices[j]];
////            coords[j] = dataset->vector_dataset_all_vars[i][indices[j]].second.GetInt();
//        }
//        // increment the counting value at the given configuration by 1
//        int cell_index = GetCellIndex(config);
////#pragma omp atomic
//        cells[cell_index]++;
//    }
//}
//
///**
// * @brief: get the cell index (the row of the table) according to a given configuration
// * @example: 3 features, dims = {2, 3, 2}
// *  cell table: 0 0 0 (row 0)
// *              0 0 1
// *              0 1 0
// *              0 1 1
// *              0 2 0
// *              0 2 1
// *              1 0 0
// *              1 0 1
// *              1 1 0
// *              1 1 1 (row 9)
// *              1 2 0
// *              1 2 1 (row 11 = 2 * 3 * 2 - 1)
// *  given config (x, y, z), the cell index should be (((0 * 2) + x) * 3 + y) * 2 + z
// *  given config (1, 1, 1), the cell index = (((0 * 2) + 1) * 3 + 1) * 2 + 1 = 9
// */
//int inline CellTable::GetCellIndex(const vector<int> &config) {
//    int cell_index = 0;
//    for (int i = 0; i < num_vars_tested; ++i) {
//        cell_index *= dims[i];
//        cell_index += config[i];
//    }
//    return cell_index;
//}
//
///**
// * get the count value of the configuration
// */
//long CellTable::GetValue(const vector<int> &config) {
//    return cells[GetCellIndex(config)];
//}
//
///**
// * @brief: calculate a marginal sum for the cell table
// * the variables over which marginal sums should be taken are indicated by placing "-1"
// * in the appropriate positions in the configuration argument
// * @example: to find the margin for v0 = 1, v1 = 3, and v3 = 2,
// *           where the marginal sum ranges over all values of v2 and v4:
// *           the array [1, 3, -1, 2, -1] should be used
// * @param config: a vector of the sort described above.
// * @return the marginal sum specified
// */
//long CellTable::ComputeMargin(const vector<int> &config) {
//    // make a copy of the config vector so that the original is not messed up
//    config_copy = config;
//
//    long sum = 0;
//    int i = -1;
//
//    while (++i < config.size()) { // for each position of the config
//        if (config_copy[i] == -1) { // the variable of position i is to be marginalized
//            for (int j = 0; j < dims[i]; ++j) { // for all possible values of position i
//                config_copy[i] = j; // fill each possible value in position i
//                sum += ComputeMargin(config_copy);
//            }
//            config_copy[i] = -1;
//            return sum;
//        }
//    } // until every variable is specified a value, then we can obtain the counting value and return
//    long ret = GetValue(config_copy);
//    return ret;
//}
//
///**
// * an alternative way to specify a marginal calculation
// * config specifies a particular cell in the table,
// * and margin_vars contains the indices of the variables over which the margin sum should be calculated
// * the sum is over the cell specified by 'coord' and
// * all of the cells which differ from that cell in any of the specified coordinates
// */
//long CellTable::ComputeMargin(const vector<int> &config, int* margin_vars, int margin_size) {
//    // make a copy of the config vector so that the original is not messed up
//    config_copy = config;
//    for (int i = 0; i < margin_size; ++i) {
//        config_copy[margin_vars[i]] = -1;
//    }
//    long ret = ComputeMargin(config_copy);
//    return ret;
//}
/**----------------------------- implementations like Tetrad -----------------------------**/