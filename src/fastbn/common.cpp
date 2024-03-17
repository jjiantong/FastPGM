//
// Created by jjt on 18/09/22.
//
#include <random>
#include "fastbn/common.h"

/**
 * @brief: for structure learning; check if a directed graph has a cycle.
 * use the same idea of `TopoSortOfDAGZeroInDegreeFirst` to get the topological ordering: iteratively handling the nodes
 * with zero in-degree and reducing the in-degree of its children by 1. if there are no left node after this process,
 * return false, which means the graph does not contain a circle.
 * we didn't merge this function with `TopoSortOfDAGZeroInDegreeFirst` because this function doesn't need to maintain
 * a vector<int> type `result`.
 */
bool DirectedGraphContainsCircle(int **graph, int *in_degrees, int num_nodes) {
    int visited_count = 0;
    queue<int> que;

    for (int i = 0; i < num_nodes; ++i) {
        if (in_degrees[i] == 0) {
            que.push(i);
        }
    }

    while (!que.empty()) {
        for (int j = 0; j < num_nodes; ++j) {
            if (graph[que.front()][j] == 1) {
                --in_degrees[j];
                if (in_degrees[j] == 0) {
                    que.push(j);
                }
            }
        }
        que.pop();
        ++visited_count;
    }

    return (visited_count != num_nodes); // check whether all nodes have been visited
}

/**
 * @brief: obtain the node indexes with topological ordering (the first is the node with in-degree = 0)
 * @param graph: 2-d array representation of the adjacency matrix of the graph
 * @param num_nodes: number of the nodes in the graph
 */
vector<int> TopoSortOfDAGZeroInDegreeFirst(int **graph, int *in_degrees, int num_nodes) {
    vector<int> result;
    queue<int> que;

    for (int i = 0; i < num_nodes; ++i) {
        // the first is the node with in-degree = 0
        if (in_degrees[i] == 0) {
            que.push(i);
        }
    }

    while (!que.empty()) {
        for (int j = 0; j < num_nodes; ++j) {
            // in-degree of each child of the output node -1
            if (graph[que.front()][j] == 1) {
                --in_degrees[j];
                // then check whether the in-degree of this child node is 0 after -1
                if (in_degrees[j] == 0) {
                    que.push(j);
                }
            }
        }
        result.push_back(que.front());
        que.pop();
    }
    return result;
}

/**
 * @brief: trim all \t, \n, \r and whitespace characters on the right of a string.
 */
string TrimRight(string s) {
    while (!s.empty() && s[s.size()-1]<33) { // ASCII. \t=09, \n=10, \r=13, space=32.
        s.erase(s.size()-1);
    }
    return s;
}

string TrimRightComma(string s) {
    if (s[s.size()-1] == 44) { // ASCII: ,=44
        s.erase(s.size()-1);
    }
    return s;
}

string TrimLeftParenthesis(string s) {
    if (!s.empty() && s[0] == 40) { // ASCII: (=40
        s.erase(0, 1);
    }
    return s;
}

string TrimRightCommaAndParenthesis(string s) {
    if (s[s.size()-1] == 44 || s[s.size()-1] == 41) { // ASCII: )=41; ,=44
        s.erase(s.size()-1);
    }
    return s;
}

string TrimRightCommaAndSemicolon(string s) {
    if (s[s.size()-1] == 44 || s[s.size()-1] == 59) { // ASCII: ;=59; ,=44
        s.erase(s.size()-1);
    }
    return s;
}

/**
 * @brief: trim all \t, \n, \r and whitespace characters on the left of a string.
 */
string TrimLeft(string s) {
    while (!s.empty() && s[0]<33) { // ASCII. \t=09, \n=10, \r=13, space=32.
        s.erase(0, 1);
    }
    return s;
}

/**
 * @brief: trim all \t, \n, \r and whitespace characters on the left or right of a string.
 */
string Trim(string &s) {
    return TrimLeft( TrimRight(s) );
}

/**
 * @brief: split string s into a vector of strings by the delimiter
 */
vector<string> Split(string &s, string delimiter) {
    vector<string> result;
    size_t begin = 0, end = 0;
    while ((end=s.find_first_of(delimiter, begin)) != string::npos) {
        result.push_back(s.substr(begin, end-begin));
        begin = (++end);
    }
    result.push_back(s.substr(begin, s.size()-begin)); // the last one
    return result;
}

/**
 * @brief: given the range of each digit, obtain all possible configs (N-ary count)
 */
vector<vector<int>> NaryCount(vector<int> vec_range_each_digit) {//checked on 11/Apr/2020

    int num_digits = vec_range_each_digit.size();
    int num_counts = 1;   // The number of all n-ary counts.
    vector<int> single_count;
    single_count.reserve(num_digits);
    for (int i = 0; i < num_digits; ++i) {
        single_count.push_back(0);
        num_counts *= vec_range_each_digit[i]; // it's like the table size.
    }

    vector<vector<int>> result_counts;
    result_counts.reserve(num_counts);

    /************************** two versions ... **************************/
    // example: 612345. where 1 is the least significant digit, 6 is the most significant digit. weird!!
    // The domain of each digit start at 0, so the max value of this digit is one smaller than the range.
    vector<int> check_digit_order;
    check_digit_order.reserve(num_digits);
    for (int i = 1; i < num_digits; ++i) {
        check_digit_order.push_back(i);
    }
    check_digit_order.push_back(0);

    for (int i = 0; i < num_counts; ++i) {
        result_counts.push_back(single_count);

        int pos = 0;
        int check_digit = check_digit_order[pos]; // start at the least significant digit.

        // Add 1 to count.

        ++single_count[check_digit]; // Add 1 to the least significant digit.

        // Then deal with the carries.

        // The domain of each digit start at 0,
        // so the max value of this digit is one smaller than the range.
        bool need_carry = (single_count[check_digit] >= vec_range_each_digit[check_digit]);
        while (need_carry && pos < num_digits - 1) {
            single_count[check_digit] = 0;
            check_digit = check_digit_order[++pos];
            ++single_count[check_digit];
            need_carry = (single_count[check_digit] >= vec_range_each_digit[check_digit]);
        }
    }

//    // The left-most digit is the most significant digit.
//    // The domain of each digit start at 0,
//    // so the max value of this digit is one smaller than the range.
//    for (int i = 0; i < num_counts; ++i) {
//        result_counts.push_back(single_count);
//        int check_digit = num_digits-1;
//
//        // Add 1 to count.
//
//        ++single_count[check_digit]; // Add 1 to the least significant digit.
//
//        // Then deal with the carries.
//
//        // The domain of each digit start at 0,
//        // so the max value of this digit is one smaller than the range.
//        bool need_carry = (single_count[check_digit] >= vec_range_each_digit[check_digit]);
//        while (need_carry && check_digit > 0) {
//            single_count[check_digit--] = 0;
//            ++single_count[check_digit];
//            need_carry = (single_count[check_digit] >= vec_range_each_digit[check_digit]);
//        }
//    }
    /************************** two versions ... **************************/

    return result_counts;
}

int Random01() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> distribution(0, 1);
    return distribution(gen);
}

