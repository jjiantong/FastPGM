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

    DiscreteConfig empty_config;

    auto a = new DiscreteNode(0, "A");
    a->SetDomain(vector<int> {0});
    a->AddCount(0,empty_config,1);
    auto b = new DiscreteNode(1, "B");
    b->SetDomain(vector<int> {0});
    b->AddCount(0,empty_config,1);
    auto c = new DiscreteNode(2, "C");
    c->SetDomain(vector<int> {0});
    c->AddCount(0,empty_config,1);
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



//    x->IdentifyContPar();
    x->GenDiscParCombs(net->GetParentPtrsOfNode(x->GetNodeIndex()));
//    y->IdentifyContPar();
    y->GenDiscParCombs(net->GetParentPtrsOfNode(y->GetNodeIndex()));
//    z->IdentifyContPar();
    z->GenDiscParCombs(net->GetParentPtrsOfNode(z->GetNodeIndex()));

    x->map_mu[*(x->set_discrete_parents_combinations.begin())] = 0;
    x->map_variance[*(x->set_discrete_parents_combinations.begin())] = 1;

    y->map_mu[*(y->set_discrete_parents_combinations.begin())] = 0;
    y->map_variance[*(y->set_discrete_parents_combinations.begin())] = 1;
    y->map_coefficients[*(y->set_discrete_parents_combinations.begin())] = vector<double> {1};

    z->map_mu[*(z->set_discrete_parents_combinations.begin())] = 0;
    z->map_variance[*(z->set_discrete_parents_combinations.begin())] = 1;
    z->map_coefficients[*(z->set_discrete_parents_combinations.begin())] = vector<double> {1};


  }

  Network *net;
};


TEST_F(CGNetworkTest, structure_correctness) {
  auto x = (ContinuousNode*)net->FindNodePtrByName("X");
  auto y = (ContinuousNode*)net->FindNodePtrByName("Y");
  auto z = (ContinuousNode*)net->FindNodePtrByName("Z");

  EXPECT_EQ(1,x->set_discrete_parents_combinations.size());
  EXPECT_EQ(0,x->contin_par_indexes.size());
  EXPECT_EQ(0,x->map_coefficients[*x->set_discrete_parents_combinations.begin()].size());

  EXPECT_EQ(1,y->set_discrete_parents_combinations.size());
  EXPECT_EQ(1,y->contin_par_indexes.size());
  EXPECT_EQ(1,y->map_coefficients[*y->set_discrete_parents_combinations.begin()].size());

  EXPECT_EQ(1,z->set_discrete_parents_combinations.size());
  EXPECT_EQ(1,z->contin_par_indexes.size());
  EXPECT_EQ(1,z->map_coefficients[*z->set_discrete_parents_combinations.begin()].size());

  auto top = net->GetTopoOrd();
  set<int> top3, assumed_top3, back3, assumed_back3;
  for (int i=0; i<3; ++i) {
    top3.insert(top.at(i));
    back3.insert(top.at(5-i));
    assumed_top3.insert(i);
    assumed_back3.insert(5-i);
  }
  EXPECT_EQ(6, top.size());
  EXPECT_TRUE(top3==assumed_top3);
  EXPECT_TRUE(back3==assumed_back3);

//  net->PrintEachNodeParents();
}

TEST_F(CGNetworkTest, elim_tree) {
  EliminationTree *elim_tree = new EliminationTree(net, vector<int> {4,3,5,2,0,1});
  EXPECT_EQ(6,elim_tree->set_clique_ptr_container.size());

  elim_tree->EnterSingleContEvidence(pair<int,double> (3, 1.0));
  DiscreteConfig E;
  E.insert(pair<int,int>(2,0));
  elim_tree->LoadDiscreteEvidence(E);
  elim_tree->MessagePassingUpdateDiscretePartJT();
  auto cgr = elim_tree->CalMarginalOfContinuousVar(4);
  auto f = elim_tree->BeliefPropagationCalcuDiscreteVarMarginal(2);
}