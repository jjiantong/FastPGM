//
// Created by llj on 4/4/19.
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


class ScoreTest : public ::testing::Test {
 protected:
  void SetUp() override {

    origin_dog_net = new CustomNetwork();
    incomplete_dog_net = new CustomNetwork();
    redundant_dog_net = new CustomNetwork();
    origin_dog_net->pure_discrete = true;
    incomplete_dog_net->pure_discrete = true;
    redundant_dog_net->pure_discrete = true;


    origin_dog_net->GetNetFromXMLBIFFile("../../data/interchange-format-file/dog-problem.xml");
    incomplete_dog_net->GetNetFromXMLBIFFile("../../data/interchange-format-file/incomplete-dog-problem.xml");
    redundant_dog_net->GetNetFromXMLBIFFile("../../data/interchange-format-file/redundant-dog-problem.xml");

    vector<Combination> samples = origin_dog_net->DrawSamplesByProbLogiSamp(5000);
    trainer = new Trainer();
    string samp_to_file = "origin-dog-net-samples-to-libsvm.txt";
    trainer->SamplesToLIBSVMFile(samples, samp_to_file);
    trainer->LoadLIBSVMDataAutoDetectConfig(samp_to_file);


  }

  Trainer *trainer;
  CustomNetwork *origin_dog_net;
  CustomNetwork *incomplete_dog_net;
  CustomNetwork *redundant_dog_net;

};


TEST_F(ScoreTest, score_for_diff_nets) {
  ScoreFunction sf(origin_dog_net,trainer);
  ScoreFunction incomp_sf(incomplete_dog_net,trainer);
  ScoreFunction redun_sf(redundant_dog_net,trainer);

  EXPECT_GT(sf.LogLikelihood(),incomp_sf.LogLikelihood());
  EXPECT_GT(sf.AIC(),incomp_sf.AIC());
  EXPECT_GT(sf.BIC(),incomp_sf.BIC());
  EXPECT_GT(sf.K2(),incomp_sf.K2());
  EXPECT_GT(sf.BDeu(),incomp_sf.BDeu());

  EXPECT_GT(sf.LogLikelihood(),redun_sf.LogLikelihood());
  EXPECT_GT(sf.AIC(),redun_sf.AIC());
  EXPECT_GT(sf.BIC(),redun_sf.BIC());
  EXPECT_GT(sf.K2(),redun_sf.K2());
  EXPECT_GT(sf.BDeu(),redun_sf.BDeu());

  cout << "origin : incomplete : redundant\n"
       << sf.LogLikelihood() << " : " << incomp_sf.LogLikelihood() << " : " << redun_sf.LogLikelihood() << '\n'
       << sf.AIC() << " : " << incomp_sf.AIC() << " : " << redun_sf.AIC() << '\n'
       << sf.BIC() << " : " << incomp_sf.BIC() << " : " << redun_sf.BIC() << '\n'
       << sf.K2()  << " : " << incomp_sf.K2()  << " : " << redun_sf.K2() << '\n'
       << sf.BDeu() << " : " << incomp_sf.BDeu() << " : " << redun_sf.BDeu() << endl;

  // The test result indicates that some scoring functions prefer complex network
}