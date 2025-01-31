//
// Created by jjt on 2021/6/26.
//

#ifndef BAYESIANNETWORK_CHOICEGENERATOR_H
#define BAYESIANNETWORK_CHOICEGENERATOR_H

#include <iostream>
#include <vector>

using namespace std;

/**
 * @brief: Generates (non-recursively) all of the combinations of a choose b
 * a, b are non-negative integers and a >= b, the values of a and b are given in the constructor.
 * The sequence of choices is obtained by repeatedly calling the next() method. When the sequence
 * is finished, null is returned. A valid combination for a choose b is an array x[] of b int i,
 * 0 <= i < a, x[j] < x[j + 1] for each j from 0 to b - 1.
 */
class ChoiceGenerator {
public:

    int a; // number of objects being selected from
    int b; // number of objects in the desired selection
    int diff; // difference between a and b (should be non-negative)
    vector<int> choice; // internally stored choice

    ChoiceGenerator(int a, int b);

    vector<vector<int>> NextN(int n);
    vector<int> Next();
    void Fill(int index);
};

#endif //BAYESIANNETWORK_CHOICEGENERATOR_H
