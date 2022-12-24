#ifndef SMILE_DBCML_H
#define SMILE_DBCML_H

// {{SMILE_PUBLIC_HEADER}}

#include <vector>
#include <string>

class DSL_dataset;
class DSL_progress;
class DSL_network;
class DSL_pattern;

class DSL_dbcml
{
 public:
    DSL_dbcml()
    {
        maxAdjacency = 8;
        significance = 0.05;
        maxDeriv = 3;
    }
    
    int maxAdjacency;
    double significance;
    int maxDeriv;
    
    int Learn(DSL_dataset &ds, std::vector<std::vector<int> > &derivs, DSL_pattern &pat, DSL_dataset *dsderivs = NULL, DSL_progress *progress = NULL) const;
    int Learn(std::vector<DSL_dataset> &dss, std::vector<std::vector<int> > &derivs, DSL_pattern &pat, DSL_dataset *dsderivs = NULL, DSL_progress *progress = NULL) const;
    
    void CalcDerivsInit(const DSL_dataset &ds, int maxDeriv, DSL_dataset &all) const;
    void CalcDerivsNext(const DSL_dataset &ds, int maxDeriv, DSL_dataset &all) const;
    
    int Idx(int nvar, int var, int deriv, int time) const;
    
 private:
};

#endif
