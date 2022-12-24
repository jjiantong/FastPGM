#ifndef SMILE_BS_H
#define SMILE_BS_H

// {{SMILE_PUBLIC_HEADER}}

#include <vector>
#include "dataset.h"
#include "bkgndknowledge.h"

class DSL_progress;

class DSL_bsEvaluator
{
public:
	virtual int Evaluate(int iteration, double bsScore, double bestScore,
		DSL_network &net, const DSL_dataset &ds, const std::vector<DSL_datasetMatch> &matching, 
		DSL_progress *progress,
		double &outputScore) = 0;
};

class DSL_bs
{
public:
    DSL_bs()
    {
		maxParents = 5;
		maxSearchTime = 0;
		nrIteration = 20;
		linkProbability = 0.1;
		priorLinkProbability = 0.001;
		priorSampleSize = 50;
		seed = 0;
		ThickThinning = false;
		maxcache = 2048;	
    }
	~DSL_bs() {}
	int Learn(const DSL_dataset &ds_, DSL_network &net, 
		DSL_progress *progress = NULL, DSL_bsEvaluator *eval = NULL, 
		double *bestScore = NULL, int *bestIteration = NULL, double *emLogLik = NULL) const;

	int maxParents;
	int maxSearchTime;
	int nrIteration;
	double linkProbability;
	double priorLinkProbability;
	int priorSampleSize;
	int seed;
	bool ThickThinning;
	unsigned long maxcache;
    
	DSL_bkgndKnowledge bkk;

protected:
	int PreChecks(const DSL_dataset &ds_) const;
	void PrepareMask(const DSL_dataset &ds, std::vector< std::vector<char> > &mask) const;
};
#endif
