#ifndef SMILE_EM_H
#define SMILE_EM_H

// {{SMILE_PUBLIC_HEADER}}

/* This class implements the EM algorithm
 * 
 * IMPORTANT: THE STATE INDICES IN THE DATA SET SHOULD MATCH THE STATE INDICES IN THE NETWORK
 *            IT DOES *NOT* AUTOMATICALLY MATCH THE STATE NAMES
 *            THIS IMPLIES THAT YOU PROBABLY HAVE TO REORDER THE STATES IN THE DATA SET
 *            (DSL_dataset::MatchNetwork can help in reordering)   
 * 
 * The equivalent sample size determines the weight of the network
 * against the data. An equivalent sample size of 100, means that
 * the current parameters in the network are based on 100 cases.
 * A typical equivalent sample size is 0, this means that the current
 * parameters have no weight and will be overwritten.
 * 
 * The variables in the network and data set should be all discrete or all continuous.
 */

#include "dataset.h"

class DSL_network;
class DSL_progress;

class DSL_em
{
public:
	DSL_em() : eqSampleSize(0), uniParams(false), randParams(true), randSeed(0), relevance(false), autoSlices(true), learningrate(0.6f), initLearningPoint(1) {}
    
	int Learn(const DSL_dataset& ds, DSL_network& orig, const std::vector<DSL_datasetMatch> &matches, double *loglik = NULL, DSL_progress *progress = 0);
    int Learn(const DSL_dataset& ds, DSL_network& orig, const std::vector<DSL_datasetMatch> &matches, const std::vector<int> &fixedNodes, double *loglik = NULL, DSL_progress *progress = 0);
	
	int LearnOnlineEM(const DSL_dataset& ds, DSL_network& orig, const std::vector<DSL_datasetMatch> &matches, double *loglik = NULL, DSL_progress *progress = 0);
	int LearnOnlineEM(const DSL_dataset& ds, DSL_network& orig, const std::vector<DSL_datasetMatch> &matches, const std::vector<int> &fixedNodes, double *loglik = NULL, DSL_progress *progress = 0);

    int SetEquivalentSampleSize(float eqs);
	float GetEquivalentSampleSize() const { return eqSampleSize; }
	void SetUniformizeParameters(bool u);
	bool GetUniformizeParameters() const { return uniParams; }
	void SetRandomizeParameters(bool r);
	bool GetRandomizeParameters() const { return randParams; }
	int GetSeed() const { return randSeed; }
	void SetSeed(int seed) { randSeed = seed; }
	void SetRelevance(bool r) { relevance = r; }
	bool GetRelevance() { return relevance; }
	bool GetAutoSlices() const { return autoSlices; }
	void SetAutoSlices(bool a) { autoSlices = a; }

	void SetLearningRate(float lr) { learningrate = lr; }
	float GetLearningRate() const { return learningrate; }
	void SetInitialLearningPoint(int ilp) { initLearningPoint = ilp;}
	int GetInitialLearningPoint() const {return initLearningPoint; }

private:
	int LearnDisc(const DSL_dataset& ds, DSL_network& orig, const std::vector<DSL_datasetMatch> &matches, const std::vector<int> &fixedNodes, double *loglik, DSL_progress *progress = 0);
	int LearnCont(const DSL_dataset& ds, DSL_network& orig, const std::vector<DSL_datasetMatch> &matches, const std::vector<int> &fixedNodes, double *loglik, DSL_progress *progress = 0);
    int LearnDiscOnlineEM(const DSL_dataset& ds, DSL_network& orig, const std::vector<DSL_datasetMatch> &matches, const std::vector<int> &fixedNodes, double *loglik, DSL_progress *progress = 0);

	float eqSampleSize;
	bool uniParams;
	bool randParams;
	int randSeed;
	bool relevance;
	bool autoSlices;
	float learningrate;
	int initLearningPoint;
};

#endif
