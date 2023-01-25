//
// Created by jjt on 18/09/22.
//
#include "common.h"

/**
 * @brief: convert a configuration (e.g., a config can be derived from an instance) into a map for fast query
 * set<pair<int, int>> --> map<int, int>
 */
map<int, int> DiscreteConfigToMap(DiscreteConfig &disc_cfg) {
    map<int, int> result;
    for (const auto var_val : disc_cfg) {
        result[var_val.first] = var_val.second;
    }
    if (disc_cfg.size() != result.size()) {
        fprintf(stderr, "Function [%s]: The given DiscreteConfig may contain duplicate keys!", __FUNCTION__);
    }
    return result;
}

/**
 * @brief: for structure learning; check whether a directed graph has a cycle.
 * (This function uses BFS, but other graph traversal algorithms can work as well)
 * use topological ordering: if there are left nodes after topological ordering, then return true
 */
// TODO: can be merge with "TopoSortOfDAGZeroInDegreeFirst"
bool DirectedGraphContainsCircleByBFS(int **graph, int num_nodes) {
    int visited_count = 0;
    queue<int> que;

    // Calculate the in-degrees of all nodes.
    // TODO: calculate the in-degrees in "ConvertDAGNetworkToAdjacencyMatrix"
    int *in_degrees = new int[num_nodes](); // The parentheses at end will initialize the array to be all zeros.
    for (int i = 0; i < num_nodes; ++i) {
        for (int j = 0; j < num_nodes; ++j) {
            if (graph[i][j] == 1) {
                ++in_degrees[j];
            }
        }
    }

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

    delete[] in_degrees;
    return (visited_count != num_nodes); // check whether all nodes have been visited
}

/**
 * @brief: obtain the node indexes with topological ordering (the first is the node with in-degree = 0)
 * @param graph: 2-d array representation of the adjacency matrix of the graph
 * @param num_nodes: number of the nodes in the graph
 */
// TODO: may not need to use the directed adjacency matrix
// just use in-degree array and "set_children_indexes" to generate the ordering
vector<int> TopoSortOfDAGZeroInDegreeFirst(int **graph, int num_nodes) {
    vector<int> result;
    queue<int> que;

    // Calculate the in-degrees of all nodes.
    // TODO: move this computation to "GenTopoOrd"
    int *in_degrees = new int[num_nodes](); // The parentheses at end will initialize the array to be all zeros.
    for (int i = 0; i < num_nodes; ++i) {
        for (int j = 0; j < num_nodes; ++j) {
            if (graph[i][j] == 1) { ++in_degrees[j]; }
        }
    }

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
    delete[] in_degrees;
    return result;
}

/**
 * @brief: check if two configurations have a conflict of any shared variable.
 * @return:  true if they have different values on the same variable
 */
bool Conflict(const DiscreteConfig *cfg1, const DiscreteConfig *cfg2) {
    // cfg1 / cfg2: set< pair<int, int> >
    // f / s: pair<int, int>
    for (const auto &f : *cfg1) {
        for (const auto &s : *cfg2) {
            // if the two configs have the same variable but different values of the variable
            if (f.first == s.first && f.second != s .second) {
                return true;
            }
        }
    }
    return false;
}

bool Conflict(const DiscreteConfig *cfg, vector<int> &vec) {
    // cfg: set< pair<int, int> >
    // f: pair<int, int>
    for (const auto &f : *cfg) {
        for (int i = 0; i < vec.size(); ++i) {
            // if the two configs have the same variable but different values of the variable
            if (f.first == i && f.second != vec[i]) {
                return true;
            }
        }
    }
    return false;
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
        num_counts *= vec_range_each_digit[i];
    }

    vector<vector<int>> result_counts;
    result_counts.reserve(num_counts);

    // The left-most digit is the most significant digit.
    // The domain of each digit start at 0,
    // so the max value of this digit is one smaller than the range.

    for (int i = 0; i < num_counts; ++i) {
        result_counts.push_back(single_count);
        int check_digit = num_digits-1;

        // Add 1 to count.

        ++single_count[check_digit]; // Add 1 to the least significant digit.

        // Then deal with the carries.

        // The domain of each digit start at 0,
        // so the max value of this digit is one smaller than the range.
        bool need_carry = (single_count[check_digit] >= vec_range_each_digit[check_digit]);
        while (need_carry && check_digit > 0) {
            single_count[check_digit--] = 0;
            ++single_count[check_digit];
            need_carry = (single_count[check_digit] >= vec_range_each_digit[check_digit]);
        }
    }

    return result_counts;
}


