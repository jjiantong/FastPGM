#include <iostream>
#include <vector>
#include "gtest/gtest.h"
#include "gadget.h"

TEST(GadgetTest, DISABLED_nary_count) {
  vector<int> vec_range_each_digit {2,2,3};
  vector<vector<int>> counts = NaryCount(vec_range_each_digit);
  for (auto &c : counts) {
    for (auto &d : c) {
      cout << d;
    }
    cout << ", ";
  }
}


TEST(GadgetTest, template_test) {
  set<pair<int,int>> s1;
  set<pair<int,int>> s2;
  s1.insert(pair<int,int>(1,1));
  s1.insert(pair<int,int>(2,2));
  s2.insert(pair<int,int>(7,7));
  s2.insert(pair<int,int>(8,9));
  s2.insert(pair<int,int>(9,9));
  set<set<pair<int,int>>> s {s1,s2};

  set<set<pair<int,int>>> comb = GenAllCombinationsFromSets(&s);
  for (auto &c : comb) {
    for (auto &d : c) {
      cout << d.first;
    }
    cout << ", ";
  }
}