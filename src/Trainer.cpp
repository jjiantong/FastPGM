//
// Created by Linjian Li on 2018/11/29.
//

#include "Trainer.h"

Trainer::Trainer() {
	// do something
}

void Trainer::loadConfig(string configFilePath) {

	ifstream inFile;
	inFile.open(configFilePath);
	if (!inFile.is_open()) {
		cerr << "Unable to open config file!";
		exit(1);   // call system to stop
	}

	cout << "Config file opened. Begin to load config." << endl;

	inFile >> numOfTrainingSamples >> numOfFeatures;

	featuresNames = new string[numOfFeatures];
	for(int i=0; i<numOfFeatures; i++) {
		inFile >> featuresNames[i];
	}

	isFeaturesDiscrete = new bool[numOfFeatures];
	for(int i=0; i<numOfFeatures; i++) {
		inFile >> isFeaturesDiscrete[i];
	}

	numOfPossibleValuesOfFeatures = new int[numOfFeatures];
	for(int i=0; i<numOfFeatures; i++) {
		inFile >> numOfPossibleValuesOfFeatures[i];
	}

	inFile.close();

	cout << "Number of training samples: " << numOfTrainingSamples << "\n"
		 << "Number of features: " << numOfFeatures << endl;

}

void Trainer::loadLibSVMData(string dataFilePath) {

	// Initialize trainingSet to be all zero.
	trainingSet = new int* [numOfTrainingSamples];
	for (int i=0; i<numOfTrainingSamples; i++) {
		trainingSet[i] = new int[numOfFeatures]();	// The parentheses at end will initialize the array to be all zeros.
	}

	ifstream inFile;
	inFile.open(dataFilePath);
	if (!inFile.is_open()) {
		cerr << "Unable to open config file!";
		exit(1);   // call system to stop
	}

	cout << "Data file opened. Begin to load data." << endl;

	string sample;
	vector<string> parsedSample;
	vector<string> parsedFeature;
	vector<string>::iterator it;
	int index, value;

	// Load data into trainingSet.
	for (int i=0; i<numOfTrainingSamples; i++) {
		getline(inFile, sample);
		sample = boost::algorithm::trim_right_copy(sample);	// There is a whitespace at the end of each line of libSVM data set format, which will will cause a bug.
		boost::algorithm::split(parsedSample, sample, boost::algorithm::is_space());
		it=parsedSample.begin();
		trainingSet[i][0] = stoi(*it);
		//cout << trainingSet[i][0] << '\t'; // todo delete
		it++;
		for ( ; it!=parsedSample.end(); it++) {
			boost::algorithm::split(parsedFeature, *it, boost::algorithm::is_any_of(":"));
			index = stoi(parsedFeature[0]);
			value = stoi(parsedFeature[1]);
			trainingSet[i][index] = value;
			//cout << index << ':' << trainingSet[i][index] << '\t'; // todo delete
		}
		//cout << "Sample " << i << " loaded." << endl; // todo delete

	}

	cout << "Finish loading data." << endl;


	inFile.close();

}