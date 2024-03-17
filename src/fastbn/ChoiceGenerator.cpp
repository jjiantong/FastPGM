//
// Created by jjt on 2021/6/26.
//

#include "fastbn/ChoiceGenerator.h"

/**
 * @brief: Constructs a new choice generator for a choose b.
 * once this initialization has been performed, successive calls to next() will produce the series of combinations.
 * to begin a new series at any time, call this init method again with new values for a and b.
 * @param a the number of objects being selected from
 * @param b the number of objects in the desired selection
 */
ChoiceGenerator::ChoiceGenerator (int a, int b) {
    if ((a < 0) || (b < 0) || (a < b)) {
        printf("Function ChoiceGenerator: Invalid value of a and/or b!");
        exit(1);
    }

    this->a = a;
    this->b = b;
    diff = a - b;

    /**
     * initialize the choice array with successive b int [0 1 2 ...]
     * set the value at the last index one less than it would be ([0 1 2 ... b-2 b-2])
     * so that on the first call to next() correctly returns the first combination ([0 1 2 ... b-1])
     */
    choice.reserve(b);
    for (int i = 0; i < b - 1; i++) {
        choice.push_back(i);
    }
    choice.push_back(b-2);
}

/**
 * generate multiple choice at one time
 * @param n: the number of choices to be generated at one time
 * @return the next n choices, or null
 */
vector<vector<int>> ChoiceGenerator::NextN(int n) {
    vector<vector<int>> choices;
    choices.reserve(n);

    for (int i = 0; i < n; ++i) {
        vector<int> c = Next();
        choices.push_back(c);
    }
    return choices;
}

/**
 * @return the next combination in the series, or null if the series is finished
 */
vector<int> ChoiceGenerator::Next() {
    //-------------------- multiple ci tests at one time -------------------//
    if (choice.empty()) {
        return vector<int>();
    }
    //-------------------- multiple ci tests at one time -------------------//
    int i = b;

    // Scan from the right, find the first index whose value is less than its expected maximum (i + diff),
    // and then perform the fill() operation at this index, because this index is able to be added
    while (--i > -1) {
        if (choice[i] < i + diff) {
            Fill(i);
            return choice;
        }
    }
    return vector<int>();
}

/**
 * fill the 'choice' array, from index 'index' to the end of the array,
 * with successive integers starting with choice[index] + 1
 * @param index the index to begin this incrementing operation
 */
void ChoiceGenerator::Fill(int index) {
    choice[index]++;

    for (int i = index + 1; i < b; ++i) {
        choice[i] = choice[i - 1] + 1;
    }
}
