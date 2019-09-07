#include <iostream>
#include <string>
#include <vector>
#include "gtest/gtest.h"

#include "CustomNetwork.h"
#include "JunctionTree.h"
#include "EliminationTree.h"
#include "gadget.h"

class CGNetworkTest : public ::testing::Test {
 protected:
  void SetUp() override {

    // Figure 2 in [Local Propagation in Conditional Gaussian Bayesian Networks (Cowell, 2005)]

    net = new CustomNetwork(false);
    auto a = new DiscreteNode(0, "A");
    a->SetDomain(vector<int> {0});
    a->map_marg_prob_table[0] = 1;
    auto b = new DiscreteNode(1, "B");
    b->SetDomain(vector<int> {0});
    b->map_marg_prob_table[0] = 1;
    auto c = new DiscreteNode(2, "C");
    c->SetDomain(vector<int> {0});
    c->map_marg_prob_table[0] = 1;
    auto x = new ContinuousNode(3, "X");
    auto y = new ContinuousNode(4, "Y");
    auto z = new ContinuousNode(5, "Z");

    net->AddNode(a);
    net->AddNode(b);
    net->AddNode(c);
    net->AddNode(x);
    net->AddNode(y);
    net->AddNode(z);

    net->SetParentChild(a,x);
    net->SetParentChild(b,x);
    net->SetParentChild(c,y);
    net->SetParentChild(x,y);
    net->SetParentChild(c,z);
    net->SetParentChild(y,z);



    x->IdentifyContPar();
    x->GenDiscParCombs();
    y->IdentifyContPar();
    y->GenDiscParCombs();
    z->IdentifyContPar();
    z->GenDiscParCombs();

    x->mu[*(x->set_discrete_parents_combinations.begin())] = 0;
    x->variance[*(x->set_discrete_parents_combinations.begin())] = 1;

    y->mu[*(y->set_discrete_parents_combinations.begin())] = 0;
    y->variance[*(y->set_discrete_parents_combinations.begin())] = 1;
    y->coefficients[*(y->set_discrete_parents_combinations.begin())] = vector<double> {1};

    z->mu[*(z->set_discrete_parents_combinations.begin())] = 0;
    z->variance[*(z->set_discrete_parents_combinations.begin())] = 1;
    z->coefficients[*(z->set_discrete_parents_combinations.begin())] = vector<double> {1};


  }

  Network *net;
};


TEST_F(CGNetworkTest, DISABLED_structure_correctness) {
  auto x = (ContinuousNode*)net->FindNodePtrByName("X");
  auto y = (ContinuousNode*)net->FindNodePtrByName("Y");
  auto z = (ContinuousNode*)net->FindNodePtrByName("Z");

  EXPECT_EQ(1,x->set_discrete_parents_combinations.size());
  EXPECT_EQ(0,x->contin_par_indexes.size());
  EXPECT_EQ(0,x->coefficients[*x->set_discrete_parents_combinations.begin()].size());

  EXPECT_EQ(1,y->set_discrete_parents_combinations.size());
  EXPECT_EQ(1,y->contin_par_indexes.size());
  EXPECT_EQ(1,y->coefficients[*y->set_discrete_parents_combinations.begin()].size());

  EXPECT_EQ(1,z->set_discrete_parents_combinations.size());
  EXPECT_EQ(1,z->contin_par_indexes.size());
  EXPECT_EQ(1,z->coefficients[*z->set_discrete_parents_combinations.begin()].size());

  auto top = net->GetTopoOrd();
  set<int> top3, assumed_top3;
  for (int i=0; i<3; ++i) {
    top3.insert(top.at(i));
    assumed_top3.insert(i);
  }
  EXPECT_EQ(6, top.size());
  EXPECT_TRUE(top3==assumed_top3);
}

TEST_F(CGNetworkTest, elim_tree) {
  auto elim_tree = new EliminationTree(net);
}