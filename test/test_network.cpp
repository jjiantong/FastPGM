#include <iostream>
#include <string>
#include <vector>
#include "gtest/gtest.h"
#include "tinyxml2.h"

#include "Dataset.h"
#include "Network.h"
#include "ChowLiuTree.h"
#include "JunctionTree.h"
#include "CustomNetwork.h"
#include "ScoreFunction.h"
#include "gadget.h"

class NetworkTest : public ::testing::Test {
 protected:

  void SetUp() override {
    trainer = new Dataset();
    tester = new Dataset();
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
    network->StructLearnCompData(trainer, true);
    network->LearnParamsKnowStructCompData(trainer, 1, false);
  }


  Dataset *trainer;
  Dataset *tester;
  Network *network;
};


TEST_F(NetworkTest, chow_liu_tree_var_elim_accuracy) { // The prefix "DISABLED" disable this test.
  double accuracy = network->TestNetByVarElimReturnAccuracy(tester);
  ScoreFunction sf(network, trainer);
  sf.PrintAllScore();
  EXPECT_GT(accuracy,0.67);
  EXPECT_LT(sf.LogK2()+6196.83, 1e-3);
  EXPECT_LT(sf.LogBDeu()+7944.37, 1e-5);
  EXPECT_LT(sf.AIC()+5970.43, 1e-5);
  EXPECT_LT(sf.BIC()-sf.MDL(), 1e-5);
  EXPECT_LT(sf.BIC()+6577.86,1e-5);
  EXPECT_LT(sf.MDL()+6577.86, 1e-5);
}

TEST_F(NetworkTest, approx_inference_accuracy) {
  double acc = network->TestNetByApproxInferReturnAccuracy(tester,100);
  //EXPECT_GT(acc, 0.6);
}

TEST_F(NetworkTest, gibbs_samples_to_libsvm_file) {
  vector<DiscreteConfig> samples = network->DrawSamplesByGibbsSamp(1e4, 1e5);
  string sample_file = "./gibbs_samples_to_LIBSVM_file.txt";
  trainer->SamplesToLIBSVMFile(samples, sample_file);

  Dataset *trn_samp = new Dataset();
  Network *net_samp = new ChowLiuTree();
  trn_samp->LoadLIBSVMDataAutoDetectConfig("./gibbs_samples_to_LIBSVM_file.txt");
  net_samp->StructLearnCompData(trn_samp, false);
  net_samp->LearnParamsKnowStructCompData(trn_samp, false);

  for(int i=0; i<net_samp->num_nodes; ++i) {
    fprintf(stdout, "\n====================================\n");
    Factor f1(dynamic_cast<DiscreteNode*>(network->FindNodePtrByIndex(i)), network),
           f2(dynamic_cast<DiscreteNode*>(net_samp->FindNodePtrByIndex(i)), network);
    f1.PrintPotentials();
    f2.PrintPotentials();
  }
}

TEST_F(NetworkTest, var_elim_and_jun_tree) {
  DiscreteConfig E;
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

  auto *jt = new JunctionTree(network, false);
  jt->LoadEvidenceAndMessagePassingUpdateJT(E);

  Factor f2 = jt->BeliefPropagationCalcuDiscreteVarMarginal(0);
  f2.PrintPotentials();

  for (auto &c : f1.set_combinations) {
    double diff_abs = abs(f1.map_potentials[c] - f2.map_potentials[c]);
    // Use EQ may fail in some cases where they should pass.
    EXPECT_LE(diff_abs,1.0E-10);
  }
}


TEST_F(NetworkTest, jun_tree_accuracy) {

  auto *jt = new JunctionTree(network, false);

  ScoreFunction sf(network, trainer);
  sf.PrintAllScore();
  double accuracy = jt->TestNetReturnAccuracy(0,tester);
  delete jt;
  EXPECT_GT(accuracy,0.7350);
}


