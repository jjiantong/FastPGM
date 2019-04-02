//
// Created by llj on 3/11/19.
//

#include <iostream>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "gtest/gtest.h"

#include "Trainer.h"
#include "Network.h"
#include "ChowLiuTree.h"
#include "JunctionTree.h"
#include "CustomNetwork.h"
#include "ScoreFunction.h"
#include "gadget.h"

class NetworkTest : public ::testing::Test {
 protected:
  void SetUp() override {
    trainer = new Trainer();
    tester = new Trainer();
    network = new ChowLiuTree();

    string train_set_file_path, test_set_file_path;


    train_set_file_path =
            //  "../../data/a1a.txt"
            //  "../../data/a2a.txt"
              "../../data/dataset/a3a.txt"
      //  "../../data/w1a.txt"
      //  "../../data/w7a.txt"
            ;

    test_set_file_path =
            //  "../../data/a1a.test.txt"
            //  "../../data/a2a.test.txt"
              "../../data/dataset/a3a.test.txt"
      //  "../../data/w1a.test.txt"
      //  "../../data/w7a.test.txt"
            ;

    trainer->LoadLIBSVMDataAutoDetectConfig(train_set_file_path);
    tester->LoadLIBSVMDataAutoDetectConfig(test_set_file_path);
    network->StructLearnCompData(trainer);
    network->LearnParmsKnowStructCompData(trainer);
  }

  Trainer *trainer;
  Trainer *tester;
  Network *network;
};

TEST(OtherTest,DISABLED_usablity_of_gtest) {
  EXPECT_EQ(1,1);
  EXPECT_EQ(tgamma(6),FactorialForSmallInteger(5));
}

TEST_F(NetworkTest,DISABLED_chow_liu_tree_var_elim_accuracy) { // The prefix "DISABLED" disable this test.
  network->TestNetReturnAccuracy(tester);
}

TEST_F(NetworkTest, DISABLED_approx_inference_accuracy) {
  network->TestNetByApproxInferReturnAccuracy(tester,100);
  EXPECT_EQ(1,2);
}

TEST_F(NetworkTest, DISABLED_samples_to_libsvm_file) {
  vector<Combination> samples = network->DrawSamples(10000);
  trainer->SamplesToLIBSVMFile(samples,"./samples_to_LIBSVM_file.txt");

  Trainer *trn_samp = new Trainer();
  Network *net_samp = new ChowLiuTree();
  trn_samp->LoadLIBSVMDataAutoDetectConfig("./samples_to_LIBSVM_file.txt");
  net_samp->StructLearnCompData(trn_samp);
  net_samp->LearnParmsKnowStructCompData(trn_samp);

  for(int i=0; i<net_samp->num_nodes; ++i) {
    fprintf(stdout, "\n====================================\n");
    Factor f1, f2;
    f1.ConstructFactor(network->FindNodePtrByIndex(i));
    f1.PrintPotentials();
    f2.ConstructFactor(net_samp->FindNodePtrByIndex(i));
    f2.PrintPotentials();
  }

  EXPECT_EQ(1,1);
}

TEST_F(NetworkTest,DISABLED_var_elim_and_jun_tree) { // The prefix "DISABLED" disable this test.
  Combination E;
  E.insert(pair<int,int>(104,1));
  E.insert(pair<int,int>(112,1));
  E.insert(pair<int,int>(99,1));
  cout << "Size of evidence: " << E.size() << endl;
  cout << "Evidence (only print 1): { ";
  for (auto &e : E) {
    cout << e.first << ':' << e.second << ' ';
  }
  cout << " }" << endl;

  Node *node_ptr = network->FindNodePtrByIndex(0);
  Factor f1 = network->VarElimInferReturnPossib(E,node_ptr);
  f1.PrintPotentials();


  auto *jt = new JunctionTree(network);
  jt->LoadEvidence(E);

  jt->MessagePassingUpdateJT();

  set<int> indexes;
  indexes.insert(0);
  Factor f2 = jt->BeliefPropagationReturnPossib(indexes);
  f2.PrintPotentials();

  for (auto &c : f1.set_combinations) {
    double diff_abs = abs(f1.map_potentials[c] - f2.map_potentials[c]);
    // Use EQ may fail in some cases where they should pass.
    EXPECT_LE(diff_abs,1.0E-10);
  }
}

