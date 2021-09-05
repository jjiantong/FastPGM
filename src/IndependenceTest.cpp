//
// Created by jjt on 2021/6/23.
//

#include "IndependenceTest.h"

/**
 * @brief: constructing a test using the given data set and significance level
 * @param dataset: a data set consisting entirely of discrete variables
 * @param alpha: the significance level, usually 0.05
 */
IndependenceTest::IndependenceTest(Dataset *dataset, double alpha) {
    if (alpha < 0.0 || alpha > 1.0) {
        fprintf(stderr, "Function [%s]: Error with incorrect significance level alpha!",
                __FUNCTION__);
        exit(1);
    }

    this->dataset = dataset;
    this->alpha = alpha;
}

IndependenceTest::~IndependenceTest() {
}

/**----------------------------- implementations like bnlearn -----------------------------**/
/**
 * @brief: CI test TEST (x, y | z)
 * @param x_idx the one variable index being compared
 * @param y_idx the second variable index being compared
 * @param z the set of conditioning variables
 * @param metric the conditional independence test method
 * @return CI test result, including g-square statistic, degree of freedom, p value, independent judgement
 */
IndependenceTest::Result IndependenceTest::IndependenceResult(int x_idx, int y_idx, const vector<int> &z,
                                                              string metric, Timer *timer, int group_size) {
//    /**
//     * for testing x, y given z1,...,zn,
//     * set up an array of length n + 2 containing the indices of these variables in order
//     */
//    vector<int> test_idx;
//    test_idx.push_back(x_idx);
//    test_idx.push_back(y_idx);
//    for (const auto &z_idx : z) {
//        test_idx.push_back(z_idx);
//    }

    if (metric.compare("g square") == 0) {
        if (z.empty()) {
            return ComputeGSquareXY(x_idx, y_idx, timer);
        } else if (group_size > 1) {
            return ComputeGSquareXYZGroup(x_idx, y_idx, z, group_size, timer);
        } else {
            return ComputeGSquareXYZ(x_idx, y_idx, z, timer);
        }
    } else if (metric.compare("mutual information") == 0) {}
    else {}
}

/**
 * calculate g square for ci-test x _||_ y | z1, z2, ..., for discrete data
 * by a commonly used approach, which also used by bnlearn and Tetrad -- for given test_idx,
 * it first computes the counts by scanning the complete data set to fill up a contingency table / cell table
 */
