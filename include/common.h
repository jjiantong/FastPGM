//
// Created by jjt on 17/09/22.
//

#ifndef BAYESIANNETWORK_COMMON_H
#define BAYESIANNETWORK_COMMON_H

#include <set>
#include <utility>
#include <string>
#include <queue>
#include <vector>
#include <map>
#include <iostream>
#include <cmath>
#include <cfloat>

#define SAFE_DELETE(p) do { if(p) { delete (p); (p) = nullptr; } } while(0)
#define SAFE_DELETE_ARRAY(p) do { if(p) { delete[] (p); (p) = nullptr; } } while(0)

#define ALGPCSTABLE 0
#define ALGBF 1
#define ALGJT 2
#define ALGVE 3
#define ALGPLS 4
#define ALGLW 5
#define ALGEPISBN 6
#define ALGLBP 7
#define ALGSIS 8
#define ALGAISBN 9

using namespace std;
typedef set< pair<int, int> > DiscreteConfig;//[variable id, variable value]

/**
 * @brief: to support both discrete and continuous variables
 * a value whose type is int or float
 */
struct Value {
private:
    int int_ = INT32_MIN;     //for discrete variable
    float float_ = -FLT_MAX;  //for continuous variable
    bool use_int = true;
public:
    bool UseInt() const { return use_int; }
    void SetInt(int i) { int_ = i; float_ = -FLT_MAX; use_int = true; }
    void SetFloat(float f) { float_ = f; int_ = INT32_MIN; use_int = false; }
    int GetInt() const { if (!use_int) exit(1); return int_; }
    float GetFloat() const { if (use_int) exit(1); return float_; }

    //define the "<" operator; this operator is needed for set/map.
    bool operator <(const Value v) const {
        if (this->UseInt() && v.UseInt()) {
            return this->GetInt() < v.GetInt();
        } else if (this->UseInt() && !v.UseInt()) {
            return this->GetInt() < v.GetFloat();
        } else if (!this->UseInt() && v.UseInt()) {
            return this->GetFloat() < v.GetInt();
        } else {
            return this->GetFloat() < v.GetFloat();
        }
    }
};

typedef pair<int, Value> VarVal; //variable/node id, value of the variable/node
typedef pair<int, int> DiscVarVal;//similar to VarVal, but for discrete varaible

/** this configuration is for both discrete and continuous.
 *  the set contains the observed values of each variable.
 * **/
typedef set<VarVal> Configuration;

/**
 * @brief: generate all the combinations given the set of parents
 */
template <typename T>
set<set<T>> GenAllCombinationsFromSets(set<set<T>> *set_of_sets) {//checked 11/Apr/2020
    if (set_of_sets->empty()) {
        return set<set<T>>(); //this func uses recursion, because the number of sets is unknown sometimes.
    }

    auto its = set_of_sets->begin(); // get the set of all possible values of the first parent
    set<T> to_be_inserted = *its;
    set<set<T>> result, temp_result;

    // Base Case
    if (set_of_sets->size() == 1) {
        for (T p : to_be_inserted){ // for each possible value of the parent
            set<T> c;
            c.insert(p);
            result.insert(c);
        }
        return result;
    }

    // Recursive Case (the size of set_of_sets is greater than 1)
    set_of_sets->erase(its);
    temp_result = GenAllCombinationsFromSets(set_of_sets); // get the result except for the first parent
    for (T p : to_be_inserted){ // for each possible value of the first parent
        for (set<T> c : temp_result) { // for each of the current parent combinations
            c.insert(p);
            result.insert(c);
        }
    }
    return result;
};

map<int, int> DiscreteConfigToMap(DiscreteConfig &disc_cfg);

bool DirectedGraphContainsCircleByBFS(int **graph, int num_nodes);
vector<int> TopoSortOfDAGZeroInDegreeFirst(int **graph, int num_nodes);


bool Conflict(const DiscreteConfig *cfg1, const DiscreteConfig *cfg2);
bool Conflict(const DiscreteConfig *cfg, vector<int> &vec);

string TrimRight(string);
string TrimLeft(string);
string Trim(string &);
string TrimRightComma(string s);
vector<string> Split(string &s, string delimiter);

vector<vector<int>> NaryCount(vector<int> vec_range_each_digit);


class Parameter{ // TODO: it's different from that in FastBN
public:
    int algorithm;
    string dataset;
    int num_threads; //for OpenMP

    int group_size; // for PC-Stable

    int num_samples; // for approximate inference
    int num_updating; // for approximate inference
    int lbp_iter; // for epis-bn and lbp
    bool enable_heuristic_uniform_distribution;
    bool enable_heuristic_theta_cutoff;

    Parameter();
    void read_input_parameters(int argc, char *argv[]);
};

#endif //BAYESIANNETWORK_COMMON_H