TEST_F(NetworkTest, likelihood_weighting_accuracy) {
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


TEST_F(NetworkTest, sampling_node) {
  Node *n_39 = network->FindNodePtrByIndex(39);
  //Factor fac(n_39);
  //fac.PrintPotentials();

  DiscreteConfig e;
  e.insert(pair<int,int>(0,-1));
  int count_0 = 0;
  for (int i=0; i<10000; ++i) {
    if(0 == dynamic_cast<DiscreteNode*>(n_39)->SampleNodeGivenParents(e)) {
      ++count_0;
    }
  }
  double rate_0 = ((double)count_0) / ((double)10000);
  fprintf(stdout, "%f", rate_0);
  EXPECT_GT(rate_0, 0.8);
  EXPECT_LT(rate_0, 0.86);
}

TEST_F(NetworkTest, DISABLED_sampling_network) {
  DiscreteConfig samp = network->ProbLogicSampleNetwork();
}

TEST(CustomNetworkTest, sampling_dog_net_to_csv_file_and_relearn_params) {

  string custom_file = "../../data/interchange-format-file/dog-problem.xml";
  auto custom_net = new CustomNetwork(true);
  custom_net->GetNetFromXMLBIFFile(custom_file);

  vector<string> names;
  for (int i = 0; i < custom_net->num_nodes; ++i) {
    names.push_back(custom_net->FindNodePtrByIndex(i)->node_name);
  }
  string sample_file = "../../data/dataset/dog_problem_dataset_by_gibbs_sampling.csv";
//  vector<DiscreteConfig> samples = custom_net->DrawSamplesByGibbsSamp(5e4, 2e5);
//  auto trainer = new Dataset();
//  trainer->SamplesToCSVFile(samples, sample_file, names);

  CustomNetwork *net_samp = new CustomNetwork(true);
  net_samp->GetNetFromXMLBIFFile(custom_file);
  net_samp->ClearParams();
  Dataset *trn_samp = new Dataset();
  trn_samp->LoadCSVDataAutoDetectConfig(sample_file);
  net_samp->LearnParamsKnowStructCompData(trn_samp, false);
  for(int i=0; i<net_samp->num_nodes; ++i) {
    fprintf(stdout, "\n====================================\n");
    Factor f1(dynamic_cast<DiscreteNode*>(custom_net->FindNodePtrByIndex(i)), custom_net),
           f2(dynamic_cast<DiscreteNode*>(net_samp->FindNodePtrByIndex(i)), net_samp);
    f1.PrintPotentials();
    f2.PrintPotentials();
  }


  net_samp->ClearParams();
  net_samp->LearnParamsKnowStructCompData(trn_samp, false);
  for(int i=0; i<net_samp->num_nodes; ++i) {
    fprintf(stdout, "\n====================================\n");
    Factor f1(dynamic_cast<DiscreteNode*>(custom_net->FindNodePtrByIndex(i)), custom_net),
            f2(dynamic_cast<DiscreteNode*>(net_samp->FindNodePtrByIndex(i)), net_samp);
    f1.PrintPotentials();
    f2.PrintPotentials();
  }



  Dataset *dts = new Dataset();
  dts->LoadCSVDataAutoDetectConfig("./dog_problem_dataset_by_gibbs_sampling.csv", true, 0);
  Network *net = new Network();
  net->StructLearnCompData(dts, true);
}

TEST(CustomNetworkTest, dog_net_struct_learn_ott) {
  Dataset *dts = new Dataset();
  dts->LoadCSVDataAutoDetectConfig("./dog_problem_dataset_by_gibbs_sampling.csv", true, 0);
  Network *net = new Network();
  net->StructLearnCompData(dts, true, "ott");
//  net->LearnParamsKnowStructCompData(dts);
  ScoreFunction sf(net, dts);
  sf.PrintAllScore();

  cout << "=====================================" << endl;

  net->SetParentChild(0, 1);
  net->PrintEachNodeParents();
  ScoreFunction sff(net, dts);
  sff.PrintAllScore();

  cout << "=====================================" << endl;

  net->ClearStructure();
  net->PrintEachNodeParents();
  ScoreFunction sf2(net, dts);
  sf2.PrintAllScore();


  cout << "=====================================\n"
       << "Checking score with Weka" << endl;

  net->ClearStructure();
  net->SetParentChild(4, 0);
  net->SetParentChild(2, 1);
  net->SetParentChild(3, 2);
  net->SetParentChild(2, 4);
  net->GetTopoOrd();
  net->GenDiscParCombsForAllNodes();
  net->PrintEachNodeParents();
  ScoreFunction sf3(net, dts);
  sf3.PrintAllScore();


  cout << "=====================================\n"
       << "Checking score with Weka" << endl;

  net->ClearStructure();
  net->SetParentChild(4, 0);
  net->SetParentChild(2, 1);
  net->SetParentChild(4, 1);
  net->SetParentChild(2, 3);
  net->SetParentChild(2, 4);
  net->GetTopoOrd();
  net->GenDiscParCombsForAllNodes();
  net->PrintEachNodeParents();
  ScoreFunction sf4(net, dts);
  sf4.PrintAllScore();


  cout << "=====================================\n"
       << "Checking score with Weka" << endl;
  net->ClearStructure();
  net->SetParentChild(4, 0);
  net->SetParentChild(3, 1);
  net->SetParentChild(3, 2);
  net->SetParentChild(4, 2);
  net->SetParentChild(1, 2);
  net->SetParentChild(3, 4);
  net->GetTopoOrd();
  net->GenDiscParCombsForAllNodes();
  net->PrintEachNodeParents();
  ScoreFunction sf5(net, dts);
  sf5.PrintAllScore();


  cout << "=====================================\n"
       << "Checking score with Weka" << endl;
  net->ClearStructure();
  net->SetParentChild(0, 1);
  net->SetParentChild(0, 2);
  net->SetParentChild(1, 2);
  net->SetParentChild(2, 3);
  net->SetParentChild(0, 4);
  net->SetParentChild(1, 4);
  net->SetParentChild(2, 4);
  net->GetTopoOrd();
  net->GenDiscParCombsForAllNodes();
  net->PrintEachNodeParents();
  ScoreFunction sf6(net, dts);
  sf6.PrintAllScore();
}

TEST(CustomNetworkTest, dog_net_struct_learn_k2_weka) {
  Dataset *dts = new Dataset();
  dts->LoadCSVDataAutoDetectConfig("./dog_problem_dataset_by_gibbs_sampling.csv", true, 0);
  Network *net = new Network();
  net->StructLearnCompData(dts, true, "k2-weka", "dataset-ord");
//  net->LearnParamsKnowStructCompData(dts);
  ScoreFunction sf(net, dts);
  sf.PrintAllScore();


  cout << "=====================================\n"
       << "Checking score with Weka" << endl;

//  Bayes Network Classifier
//  not using ADTree
//  #attributes=5 #classindex=0
//  Network structure (nodes followed by parents)
//  light-on(2):
//  bowel-problem(2): light-on
//  dog-out(2): light-on bowel-problem
//  hear-bark(2): dog-out
//  family-out(2): light-on dog-out bowel-problem
//  LogScore Bayes: -81384.87607100065
//  LogScore BDeu: -81413.25712910524
//  LogScore MDL: -81430.32582140683
//  LogScore ENTROPY: -81338.35770598934
//  LogScore AIC: -81355.35770598934

  net->ClearStructure();
  net->SetParentChild(0, 1);
  net->SetParentChild(0, 2);
  net->SetParentChild(1, 2);
  net->SetParentChild(2, 3);
  net->SetParentChild(0, 4);
  net->SetParentChild(1, 4);
  net->SetParentChild(2, 4);

  net->GetTopoOrd();
  net->GenDiscParCombsForAllNodes();
  net->PrintEachNodeParents();
  ScoreFunction sf3(net, dts);
  sf3.PrintAllScore();

}

TEST(CustomNetworkTest, DISABLED_a1a_struct_learn) {
  // The program takes too much time.
  Dataset *dts = new Dataset();
  dts->LoadCSVDataAutoDetectConfig("../../data/dataset/Phishing/Phishing_Training_Dataset_split_1_of_10.arff.csv");
  Network *net = new Network();
  net->StructLearnCompData(dts, true, "k2-weka", "dataset-ord");
//  net->LearnParamsKnowStructCompData(dts);
  ScoreFunction sf(net, dts);
  sf.PrintAllScore();
}