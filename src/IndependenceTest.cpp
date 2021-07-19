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

    for (int i = 0; i < dataset->num_vars; i++) {
        // get the number of possible values of each feature in indices, from Dataset.num_of_possible_values_of_disc_vars
        int dim = dataset->num_of_possible_values_of_disc_vars.at(i);
        dims.push_back(dim);
    }

    cell_table = new CellTable();
}

/**
 * @brief: determining whether variable x is independent of variable y given a set of conditioning variables z
 * @param x_idx the one variable index being compared
 * @param y_idx the second variable index being compared
 * @param z the set of conditioning variables
 * @param metric the conditional independence test method
 */
bool IndependenceTest::IsIndependent(int x_idx, int y_idx, const set<int> &z, string metric) {
    /**
     * for testing x, y given z1,...,zn,
     * set up an array of length n + 2 containing the indices of these variables in order
     */
    int* test_idx = new int[z.size() + 2];
    test_idx[0] = x_idx;
    test_idx[1] = y_idx;
    int index = 2;
    for (auto it = z.begin(); it != z.end(); ++it) {
        test_idx[index++] = *it;
    }

    if (metric.compare("g square") == 0) {
        return IsIndependentByGSquare(test_idx, z.size() + 2);
    }
    else if (metric.compare("mutual information") == 0) {}
    else { // use for testing
        int random_number = rand()%10000;
        if (random_number > 9990) {
            return true;
        } else {
            return false;
        }
    }
    delete [] test_idx;
}

/**
 * perform conditional independence tests of discrete data using the G Square method
 * degrees of freedom are calculated as in Fienberg, The Analysis of Cross-Classified Categorical Data, 2nd Edition, 142
 * @param size: number of x, y, z1, z2, ...
 */
bool IndependenceTest::IsIndependentByGSquare(int* test_idx, int size) {

    IndependenceTest::Result result = ComputeGSquare(test_idx, size);
    return result.is_independent;
}

/**
 * calculate g square for a conditional crosstabulation table for independence question 0 _||_ 1 | 2, 3, ...max
 * by summing up g square and degrees of freedom for each conditional table in turn
 * rows or columns that consist entirely of zeros have been removed
 * @param size: number of x, y, z1, z2, ...
 */
IndependenceTest::Result IndependenceTest::ComputeGSquare(int* test_idx, int size) {
    timer.Start("counting");
    // reset the cell table for the columns referred to in 'test_idx', do cell coefs for those columns
    cell_table->AddToTable(dataset, test_idx, size);

    // indicator vectors to tell the cell table which margins to calculate
    // for x _||_ y | z1, z2, ..., we want to calculate the margin for x, the margin for y, and the margin for x and y
    int first_var[1] = {0};
    int second_var[1] = {1};
    int both_vars[2] = {0, 1};

    double g2 = 0.0;
    int df = 0;

    // dimensions of z1, z2, ...; copy from "cell_table->dims", starting from the third element
    // because "cell_table->dims" contains the dimensions of x, y, z1, z2, ..., then we ignore the first two elements
    // note that "dims" contains the dimensions of all variables, so we cannot use "dims" here
    int* cond_dims = new int[size - 2];
    for (int i = 0; i < size - 2; ++i) {
        cond_dims[i] = cell_table->dims[i + 2];
    }

    int* config = new int[size];
    int num_rows = cell_table->dims[0]; // dimension of x
    int num_cols = cell_table->dims[1]; // dimension of y
    vector<bool> attested_rows;
    vector<bool> attested_cols;

    CombinationGenerator cg(cond_dims, size - 2);
    while (cg.has_next) {
        vector<int> combination = cg.Next();
        for (int i = 0; i < size - 2; ++i) {
            config[i + 2] = combination.at(i);
        }
        attested_rows.assign(num_rows, true);
        attested_cols.assign(num_cols, true);

        long total = cell_table->ComputeMargin(config, size, both_vars, 2); // N_{++z}

        double local_g2 = 0.0;
        vector<double> e;
        vector<double> o;

        for (int i = 0; i < num_rows; ++i) { // for each possible value of x
            for (int j = 0; j < num_cols; ++j) { // for each possible value of y
                config[0] = i;
                config[1] = j;

                long sum_row = cell_table->ComputeMargin(config, size, second_var, 1); // N_{x+z}
                long sum_col = cell_table->ComputeMargin(config, size, first_var, 1); // N_{+yz}
                long observed = cell_table->GetValue(config); // N_{xyz}

                bool skip = false;
                if (sum_row == 0) {
                    attested_rows[i] = false;
                    skip = true;
                }
                if (sum_col == 0) {
                    attested_cols[j] = false;
                    skip = true;
                }
                if (skip) {
                    continue;
                }

                e.push_back(sum_col * sum_row); // N_{x+z} * N_{+yz}
                o.push_back(observed); // N_{xyz}
            }
        }

        for (int i = 0; i < o.size(); ++i) {
            double expected = e.at(i) / (double) total; // E_{xyz} = (N_{x+z} * N_{+yz}) / N_{++z}

            if (o.at(i) != 0) {
                local_g2 += 2.0 * o.at(i) * log(o.at(i) / expected); // 2 * N_{xyz} * log (N_{xyz} / E_{xyz})
            }
        }

        if (total == 0) {
            continue;
        }

        int num_attested_rows = 0;
        int num_attested_cols = 0;
        for (const bool &attested_row : attested_rows) {
            if (attested_row) {
                num_attested_rows++;
            }
        }
        for (const bool &attested_col : attested_cols) {
            if (attested_col) {
                num_attested_cols++;
            }
        }
        // like (|X| - 1)(|Y| - 1) but not actually: |X| and |Y| are for non-zero cases
        int local_df = (num_attested_rows - 1) * (num_attested_cols - 1);
        if (local_df > 0) {
            df += local_df;
            g2 += local_g2;
        }
    }

    delete [] config;
    delete [] cond_dims;
    timer.Stop("counting");

    timer.Start("computing p-value");
    if (df == 0) { // if df == 0, this is definitely an independent table
        double p_value = 1.0;
        IndependenceTest::Result result(g2, p_value, df, true);
        return result;
    }

    // if p < alpha, reject the null hypothesis: dependent
    // if p > alpha, accept the null hypothesis: independent
    double p_value = 1.0 - stats::pchisq(g2, df, false);
    bool indep = (p_value > alpha);
    timer.Stop("computing p-value");
    return IndependenceTest::Result(g2, p_value, df, indep);
}