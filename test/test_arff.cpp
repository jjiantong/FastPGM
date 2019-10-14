//
// Created by LinjianLi on 2019/10/11.
//

#include "gtest/gtest.h"
#include "arff_parser.h"

TEST(ArffTest, sometest) {
  ArffParser ap = ArffParser("../../data/dataset/Phishing/Phishing_Training_Dataset.arff");
  auto ap_data = ap.parse();
  EXPECT_EQ(ap_data->get_relation_name(), "phishing");
  EXPECT_EQ(ap_data->num_attributes(), 31);
}