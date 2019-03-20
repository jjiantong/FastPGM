//
// Created by llj on 3/18/19.
//

#include <iostream>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
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