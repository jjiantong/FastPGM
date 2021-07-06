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
bool IndependenceTest::IsIndependent(int x_idx, int y_idx, set<int> z, string metric) {
    /**
     * for testing x, y given z1,...,zn,
     * set up an array of length n + 2 containing the indices of these variables in order
     */
    vector<int> test_idx;
    test_idx.push_back(x_idx);
    test_idx.push_back(y_idx);
    for (auto it = z.begin(); it != z.end(); it++) {
        test_idx.push_back(*it);
    }

    if (metric.compare("g square") == 0) {}
    else if (metric.compare("mutual information") == 0) {}
    else {}

    int random_number = rand()%10000;
    if (random_number > 9990) {
        return true;
    } else {
        return false;
    }
}

/**
 * perform conditional independence tests of discrete data using the G Square method
 * degrees of freedom are calculated as in Fienberg, The Analysis of Cross-Classified Categorical Data, 2nd Edition, 142
 */
//bool IndependenceTest::IsIndependentByGSquare(vector<int> test_idx) {
//
//    GSquareTest.Result result = gSquareTest.calcGSquare(testIndices);
//    this.gSquare = result.getGSquare();
//    this.pValue = result.getPValue();
//
//    if (result.isIndep()) {
//        StringBuilder sb = new StringBuilder();
//        sb.append("INDEPENDENCE ACCEPTED: ");
//        sb.append(SearchLogUtils.independenceFact(x, y, z));
//        sb.append("\tp = ").append(nf.format(result.getPValue())).append(
//                "\tg^2 = ").append(nf.format(result.getGSquare())).append(
//                "\tdf = ").append(result.getDf());
//        TetradLogger.getInstance().log("independencies", sb.toString());
//    } else {
//        StringBuilder sb = new StringBuilder();
//        sb.append("Not independent: ");
//        sb.append(SearchLogUtils.independenceFact(x, y, z));
//        sb.append("\tp = ").append(nf.format(result.getPValue())).append(
//                "\tg^2 = ").append(nf.format(result.getGSquare())).append(
//                "\tdf = ").append(result.getDf());
//        TetradLogger.getInstance().log("independencies", sb.toString());
//    }
//
//    return result.isIndep();
//}

/**
 * calculate g square for a conditional crosstabulation table for independence question 0 _||_ 1 | 2, 3, ...max
 * by summing up g square and degrees of freedom for each conditional table in turn
 * rows or columns that consist entirely of zeros have been removed
 */
IndependenceTest::Result IndependenceTest::ComputeGSquare(vector<int> test_idx) {
    // reset the cell table for the columns referred to in 'test_idx', do cell coefs for those columns
    cell_table->AddToTable(dataset, test_idx);

    // indicator vectors to tell the cell table which margins to calculate
    // for x _||_ y | z1, z2, ..., we want to calculate the margin for x, the margin for y, and the margin for x and y
    vector<int> first_var = {0};
    vector<int> second_var = {1};
    vector<int> both_vars = {0, 1};

    double g2 = 0.0;
    int df = 0;

    // dimensions of z1, z2, ...; copy from "cell_table->dims", starting from the third element
    // because "cell_table->dims" contains the dimensions of x, y, z1, z2, ..., then we ignore the first two elements
    // note that "dims" contains the dimensions of all variables, so we cannot use "dims" here
    vector<int> cond_dims;
    cond_dims.assign(cell_table->dims.begin() + 2, cell_table->dims.end());

    vector<int> config; // x, y, z1, z2, ...
    config.reserve(test_idx.size());
    int num_rows = cell_table->dims[0]; // dimension of x
    int num_cols = cell_table->dims[1]; // dimension of y
    vector<bool> attested_rows;
    vector<bool> attested_cols;

    CombinationGenerator cg(cond_dims);
    while (cg.has_next) {
        vector<int> combination = cg.Next();
        for (int i = 0; i < cond_dims.size(); ++i) {
            config.at(i + 2) = combination.at(i);
        }
        attested_rows.assign(num_rows, true);
        attested_cols.assign(num_cols, true);

        int total = cell_table->ComputeMargin(config, both_vars); // N_{++z}

        double local_g2 = 0.0;
        vector<double> e;
        vector<double> o;

        for (int i = 0; i < num_rows; i++) { // for each possible value of x
            for (int j = 0; j < num_cols; j++) { // for each possible value of y
                config.at(0) = i;
                config.at(1) = j;

                int sum_row = cell_table->ComputeMargin(config, second_var); // N_{x+z}
                int sum_col = cell_table->ComputeMargin(config, first_var); // N_{+yz}
                int observed = cell_table->GetValue(config); // N_{xyz}

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

        for (int i = 0; i < o.size(); i++) {
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
        for (bool attested_row : attested_rows) {
            if (attested_row) {
                num_attested_rows++;
            }
        }
        for (bool attested_col : attested_cols) {
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

    if (df == 0) { // if df == 0, this is definitely an independent table
        double p_value = 1.0;
        IndependenceTest::Result result(g2, p_value, df, true);
        return result;
    }

    // if p < alpha, reject the null hypothesis: dependent
    // if p > alpha, accept the null hypothesis: independent
//    double pValue = 1.0 - ProbUtils.chisqCdf(g2, df);
//    boolean indep = (pValue > getAlpha());
//    return new GSquareTest.Result(g2, pValue, df, indep);

    return IndependenceTest::Result(g2, 0, df, true);
}