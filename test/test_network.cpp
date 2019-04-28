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

TEST_F(NetworkTest, gibbs_samples_to_libsvm_file) {
  vector<Combination> samples = network->DrawSamplesByGibbsSamp(1000,10000);
  trainer->SamplesToLIBSVMFile(samples,"./gibbs_samples_to_LIBSVM_file.txt");

  Trainer *trn_samp = new Trainer();
  Network *net_samp = new ChowLiuTree();
  trn_samp->LoadLIBSVMDataAutoDetectConfig("./gibbs_samples_to_LIBSVM_file.txt");
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
    if(0== n_39->SampleNodeGivenParents(e)) {
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