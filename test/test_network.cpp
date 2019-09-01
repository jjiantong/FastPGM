#include <iostream>
#include <string>
#include <vector>
#include "gtest/gtest.h"
#include "tinyxml2.h"

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
    network = new ChowLiuTree(true);

    string train_set_file_path, test_set_file_path;


    train_set_file_path =
            //  "../../data/a1a.txt"
            //  "../../data/a2a.txt"
              "../../data/dataset/a3a-small.txt"
      //  "../../data/w1a.txt"
      //  "../../data/w7a.txt"
            ;

    test_set_file_path =
            //  "../../data/a1a.test.txt"
            //  "../../data/a2a.test.txt"
              "../../data/dataset/a3a-small.test.txt"
      //  "../../data/w1a.test.txt"
      //  "../../data/w7a.test.txt"
            ;

    trainer->LoadLIBSVMDataAutoDetectConfig(train_set_file_path);
    tester->LoadLIBSVMDataAutoDetectConfig(test_set_file_path);
    network->StructLearnCompData(trainer, false);
    network->LearnParmsKnowStructCompData(trainer, false);
  }

  void TearDown() override {
//    delete trainer;
//    delete tester;
//    delete network;
  }

  Trainer *trainer;
  Trainer *tester;
  Network *network;
};


TEST_F(NetworkTest, DISABLED_chow_liu_tree_var_elim_accuracy) { // The prefix "DISABLED" disable this test.
  double accuracy = network->TestNetReturnAccuracy(tester);
  ScoreFunction sf(network, trainer);
  cout << "Scores\n"
       << "LogLikelihood: " << sf.LogLikelihood()  << '\n'
       << "AIC: " <<  sf.AIC() << '\n'
       << "BIC: " <<  sf.BIC() << '\n'
       << "K2: " <<  sf.K2()  << '\n'
       << "BDeu: " <<  sf.BDeu() << endl;
  EXPECT_GT(accuracy,0.6);
}

TEST_F(NetworkTest, DISABLED_approx_inference_accuracy) {
  double acc = network->TestNetByApproxInferReturnAccuracy(tester,100);
  //EXPECT_GT(acc, 0.6);
}

TEST_F(NetworkTest, DISABLED_gibbs_samples_to_libsvm_file) {
  vector<Combination> samples = network->DrawSamplesByGibbsSamp(1e4,1e5);
  string sample_file = "./gibbs_samples_to_LIBSVM_file.txt";
  trainer->SamplesToLIBSVMFile(samples, sample_file);

  Trainer *trn_samp = new Trainer();
  Network *net_samp = new ChowLiuTree();
  trn_samp->LoadLIBSVMDataAutoDetectConfig("./gibbs_samples_to_LIBSVM_file.txt");
  net_samp->StructLearnCompData(trn_samp, false);
  net_samp->LearnParmsKnowStructCompData(trn_samp, false);

  for(int i=0; i<net_samp->num_nodes; ++i) {
    fprintf(stdout, "\n====================================\n");
    Factor f1, f2;
    f1.ConstructFactor(network->FindNodePtrByIndex(i));
    f1.PrintPotentials();
    f2.ConstructFactor(net_samp->FindNodePtrByIndex(i));
    f2.PrintPotentials();
  }
}

TEST_F(NetworkTest, DISABLED_var_elim_and_jun_tree) {
  Combination E;
  E.insert(pair<int,int>(50,1));
  E.insert(pair<int,int>(51,1));
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
  ScoreFunction sf(network, trainer);
  cout << "Scores\n"
       << "LogLikelihood: " << sf.LogLikelihood()  << '\n'
       << "AIC: " <<  sf.AIC() << '\n'
       << "BIC: " <<  sf.BIC() << '\n'
       << "K2: " <<  sf.K2()  << '\n'
       << "BDeu: " <<  sf.BDeu() << endl;
  double accuracy = jt->TestNetReturnAccuracy(0,tester);
  delete jt;
  EXPECT_GT(accuracy,0.6);
}


TEST_F(NetworkTest, DISABLED_likelihood_weighting_accuracy) {
  ScoreFunction sf(network, trainer);
  cout << "Scores\n"
       << "LogLikelihood: " << sf.LogLikelihood()  << '\n'
       << "AIC: " <<  sf.AIC() << '\n'
       << "BIC: " <<  sf.BIC() << '\n'
       << "K2: " <<  sf.K2()  << '\n'
       << "BDeu: " <<  sf.BDeu() << endl;
  double accuracy = network->TestAccuracyByLikelihoodWeighting(tester, 50);
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
    n->set_discrete_parents_combinations.clear();
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
    if(0 == n_39->SampleNodeGivenParents(e)) {
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
}

TEST(CustomNetworkTest, sampling_dog_net_to_csv_file_and_relearn) {

  string custom_file = "../../data/interchange-format-file/dog-problem.xml";
  auto custom_net = new CustomNetwork(true);
  custom_net->GetNetFromXMLBIFFile(custom_file);

  vector<Combination> samples = custom_net->DrawSamplesByGibbsSamp(2e4,2e5);
  string sample_file = "./gibbs_samples_to_CSV_file.txt";
  auto trainer = new Trainer();
  trainer->SamplesToCSVFile(samples, sample_file);

  CustomNetwork *net_samp = new CustomNetwork(true);
  net_samp->GetNetFromXMLBIFFile(custom_file);
  net_samp->ClearParms();

  Trainer *trn_samp = new Trainer();
  trn_samp->LoadCSVDataAutoDetectConfig(sample_file);

  net_samp->LearnParmsKnowStructCompData(trn_samp, false);

  for(int i=0; i<net_samp->num_nodes; ++i) {
    fprintf(stdout, "\n====================================\n");
    Factor f1, f2;
    f1.ConstructFactor(custom_net->FindNodePtrByIndex(i));
    f1.PrintPotentials();
    f2.ConstructFactor(net_samp->FindNodePtrByIndex(i));
    f2.PrintPotentials();
  }
}