//
// Created by jjt on 2021/7/4.
//

#ifndef BAYESIANNETWORK_COMBINATIONGENERATOR_H
#define BAYESIANNETWORK_COMBINATIONGENERATOR_H

#include <vector>

using namespace std;

/**
 * iterate through all the possible combinations for a set of variables (each with a different number of possible values)
 * for example, if the number of values for each variable is two, this would iterate through a truth table
 * note: not to be confused with a combinatorial (taking n values from m possible values -- like class ChoiceGenerator)
 */
class CombinationGenerator {
public:
    vector<int> values;
    vector<int> max_values;
    int num_values;
    bool has_next;

    CombinationGenerator(vector<int> max_values);

    vector<int> Next();
};

#endif //BAYESIANNETWORK_COMBINATIONGENERATOR_H
