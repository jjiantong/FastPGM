#ifndef SMILE_TAN_H
#define SMILE_TAN_H

// {{SMILE_PUBLIC_HEADER}}

#include <string>
#include <vector>
#include "mst.h"

class DSL_dataset;
class DSL_network;
class DSL_progress;

class DSL_tan
{
public:
	DSL_tan();
	int Learn(DSL_dataset &ds, DSL_network &net, DSL_progress *progress = NULL, double *emLogLik = NULL) const;
	std::string classvar;
	int maxSearchTime;
	unsigned int seed;
	unsigned long maxcache;
private:
	int PreChecks(const DSL_dataset &ds_, int &cvar) const;
	void FinishStructure(std::vector<DSL_mst::weight> &result, const DSL_dataset &ds, DSL_network &net, const int &cvar, int seed) const;
	void CalcWeights(const DSL_dataset &ds, const int &cvar, std::vector<DSL_mst::weight> &edges) const;
	int LogPreCheckError(const DSL_dataset &ds_, int varIdx, const char *error) const;
};

#endif
