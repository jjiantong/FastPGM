#ifndef SMILE_ABN_H
#define SMILE_ABN_H

// {{SMILE_PUBLIC_HEADER}}

#include <string>
#include <vector>

class DSL_dataset;
class DSL_network;
class DSL_progress;

class DSL_abn
{
public:
    DSL_abn()
    {
		feature_selection = false;
		maxParents = 5;
        maxSearchTime = 0;
		nrIteration = 20;
		linkProbability = 0.1;
		priorLinkProbability = 0.001;
		priorSampleSize = 50;
		seed = 0;
		ThickThinning = false;
    }

	int Learn(DSL_dataset &ds, DSL_network &net, DSL_progress *progress = NULL,
		double *bestScore = NULL, int *bestIteration = NULL, double *emLogLik = NULL) const;

	std::string classvar;
	bool feature_selection;

	int maxParents;
    int maxSearchTime;
	int nrIteration;
	double linkProbability;
	double priorLinkProbability;
	int priorSampleSize;
	int seed;
	bool ThickThinning;

private:
	int PreChecks(const DSL_dataset &ds_, int &cvar) const;
};


#endif