IndependenceTest::Result IndependenceTest::ComputeGSquareXYZ(int x_idx, int y_idx, const vector<int> &z, Timer *timer) {
    int dimx = dataset->num_of_possible_values_of_disc_vars[x_idx];
    int dimy = dataset->num_of_possible_values_of_disc_vars[y_idx];

    vector<int> cond_dims;
    cond_dims.reserve(z.size());
    for (const auto &z_idx : z) {
        int dim = dataset->num_of_possible_values_of_disc_vars[z_idx];
        cond_dims.push_back(dim);
    }

    timer->Start("new & delete");
    table_3d = new Counts3D(dimx, dimy, x_idx, y_idx, cond_dims, z);
    timer->Stop("new & delete");

    table_3d->FillTable(dataset, timer);

    /**
     * compute df: two ways are commonly used to compute the degree of freedom
     *      1. |Z| * (|X|-1) * (|Y|-1), where |Z| means # of combinations of Z
     *         so it equals (dim(x)-1) * (dim(y)-1) * dim(z1) * dim(z2) * ...
     *      2. just like 1, but |X| and |Y| only count for non-zero cases
     *         as in Fienberg, The Analysis of Cross-Classified Categorical Data, 2nd Edition, 142
     *         Tetrad uses this way; bnlearn calls this way as "adjusted degree of freedom"
     * bnlearn uses both the ways (test = "mi" and test = "mi-adf")
     * it seems that 2 is more reasonable and it can obtain smaller SHD in practice
     * we also use 2 in our implementation
     */
    timer->Start("g2 & df + p value");
    double g2 = 0.0;
    int df = 0;
    for (int k = 0; k < table_3d->dimz; ++k) { // for each config of z
        int alx = 0;
        int aly = 0;

        for (int i = 0; i < dimx; ++i) {
            alx += (table_3d->ni[k][i] > 0);
        }
        for (int j = 0; j < dimy; ++j) {
            aly += (table_3d->nj[k][j] > 0);
        }

        // ensure the degrees of freedom will not be negative.
        alx = (alx >= 1) ? alx : 1;
        aly = (aly >= 1) ? aly : 1;
        df += (alx - 1) * (aly - 1);

        long total = table_3d->nk[k]; // N_{++z}
        if (total == 0) {
            continue;
        }

        for (int i = 0; i < dimx; ++i) { // for each possible value of x
            long sum_row = table_3d->ni[k][i]; // N_{x+z}
            if (sum_row == 0) {
                continue;
            }

            for (int j = 0; j < dimy; ++j) { // for each possible value of y
                long sum_col = table_3d->nj[k][j]; // N_{+yz}
                long observed = table_3d->n[k][i][j]; // N_{xyz}
                if (sum_col == 0 || observed == 0) {
                    continue;
                }

                double expected = (double)sum_col * (double)sum_row / (double) total; // E_{xyz} = (N_{x+} * N_{+y}) / N_{++}
                g2 += 2.0 * observed * log(observed / expected); // 2 * N_{xy} * log (N_{xy} / E_{xy})
            }
        }
    }

    if (df == 0) { // if df == 0, this is definitely an independent table
        return IndependenceTest::Result(1.0, true, 0);
    }

    // if p < alpha, reject the null hypothesis: dependent
    // if p > alpha, accept the null hypothesis: independent
    double p_value = 1.0 - stats::pchisq(g2, df, false);
    timer->Stop("g2 & df + p value");

    timer->Start("new & delete");
    delete table_3d;
    timer->Stop("new & delete");

    bool indep = (p_value > alpha);
    return IndependenceTest::Result(p_value, indep, 0);
}

/**
 * calculate a group of g square for multiple ci-tests of x, y
 * by a commonly used approach, which also used by bnlearn and Tetrad -- for given test_idx,
 * it first computes the counts by scanning the complete data set to fill up a contingency table / cell table
 * in this function we use uint8 to re-store the partial data set required
 */
