#ifndef SMILE_NB_H
#define SMILE_NB_H

// {{SMILE_PUBLIC_HEADER}}

#include <string>

class DSL_dataset;
class DSL_progress;
class DSL_network;

class DSL_nb
{
public:
	int Learn(DSL_dataset ds, DSL_network &net, DSL_progress *progress = NULL, double *emLogLikelihood = NULL) const;
    std::string classVariableId;
	
private:
	int PreChecks(const DSL_dataset &ds) const;
};

#endif
