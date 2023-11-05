////
//// Created by jjt on 6/10/22.
////
//
//#ifndef BAYESIANNETWORK_BRUTEFORCE_H
//#define BAYESIANNETWORK_BRUTEFORCE_H
//
//#include "Inference.h"
//
//class BruteForce : public Inference {
//public:
//    BruteForce(Network *net, Dataset *dts, bool is_dense): Inference(net, dts, is_dense) {};
//
//    virtual double EvaluateAccuracy(string path, int num_threads);
//
//protected:
//    int PredictDirectly(const DiscreteConfig &E);
//    vector<int> PredictDirectly();
//    map<int, double> GetMarginalProbabilitiesDirectly(const DiscreteConfig &evidence);
//};
//
//#endif //BAYESIANNETWORK_BRUTEFORCE_H