IndependenceTest::Result IndependenceTest::ComputeGSquareXYZGroup(int x_idx, int y_idx, const vector<int> &z,
                                                                  int group_size, Timer *timer) {
    int c_depth = z.size() / group_size; // compute which the level is currently

    int dimx = dataset->num_of_possible_values_of_disc_vars[x_idx];
    int dimy = dataset->num_of_possible_values_of_disc_vars[y_idx];

    bool results[8];
    for (int i = 0; i < 8; ++i) {
        results[i] = false;
    }

    vector<int> cond_dims;
    cond_dims.reserve(z.size());
    for (const auto &z_idx : z) {
        int dim = dataset->num_of_possible_values_of_disc_vars[z_idx];
        cond_dims.push_back(dim);
    }

    timer->Start("new & delete");
    table_3d_group = new Counts3DGroup(dimx, dimy, x_idx, y_idx, cond_dims, z, group_size);
    timer->Stop("new & delete");

    table_3d_group->FillTableGroup(dataset, timer);

    timer->Start("g2 & df + p value");
//#pragma omp parallel for num_threads(2) schedule(dynamic)
    for (int m = 0; m < group_size; ++m) {
        /**
         * compute df: two ways are commonly used to compute the degree of freedom
         *      1. |Z| * (|X|-1) * (|Y|-1), where |Z| means # of combinations of Z
         *         so it equals (dim(x)-1) * (dim(y)-1) * dim(z1) * dim(z2) * ...
         *      2. just like 1, but |X| and |Y| only count for non-zero cases
         *         as in Fienberg, The Analysis of Cross-Classified Categorical Data, 2nd Edition, 142
         *         Tetrad uses this way; bnlearn calls this way as "adjusted degree of freedom"
         * bnlearn uses both the ways (test = "mi" and test = "mi-adf")
         * it seems that 2 is more reasonable and it can obtain smaller SHD in practice
         * we also use 2 in our implementation
         */
        double g2 = 0.0;
        int df = 0;

        for (int k = 0; k < table_3d_group->dimz[m]; ++k) { // for each config of z
            int alx = 0;
            int aly = 0;

            for (int i = 0; i < dimx; ++i) {
                alx += (table_3d_group->ni[m][k * dimx + i] > 0); // ni[m][k][i]
            }
            for (int j = 0; j < dimy; ++j) {
                aly += (table_3d_group->nj[m][k * dimy + j] > 0); //nj[m][k][j]
            }

            // ensure the degrees of freedom will not be negative.
            alx = (alx >= 1) ? alx : 1;
            aly = (aly >= 1) ? aly : 1;
            df += (alx - 1) * (aly - 1);

            long total = table_3d_group->nk[m][k]; // N_{++z}
            if (total == 0) {
                continue;
            }

            for (int i = 0; i < dimx; ++i) { // for each possible value of x
                long sum_row = table_3d_group->ni[m][k * dimx + i]; // N_{x+z}
                if (sum_row == 0) {
                    continue;
                }

                for (int j = 0; j < dimy; ++j) { // for each possible value of y
                    long sum_col = table_3d_group->nj[m][k * dimy + j]; // N_{+yz}
                    long observed = table_3d_group->n[m][k * dimx * dimy + i * dimy + j]; // N_{xyz}
                    if (sum_col == 0 || observed == 0) {
                        continue;
                    }

                    double expected = (double)sum_col * (double)sum_row / (double) total; // E_{xyz} = (N_{x+} * N_{+y}) / N_{++}
                    g2 += 2.0 * observed * log(observed / expected); // 2 * N_{xy} * log (N_{xy} / E_{xy})
                }
            }
        }

        if (df == 0) { // if df == 0, this is definitely an independent table
            results[m] = true;
        }

        // if p < alpha, reject the null hypothesis: dependent
        // if p > alpha, accept the null hypothesis: independent
        double p_value = 1.0 - stats::pchisq(g2, df, false);

        bool indep = (p_value > alpha);
        results[m] = indep;

        // TODO: verbose
    }
    timer->Stop("g2 & df + p value");

    timer->Start("new & delete");
    delete table_3d_group;
    timer->Stop("new & delete");

    for (int i = 0; i < group_size; ++i) {
        if (results[i]) { // find the first independent one
            return IndependenceTest::Result(-1, true, i);
        }
    }
    return IndependenceTest::Result(0.0, false);
}

/**
 * calculate g square for ci-test x _||_ y, for discrete data
 * by a commonly used approach, which also used by bnlearn and Tetrad -- for given test_idx,
 * it first computes the counts by scanning the complete data set to fill up a contingency table / cell table
 * @param test_idx: x, y
 */
IndependenceTest::Result IndependenceTest::ComputeGSquareXY(int x_idx, int y_idx, Timer *timer) {
    int dimx = dataset->num_of_possible_values_of_disc_vars[x_idx];
    int dimy = dataset->num_of_possible_values_of_disc_vars[y_idx];

    timer->Start("new & delete");
    table_2d = new Counts2D(dimx, dimy, x_idx, y_idx);
    timer->Stop("new & delete");

    table_2d->FillTable(dataset, timer);

    timer->Start("g2 & df + p value");
    double g2 = 0.0;
    int df = 0;

    int alx = 0;
    int aly = 0;

    for (int i = 0; i < dimx; ++i) {
        alx += (table_2d->ni[i] > 0);
    }
    for (int j = 0; j < dimy; ++j) {
        aly += (table_2d->nj[j] > 0);
    }

    // ensure the degrees of freedom will not be negative.
    alx = (alx >= 1) ? alx : 1;
    aly = (aly >= 1) ? aly : 1;
    df += (alx - 1) * (aly - 1);

    long total = dataset->num_instance; // N_{++}

    for (int i = 0; i < dimx; ++i) { // for each possible value of x
        long sum_row = table_2d->ni[i]; // N_{x+}
        if (sum_row == 0) {
            continue;
        }

        for (int j = 0; j < dimy; ++j) { // for each possible value of y
            long sum_col = table_2d->nj[j]; // N_{+y}
            long observed = table_2d->n[i][j]; // N_{xy}

            if (sum_col == 0 || observed == 0) {
                continue;
            }

            double expected = (double)sum_col * (double)sum_row / (double) total; // E_{xyz} = (N_{x+} * N_{+y}) / N_{++}
            g2 += 2.0 * observed * log(observed / expected); // 2 * N_{xy} * log (N_{xy} / E_{xy})
        }
    }

    if (df == 0) { // if df == 0, this is definitely an independent table
        return IndependenceTest::Result(1.0, true);
    }

    // if p < alpha, reject the null hypothesis: dependent
    // if p > alpha, accept the null hypothesis: independent
    double p_value = 1.0 - stats::pchisq(g2, df, false);
    timer->Stop("g2 & df & p value");

    timer->Start("new & delete");
    delete table_2d;
    timer->Stop("new & delete");

    bool indep = (p_value > alpha);
    return IndependenceTest::Result(p_value, indep);
}
/**----------------------------- implementations like bnlearn -----------------------------**/

