#include <iostream>
#include <vector>
#include "gtest/gtest.h"
#include "gadget.h"

TEST(GadgetTest, DISABLED_nary_count) {//this test case includes miscellaneous functions
  vector<int> vec_range_each_digit {2,2,3};//binary, binary, 3; in this example, the 1st and 2nd var are binary, and the 3rd is 3-nary;
  vector<vector<int>> counts = NaryCount(vec_range_each_digit);
    //this function is to generate all combinations of the vector in an ordered sequence, e.g., [0,0,0], [0,0,1], [0,0,2], [0,1,0],...
  for (auto &c : counts) {
    for (auto &d : c) {
      cout << d;
    }
    cout << ", ";
  }
}


TEST(GadgetTest, template_test) {//the function for test is a template.
  set<pair<int,int>> s1;
  set<pair<int,int>> s2;
  s1.insert(pair<int,int>(1,1));
  s1.insert(pair<int,int>(2,2));
  s2.insert(pair<int,int>(7,7));
  s2.insert(pair<int,int>(8,9));
  s2.insert(pair<int,int>(9,9));
  set<set<pair<int,int>>> s {s1,s2};

  set<set<pair<int,int>>> comb = GenAllCombinationsFromSets(&s);//get Cartesian Product of two sets.
  for (auto &c : comb) {
    for (auto &d : c) {
      cout << d.first;
    }
    cout << ", ";
  }
}