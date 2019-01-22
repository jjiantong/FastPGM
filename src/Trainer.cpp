//
// Created by Linjian Li on 2018/11/29.
//

#include "Trainer.h"

Trainer::Trainer() {
	// do something
}


void Trainer::loadLIBSVMData_AutoDetectConfig(string dataFilePath) {

	int maxIndexOccurred = -1;
	set<int> classPossibleValues_set;
	map<int,set<int>> featuresPossibleValues_map;

	ifstream inFile;
	inFile.open(dataFilePath);
	if (!inFile.is_open()) {
		fprintf(stderr, "Unable to open file %s!", dataFilePath.c_str());
		exit(1);   // call system to stop
	}

	cout << "Data file opened. Begin to load data." << endl;

	string sample;
	vector<string> parsedSample;
	vector<string> parsedFeature;
	vector<string>::iterator it;
	int index, value;

	// Load data into trainingSet.
	getline(inFile, sample);
	while (!inFile.eof()) {
		// There is a whitespace at the end of each line of libSVM data set format, which will will cause a bug.
		sample = boost::algorithm::trim_right_copy(sample);
		boost::algorithm::split(parsedSample, sample, boost::algorithm::is_space());
		it=parsedSample.begin();
		classPossibleValues_set.insert(stoi(*it));
		trainingSet_y_vector.push_back(stoi(*it));
		it++;
		vector<pair<int,int>> singleSample_vector;
		for ( ; it!=parsedSample.end(); it++) {
			boost::algorithm::split(parsedFeature, *it, boost::algorithm::is_any_of(":"));
			index = stoi(parsedFeature[0]);
			value = stoi(parsedFeature[1]);
			maxIndexOccurred = index>maxIndexOccurred ? index : maxIndexOccurred;
			featuresPossibleValues_map[index].insert(value);
			pair<int,int> featureValue_pair;
			featureValue_pair.first = index;
			featureValue_pair.second = value;
			singleSample_vector.push_back(featureValue_pair);
		}
		trainingSet_X_vector.push_back(singleSample_vector);
		getline(inFile, sample);
	}


	n_training_instance = trainingSet_y_vector.size();
	n_feature = maxIndexOccurred+1;	// Include the class variable.
	isFeaturesDiscrete = new bool[n_feature];
	featuresNames = new string[n_feature];
	numOfPossibleValuesOfFeatures = new int[n_feature];
	numOfPossibleValuesOfClass = classPossibleValues_set.size();


	for (int i=0; i<n_feature; i++) {
		// Note that the feature indexes start at 1 in LIBSVM format!!!
		featuresPossibleValues_map[i+1].insert(0);	// Because feature of LIBSVM format do not record the value of 0, we need to add it in.
		numOfPossibleValuesOfFeatures[i] = featuresPossibleValues_map[i+1].size();
		isFeaturesDiscrete[i] = true; // For now, we can only apply to discrete features.
		featuresNames[i] = to_string(i);		// Because, for now, names are simply the feature indexes.
	}

	convertVectorDatasetIntoArrayDataset();

	cout << "Finish loading data." << '\n'
		 << "Number of training samples: " << n_training_instance << '\n'
		 << "Number of features (maximum feature index occurred): " << n_feature << endl;


	inFile.close();

}


void Trainer::convertVectorDatasetIntoArrayDataset() {
	// Initialize trainingSet to be all zero.
	trainingSet = new int* [n_training_instance];
	for (int i=0; i<n_training_instance; i++)
		trainingSet[i] = new int[n_feature]();	// The parentheses at end will initialize the array to be all zeros.

	for (int i=0; i<n_training_instance; i++) {	// For each sample.
		trainingSet[i][0] = trainingSet_y_vector[i];
		vector<pair<int,int>> x_vector = trainingSet_X_vector[i];
		for (pair<int,int> p : x_vector) {		// For each non-zero-value feature of this sample.
			trainingSet[i][p.first] = p.second;
		}
	}
}