/**----------------------------- implementations like Tetrad -----------------------------**/
///**
// * @brief: CI test TEST (x, y | z)
// * @param x_idx the one variable index being compared
// * @param y_idx the second variable index being compared
// * @param z the set of conditioning variables
// * @param metric the conditional independence test method
// * @return CI test result, including g-square statistic, degree of freedom, p value, independent judgement
// */
//IndependenceTest::Result IndependenceTest::IndependenceResult(int x_idx, int y_idx, const set<int> &z, string metric) {
//    /**
//     * for testing x, y given z1,...,zn,
//     * set up an array of length n + 2 containing the indices of these variables in order
//     */
//    vector<int> test_idx;
//    test_idx.push_back(x_idx);
//    test_idx.push_back(y_idx);
//    for (const auto &z_idx : z) {
//        test_idx.push_back(z_idx);
//    }
//
//    if (metric.compare("g square") == 0) {
//        return ComputeGSquare(test_idx);
//    } else if (metric.compare("mutual information") == 0) {}
//    else {}
//}
//
///**
// * calculate g square for ci-test x _||_ y | z1, z2, ..., for discrete data
// * by a commonly used approach, which also used by bnlearn and Tetrad -- for given test_idx,
// * it first computes the counts by scanning the complete data set to fill up a contingency table / cell table
// * @param test_idx: x, y, z1, z2 ...
// */
//IndependenceTest::Result IndependenceTest::ComputeGSquare(const vector<int> &test_idx) {
////    timer->Start("counting1");
//    // reset the cell table for the columns referred to in 'test_idx', do cell coefs for those columns
//    vector<int> dims;
//    for (int i = 0; i < test_idx.size(); ++i) {
//        // get the number of possible values of each feature in indices, from Dataset.num_of_possible_values_of_disc_vars
//        int dim = dataset->num_of_possible_values_of_disc_vars[test_idx[i]];
//        dims.push_back(dim);
//    }
//
//    cell_table = new CellTable(dims);
//    cell_table->AddToTable(dataset, test_idx);
////    timer->Stop("counting1");
//
////    timer->Start("counting2");
//    // indicator vectors to tell the cell table which margins to calculate
//    // for x _||_ y | z1, z2, ..., we want to calculate the margin for x, the margin for y, and the margin for x and y
//    int first_var[1] = {0};
//    int second_var[1] = {1};
//    int both_vars[2] = {0, 1};
//
//    double g2 = 0.0;
//    int df = 0;
//
//    // dimensions of z1, z2, ...; copy from "cell_table->dims", starting from the third element
//    // because "cell_table->dims" contains the dimensions of x, y, z1, z2, ..., then we ignore the first two elements
//    // note that "dims" contains the dimensions of all variables, so we cannot use "dims" here
//    int* cond_dims = new int[test_idx.size() - 2];
//    for (int i = 0; i < test_idx.size() - 2; ++i) {
//        cond_dims[i] = cell_table->dims[i + 2];
//    }
//
//    /**
//     * two ways are commonly used to compute the degree of freedom (df)
//     *      1. |Z| * (|X|-1) * (|Y|-1), where |Z| means # of combinations of Z
//     *         so it equals (dim(x)-1) * (dim(y)-1) * dim(z1) * dim(z2) * ...
//     *      2. just like 1, but |X| and |Y| only count for non-zero cases
//     *         as in Fienberg, The Analysis of Cross-Classified Categorical Data, 2nd Edition, 142
//     *         Tetrad uses this way; bnlearn calls this way as "adjusted degree of freedom"
//     * bnlearn uses both the ways (test = "mi" and test = "mi-adf")
//     * it seems that 2 is more reasonable and it can obtain smaller SHD in practice
//     * we also use 2 in our implementation
//     */
//    vector<int> config;
//    config.resize(test_idx.size());
//    int num_rows = cell_table->dims[0]; // dimension of x
//    int num_cols = cell_table->dims[1]; // dimension of y
//    vector<bool> attested_rows;
//    vector<bool> attested_cols;
//
//    CombinationGenerator cg(cond_dims, test_idx.size() - 2);
//    while (cg.has_next) {
//        vector<int> combination = cg.Next();
//        for (int i = 0; i < test_idx.size() - 2; ++i) {
//            config[i + 2] = combination[i];
//        }
//        attested_rows.assign(num_rows, true);
//        attested_cols.assign(num_cols, true);
//
//        long total = cell_table->ComputeMargin(config, both_vars, 2); // N_{++z}
//
//        double local_g2 = 0.0;
//        vector<double> e;
//        vector<double> o;
//
//        for (int i = 0; i < num_rows; ++i) { // for each possible value of x
//            for (int j = 0; j < num_cols; ++j) { // for each possible value of y
//                config[0] = i;
//                config[1] = j;
//
//                long sum_row = cell_table->ComputeMargin(config, second_var, 1); // N_{x+z}
//                long sum_col = cell_table->ComputeMargin(config, first_var, 1); // N_{+yz}
//                long observed = cell_table->GetValue(config); // N_{xyz}
//
//                bool skip = false;
//                if (sum_row == 0) {
//                    attested_rows[i] = false;
//                    skip = true;
//                }
//                if (sum_col == 0) {
//                    attested_cols[j] = false;
//                    skip = true;
//                }
//                if (skip) {
//                    continue;
//                }
//
//                e.push_back(sum_col * sum_row); // N_{x+z} * N_{+yz}
//                o.push_back(observed); // N_{xyz}
//            }
//        }
//
//        for (int i = 0; i < o.size(); ++i) {
//            double expected = e[i] / (double) total; // E_{xyz} = (N_{x+z} * N_{+yz}) / N_{++z}
//
//            if (o[i] != 0) {
//                local_g2 += 2.0 * o[i] * log(o[i] / expected); // 2 * N_{xyz} * log (N_{xyz} / E_{xyz})
//            }
//        }
//
//        if (total == 0) {
//            continue;
//        }
//
//        int num_attested_rows = 0;
//        int num_attested_cols = 0;
//        for (const bool &attested_row : attested_rows) {
//            if (attested_row) {
//                num_attested_rows++;
//            }
//        }
//        for (const bool &attested_col : attested_cols) {
//            if (attested_col) {
//                num_attested_cols++;
//            }
//        }
//        int local_df = (num_attested_rows - 1) * (num_attested_cols - 1);
//        if (local_df > 0) {
//            df += local_df;
//            g2 += local_g2;
//        }
//    }
//
//    delete cell_table;
//    cell_table = nullptr;
//    delete [] cond_dims;
//    cond_dims = nullptr;
////    timer->Stop("counting2");
//
////    timer->Start("computing p-value");
//    if (df == 0) { // if df == 0, this is definitely an independent table
//        return IndependenceTest::Result result(1.0, true);
//    }
//
//    // if p < alpha, reject the null hypothesis: dependent
//    // if p > alpha, accept the null hypothesis: independent
//    double p_value = 1.0 - stats::pchisq(g2, df, false);
//    bool indep = (p_value > alpha);
////    timer->Stop("computing p-value");
//    return IndependenceTest::Result(p_value, indep);
//}
//
///**
// * calculate g square for ci-test x _||_ y | z1, z2, ..., max
// * using a DFS tree, refer to code https://github.com/asrivast28/ramBLe, and papers
// *      A parallel framework for constraint-based bayesian network learning via markov blanket discovery, 2020
// *      Fast counting in machine learning applications, 2018
// * @param test_idx: x, y, z1, z2 ...
// * @param size: number of test_idx
// */
//IndependenceTest::Result IndependenceTest::ComputeGSquare(int* test_idx, int size) {
//    double g2 = 0.0;
//    int df = 0;
//
//    vector<int> dims;
//    for (int i = 0; i < size; ++i) {
//        // get the number of possible values of each feature in indices, from Dataset.num_of_possible_values_of_disc_vars
//        int dim = dataset->num_of_possible_values_of_disc_vars[test_idx[i]];
//        dims.push_back(dim);
//    }
//
//    // dimensions of z1, z2, ...; copy from "dims", starting from the third element
//    // because "dims" contains the dimensions of x, y, z1, z2, ..., then we ignore the first two elements
//    int* cond_dims = new int[size - 2];
//    for (int i = 0; i < size - 2; ++i) {
//        cond_dims[i] = dims[i + 2];
//    }
//
//    int num_rows = dims[0]; // dimension of x
//    int num_cols = dims[1]; // dimension of y
//
//    CombinationGenerator cg(cond_dims, size - 2);
//    while (cg.has_next) {
//        vector<int> combination = cg.Next();
//        vector<int> base; // at the beginning, base is the whole data set
//        for (int i = 0; i < dataset->num_instance; ++i) {
//            base.push_back(i);
//        }
//
//        for (int i = 0; i < combination.size(); ++i) { // for z1, z2, ...
//            base = Common(base, test_idx[i + 2], combination[i]);
//        }
//        long total = base.size(); // N_{++z}
//        if (total == 0)
//            continue;
//
//        double local_g2 = 0.0;
//        vector<double> e;
//        vector<double> o;
//
//        for (int i = 0; i < num_rows; ++i) { // for each possible value of x
//            vector<int> count_x = Common(base, test_idx[0], i);
//            long sum_row = count_x.size(); // N_{x+z}
//            if (sum_row == 0)
//                continue;
//
//            for (int j = 0; j < num_cols; ++j) { // for each possible value of y
//                vector<int> count_y = Common(base, test_idx[1], j);
//                long sum_col = count_y.size(); // N_{+yz}
//                if (sum_col == 0)
//                    continue;
//
//                vector<int> count_xy = Common(count_x, test_idx[1], j);
//                long observed = count_xy.size(); // N_{xyz}
//
//                if (observed != 0) {
//                    double e = sum_row * sum_col; // N_{x+z} * N_{+yz}
//                    double expected = e / (double) total; // E_{xyz} = (N_{x+z} * N_{+yz}) / N_{++z}
//                    g2 += 2.0 * observed * log(observed / expected); // 2 * N_{xyz} * log (N_{xyz} / E_{xyz})
//                }
//            }
//        }
//        int local_df = (num_rows - 1) * (num_cols - 1);
//        df += local_df;
//    }
//
//    delete [] cond_dims;
//    cond_dims = nullptr;
//
//    if (df == 0) { // if df == 0, this is definitely an independent table
//        return IndependenceTest::Result(1.0, true);
//    }
//
//    // if p < alpha, reject the null hypothesis: dependent
//    // if p > alpha, accept the null hypothesis: independent
//    double p_value = 1.0 - stats::pchisq(g2, df, false);
//    bool indep = (p_value > alpha);
//    return IndependenceTest::Result(p_value, indep);
//}
//
///**
// * select feature[index] = value from a subset of data set
// * @param subset: index of a subset of the data set
// * @return a set of index
// */
//vector<int> IndependenceTest::Common(const vector<int> &subset, int index, int value) {
//    vector<int> result;
//    for (const int &i : subset) {
//        if (dataset->dataset_all_vars[i][index] == value) {
//            result.push_back(i);
//        }
//    }
//    return result;
//}
/**----------------------------- implementations like Tetrad -----------------------------**/