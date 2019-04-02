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
#include "Trainer.h"
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

  ScoreFunction(Network*, Trainer*);
  double LogLikelihoodForNode(Node*);
  double LogLikelihood();
  double K2();
  double BDe();
  double BDeu();
  double AIC();
  double BIC();
  double MDL();

 private:

  ScoreFunction() = default;

  int num_network_params = 0;
  Network *net;
  Trainer *trn;

  double LogLikelihoodForNode(Node*, Network*, Trainer*);
  double LogLikelihood(Network*, Trainer*);
  double K2(Network*, Trainer*);
  double LogK2(Network*, Trainer*);
  double BDe(Network*, Trainer*, int);
  double BDeu(Network*, Trainer*, int);
  double LogBDeu(Network*, Trainer*, int);
  double AIC(Network*, Trainer*);
  double BIC(Network*, Trainer*);
  double MDL(Network*, Trainer*);

};


#endif //BAYESIANNETWORK_SCOREFUNCTION_H
