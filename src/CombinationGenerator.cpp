//
// Created by jjt on 2021/7/4.
//

#include "CombinationGenerator.h"

/**
 * creates a combination set for a set of variables with the given number of max_values
 * @param max_values: an int array consisting of the maximum values of each variable, in order
 * @param size: size of max_values
 */
CombinationGenerator::CombinationGenerator(int* max_values, int size) {
    num_values = size;
    values.resize(size);
    this->max_values = new int[size];
    for (int i = 0; i < size; ++i) {
        this->max_values[i] = max_values[i];
    }
    has_next = true;
}

CombinationGenerator::~CombinationGenerator() {
    delete [] max_values;
}

/**
 * @brief: find the next combination
 * @return a vector of the current combination,
 * but the "values" move to the next combination, and "has_next" remains true, if we can find the next combination
 * otherwise "values" remains the current combination, and "has_next" becomes false
 */
vector<int> CombinationGenerator::Next() {
    vector<int> clone = values; // clone = current combination

    int i;
    // start from the last position to find an element that can +1
    for (i = num_values - 1; i >= 0; i--) {
        if (values.at(i) + 1 < max_values[i]) {
            break;
        }
    }

    if (i < 0) { // cannot find the next combination
        has_next = false;
    } else { // position i can +1
        values.at(i)++;
        for (int j = i + 1; j < num_values; j++) {
            values[j] = 0;
        }
    }

    return clone;
}