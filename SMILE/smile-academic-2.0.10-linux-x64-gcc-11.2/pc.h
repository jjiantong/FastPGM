#ifndef SMILE_PC_H
#define SMILE_PC_H

// {{SMILE_PUBLIC_HEADER}}

#include "bkgndknowledge.h"

class DSL_dataset;
class DSL_progress;
class DSL_network;
class DSL_pattern;
class DSL_cormat;

class DSL_pc
{
public:
    DSL_pc()
    {
        maxcache = 2048;
        maxAdjacency = 8;
        maxSearchTime = 0;
        significance = 0.05;
    }
    
    unsigned long maxcache;
    int maxAdjacency;
    int maxSearchTime;
    double significance;
    
	DSL_bkgndKnowledge bkk;
    
    int Learn(const DSL_dataset &ds, DSL_pattern &pat, DSL_progress *progress = NULL) const;
    
    // not for public use
    int Dbcml(const DSL_dataset &ds, int maxDeriv, std::vector<std::vector<int> > &derivs, DSL_pattern &pat, DSL_dataset *dsderivs, DSL_progress *progress) const;
    int DbcmlPc(DSL_cormat &cm, DSL_pattern &pat, std::vector<std::vector<std::vector<int> > > &sepsets) const;

private:	
	int Idx(int nvar, int var, int deriv, int time) const;
    std::vector<int> nc;
};

#endif
