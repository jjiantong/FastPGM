//
// Created by jjt on 2021/7/2.
//

#include "CellTable.h"

/**----------------------------- implementations like bnlearn -----------------------------**/
Counts3D::Counts3D(int llx, int lly, int llz) {
    dimx = llx;
    dimy = lly;
    dimz = llz;

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

Counts2D::Counts2D(int llx, int lly) {
    dimx = llx;
    dimy = lly;

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
 * construct a new cell table using the given (fixed) dimensions
 * each dimension must be an integer greater than zero
 * all cells are reset to zero
 * @param dims: an array of length > 0, each element specifies the number of possible values of that dimension (> 0)
 */
CellTable::CellTable(const vector<int> &dims, const vector<int> &test_index) {
    this->dims = dims;
    this->indices = test_index;
    if (this->dims.size() > 2) { // conditioning set is not empty
        // dimensions and indices of z1, z2, ...; copy from dims and indices
        for (int i = 2; i < this->dims.size(); ++i) {
            cond_dims.push_back(this->dims[i]);
            cond_indices.push_back(indices[i]);
        }

        // compute the number of possible configurations of z
        int num_cells = 1;
        for (int i = 0; i < cond_dims.size(); ++i) {
            num_cells *= cond_dims[i];
        }
        table_3d = new Counts3D(dims[0], dims[1], num_cells);

    } else { // conditioning set is empty
        table_2d = new Counts2D(dims[0], dims[1]);
    }
}

CellTable::~CellTable() {
    if (indices.size() > 2) {
        delete table_3d;
        table_3d = nullptr;
        delete [] configurations;
        configurations = nullptr;
    } else {
        delete table_2d;
        table_2d = nullptr;
    }
}

/**
 * @brief: get all configurations of the conditioning set z
 * @example: 3 features, dims = {2, 3, 2}
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
void CellTable::FastConfig(Dataset *dataset) {
    configurations = new int[dataset->num_instance];
//    omp_set_num_threads(8);
//#pragma omp parallel for
    for (int i = 0; i < dataset->num_instance; ++i) {
        int cell_index = 0;
        for (int j = 0; j < cond_dims.size(); ++j) {
            cell_index *= cond_dims[j];
//            cell_index += dataset->dataset_all_vars[i][cond_indices[j]];
            cell_index += dataset->dataset_columns[cond_indices[j]][i];
        }
        configurations[i] = cell_index;
    }
}

/**
 * initialize a three-dimensional contingency table and the marginals.
 */
void CellTable::FillTable3D(Dataset *dataset, Timer *timer) {
    /**
     * compute the joint frequency of x, y, and z (Nxyz)
     */
    timer->Start("count");
    for (int k = 0; k < dataset->num_instance; ++k) {
//        int x = dataset->dataset_all_vars[k][indices[0]];
//        int y = dataset->dataset_all_vars[k][indices[1]];
        int x = dataset->dataset_columns[indices[0]][k];
        int y = dataset->dataset_columns[indices[1]][k];
        int z = configurations[k];
        table_3d->n[z][x][y]++;
    }
    timer->Stop("count");

    /**
     * compute the marginals (Nx+z, N+yz, N++z)
     */
    timer->Start("marginals");
    for (int k = 0; k < table_3d->dimz; k++) {
        for (int i = 0; i < table_3d->dimx; i++) {
            for (int j = 0; j < table_3d->dimy; j++) {
                table_3d->ni[k][i] += table_3d->n[k][i][j];
                table_3d->nj[k][j] += table_3d->n[k][i][j];
                table_3d->nk[k] += table_3d->n[k][i][j];
            }
        }
    }
    timer->Stop("marginals");
}

void CellTable::FillTable2D(Dataset *dataset, Timer *timer) {
    /**
     * compute the joint frequency of x, y, and z (Nxyz)
     */
    timer->Start("count");
    for (int k = 0; k < dataset->num_instance; ++k) {
//        int x = dataset->dataset_all_vars[k][indices[0]];
//        int y = dataset->dataset_all_vars[k][indices[1]];
        int x = dataset->dataset_columns[indices[0]][k];
        int y = dataset->dataset_columns[indices[1]][k];
        table_2d->n[x][y]++;
    }
    timer->Stop("count");

    /**
     * compute the marginals (Nx+, N+y)
     */
    timer->Start("marginals");
    for (int i = 0; i < table_2d->dimx; i++) {
        for (int j = 0; j < table_2d->dimy; j++) {
            table_2d->ni[i] += table_2d->n[i][j];
            table_2d->nj[j] += table_2d->n[i][j];
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
////            coords.at(j) = dataset->vector_dataset_all_vars.at(i).at(indices.at(j)).second.GetInt();
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