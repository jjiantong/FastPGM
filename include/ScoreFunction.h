//
// Created by llj on 3/12/19.
//

#ifndef BAYESIANNETWORK_SCOREFUNCTION_H
#define BAYESIANNETWORK_SCOREFUNCTION_H

#include <iostream>
#include <set>
#include <queue>
#include <vector>
#include <cmath>

#include "Network.h"
#include "Dataset.h"
#include "Node.h"
#include "Factor.h"
#include "gadget.h"

/**
 * IMPORTANT!
 * In this project, higher score means better structure.
 * That is, the aim is to MAXIMIZE the score.
 */
class ScoreFunction {
 public:

  ScoreFunction(Network *net, Dataset *dts);
  double ScoreForNode(Node *node_ptr, string metric);

  double LogLikelihoodForNode(Node *node_ptr);
  double LogLikelihood();

  double LogK2ForNode(Node*);
  double LogK2();
  double K2();

  double LogBDeuForNode(Node*, int equi_sample_size);
  double LogBDeu(int equi_sample_size = 10);
  double BDeu(int equi_sample_size = 10);

  double AICForNode(Node*);
  double AIC();

  double BICForNode(Node*);
  double BIC();

  double MDLForNode(Node*);
  double MDL();

  void PrintAllScore();

 private:
  ScoreFunction() = default;
  Network *network;
  Dataset *dataset;
};


#endif //BAYESIANNETWORK_SCOREFUNCTION_H
