//
// Created by Linjian Li on 2018/11/29.
//

#ifndef BAYESIANNETWORK_TRAINER_H
#define BAYESIANNETWORK_TRAINER_H

#include <string>
#include <set>
#include <vector>
#include <utility>
#include <iostream>
#include <fstream>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;

typedef set< pair<string, int> > Combination;

class Trainer {

public:
	int numOfTrainingSamples;
	int numOfFeatures;
	string* featuresNames;
	bool* isFeaturesDiscrete;
	int* numOfPossibleValuesOfFeatures;
	int** trainingSet; // For now, the training set contains only discrete values. Future work is to let it support continuous values;
	// incompleteData;
	// incompleteDataExpectation;


	Trainer();
	void loadConfig(string);
	void loadLibSVMData(string);
};


#endif //BAYESIANNETWORK_TRAINER_H
