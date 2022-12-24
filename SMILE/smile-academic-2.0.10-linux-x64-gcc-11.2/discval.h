#ifndef SMILE_DISCVAL_H
#define SMILE_DISCVAL_H

// {{SMILE_PUBLIC_HEADER}}

#include "nodeval.h"
#include "dmatrix.h"

class DSL_discVal: public DSL_nodeVal
{
public:
	DSL_discVal(DSL_network& network, int handle);
	DSL_discVal(const DSL_valXformContext& ctx);

	int GetEvidence() const { return IsEvidence() ? evidence : DSL_OUT_OF_RANGE; }
	const char* GetEvidenceId() const;
	int GetEvidence(double& evidence) const;
	int SetEvidence(int outcomeIndex);
	int SetEvidence(const char* outcomeId);
	int SetEvidence(double evidence);
	int ClearEvidence();

	int SetPropagatedEvidence(int outcomeIndex);
	int ClearPropagatedEvidence();

	bool IsContinuousEvidence() const { return IsEvidence() && hasContEvidence; }

	int GetMean(double& mean) const;
	int GetStdDev(double& stddev) const;
	int GetTemporalMeanStdDev(DSL_doubleArray& temporalMean, DSL_doubleArray &temporalStdDev) const;
	int GetTemporalMeanStdDev(std::vector<double>& temporalMean, std::vector<double>& temporalStdDev) const;

	const DSL_Dmatrix* GetMatrix() const { return &values; }
	DSL_Dmatrix* GetWriteableMatrix() { return &values; }

	int GetValue(const DSL_Dmatrix** here) const { *here = &values; return DSL_OKAY; }

	void InitIndexingParents();

	void InitXform(DSL_valXformContext& ctx) const;

protected:
	DSL_discVal(const DSL_discVal& src, DSL_network& targetNetwork);

	int StoreEvidence(int outcomeIndex, bool contEvidence);
	int DoSetEvidence(int outcomeIndex, bool contEvidence);

	int DoGetTemporalMeanStdDev(double *outMean, double *outStdDev) const;
	int ValidPlateValue() const;

	int evidence;
	double contEvidence;
	bool hasContEvidence;
	DSL_Dmatrix values;
};

#endif