TEST_F(NetworkTest, DISABLED_custom_network) { // The prefix "DISABLED" disable this test.
  auto *network = new CustomNetwork();
  network->ConstructCustomNetworkStructFromFile("../../data/interchange-format-file/example_custom_network_file2.txt");
  network->SetCustomNetworkParamsFromFile("../../data/interchange-format-file/example_custom_network_file2.txt");

  Combination e;
  e.insert(pair<int,int>(1,0));
  e.insert(pair<int,int>(4,0));

  cout << "********************************** Variable Elimination Algorithm" << endl;
  int z[5] = {5,4,3,2,1};
  Node *node_ptr = network->FindNodePtrByIndex(0);
  Factor f1 = network->VarElimInferReturnPossib(z,5,e,node_ptr);
  f1.PrintPotentials();
  int z1[5] = {5,4,3,2,0};
  node_ptr = network->FindNodePtrByIndex(1);
  f1 = network->VarElimInferReturnPossib(z1,5,e,node_ptr);
  f1.PrintPotentials();
  int z2[5] = {5,4,3,1,0};
  node_ptr = network->FindNodePtrByIndex(2);
  f1 = network->VarElimInferReturnPossib(z2,5,e,node_ptr);
  f1.PrintPotentials();
  int z3[5] = {5,4,2,1,0};
  node_ptr = network->FindNodePtrByIndex(3);
  f1 = network->VarElimInferReturnPossib(z3,5,e,node_ptr);
  f1.PrintPotentials();
  int z4[5] = {5,3,2,1,0};
  node_ptr = network->FindNodePtrByIndex(4);
  f1 = network->VarElimInferReturnPossib(z4,5,e,node_ptr);
  f1.PrintPotentials();
  int z5[5] = {4,3,2,1,0};
  node_ptr = network->FindNodePtrByIndex(5);
  f1 = network->VarElimInferReturnPossib(z5,5,e,node_ptr);
  f1.PrintPotentials();

  JunctionTree *jt = new JunctionTree(network);
  jt->LoadEvidence(e);

  jt->MessagePassingUpdateJT();

  cout << "********************************** Junction Tree Algorithm" << endl;
  set<int> indexes;
  indexes.insert(0);
  Factor f2 = jt->BeliefPropagationReturnPossib(indexes);
  f2.PrintPotentials();
  indexes.clear();
  indexes.insert(1);
  f2 = jt->BeliefPropagationReturnPossib(indexes);
  f2.PrintPotentials();
  indexes.clear();
  indexes.insert(2);
  f2 = jt->BeliefPropagationReturnPossib(indexes);
  f2.PrintPotentials();
  indexes.clear();
  indexes.insert(3);
  f2 = jt->BeliefPropagationReturnPossib(indexes);
  f2.PrintPotentials();
  indexes.clear();
  indexes.insert(4);
  f2 = jt->BeliefPropagationReturnPossib(indexes);
  f2.PrintPotentials();
  indexes.clear();
  indexes.insert(5);
  f2 = jt->BeliefPropagationReturnPossib(indexes);
  f2.PrintPotentials();
}

TEST_F(NetworkTest, DISABLED_jun_tree_accuracy) {
  auto *jt = new JunctionTree(network);
  double accuracy = jt->TestNetReturnAccuracy(0,tester);
  EXPECT_GT(accuracy,0.6);
}

TEST_F(NetworkTest, DISABLED_score_usability) {
  auto *sf = new ScoreFunction(network,trainer);
  EXPECT_GT(sf->AIC(),-INT32_MAX);
  EXPECT_GT(sf->BIC(),-INT32_MAX);
  EXPECT_GT(sf->BDeu(),-INT32_MAX);
  EXPECT_GT(sf->K2(),-INT32_MAX);
}

TEST_F(NetworkTest, DISABLED_score_comparison) {
  auto *sf1 = new ScoreFunction(network,trainer);
  double score1 = sf1->BDeu();

  for (auto &n : network->set_node_ptr_container) {
    if (0==n->GetNodeIndex()) {continue;}
    if (0==(*n->set_parents_ptrs.begin())->GetNodeIndex()) {
      continue;
    }
    n->set_parents_combinations.clear();
  }

  auto *sf2 = new ScoreFunction(network,trainer);
  double score2 = sf2->BDeu();

  EXPECT_GT(score1,score2);
}

TEST_F(NetworkTest, DISABLED_sampling_node) {
  Node *n_39 = network->FindNodePtrByIndex(39);
  //Factor fac(n_39);
  //fac.PrintPotentials();

  Combination e;
  e.insert(pair<int,int>(0,-1));
  int count_0 = 0;
  for (int i=0; i<10000; ++i) {
    if(0==n_39->SampleNodeGiven(e)) {
      ++count_0;
    }
  }
  double rate_0 = ((double)count_0) / ((double)10000);
  //fprintf(stdout, "%f", rate_0);
  EXPECT_GT(rate_0,0.8);
  EXPECT_LT(rate_0,0.86);
}

TEST_F(NetworkTest, DISABLED_sampling_network) {
  Combination samp = network->ProbLogicSampleNetwork();
  EXPECT_EQ(1,1);
}

TEST(OtherTest, DISABLED_log_of_factorial) {
  cout << FactorialForSmallInteger(2000) << '\n'
       << log(2000) << '\n'
       << LogOfFactorial(2000) << endl;
  EXPECT_GT(LogOfFactorial(2000),0);
}