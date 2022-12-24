#ifndef SMILE_VALEQUATIONEVALUATION_H
#define SMILE_VALEQUATIONEVALUATION_H

// {{SMILE_PUBLIC_HEADER}}

#include "nodeval.h"
#include "dmatrix.h"
#include <vector>


class DSL_equationEvaluation : public DSL_nodeVal
{
public:
    DSL_equationEvaluation(DSL_network& network, int handle);
    DSL_equationEvaluation(const DSL_valXformContext& ctx);

    int GetType() const { return DSL_EQUATIONEVALUATION; }

    int AddIndexingParent(int parent) { return DSL_WRONG_NODE_TYPE; }
    const DSL_Dmatrix* GetMatrix() const { return &discBeliefs; }
    DSL_Dmatrix* GetWriteableMatrix() { return &discBeliefs; }

    int GetMean(double& mean) const;
    int GetStdDev(double& stddev) const;

    const std::vector<double>& GetSamples() const { return samples; }
	double GetSample(int index) const { return samples[index]; }

    int GetHistogram(double lo, double hi, int binCount, std::vector<int> &histogram) const;

	double GetSampleMean() const;
    double GetSampleStdDev() const;
	void GetStats(double &mean, double &stddev, double &vmin, double &vmax) const;

	bool HasSamplesOutOfBounds() const;

    void SamplingStart(int samplesToReserve = 0);
	void AddSample(double sample) { samples.push_back(sample); }
    int SamplingEnd();

	// used only for exact evaluation
	void SetSampleMean(double mean);

    bool IsDiscretized() const { return !discBeliefs.IsEmpty(); }
    const DSL_Dmatrix& GetDiscBeliefs() const { return discBeliefs; }

    int SetEvidence(int evidence);
    int SetEvidence(double evidence);
    int SetPropagatedEvidence(double evidence);
    int GetEvidence(double& e) const;
    int ClearEvidence();
    int ClearPropagatedEvidence();

    void InitXform(DSL_valXformContext& ctx) const;

private:
    DSL_equationEvaluation(const DSL_equationEvaluation& src, DSL_network& targetNetwork);
    DSL_nodeVal* Clone(DSL_network& targetNetwork) const;

    void CalcStats() const;

    double evidence;

    std::vector<double> samples; 
    
	// if equation is determinisitc (i.e., all its ancestor is deterministic), 
    // sampleMean is its value and sampleStdDev is zero
    mutable double sampleMean;
    mutable double sampleStdDev;
	mutable double sampleMin;
	mutable double sampleMax;

	mutable bool statsCalculated;

	DSL_Dmatrix discBeliefs;
};

#endif

