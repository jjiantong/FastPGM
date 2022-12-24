#ifndef SMILE_VALEXPECTEDUTILITY_H
#define SMILE_VALEXPECTEDUTILITY_H

// {{SMILE_PUBLIC_HEADER}}

#include "nodeval.h"
#include "dmatrix.h"

class DSL_expectedUtility : public DSL_nodeVal
{
public:
	DSL_expectedUtility(DSL_network& network, int handle);
	DSL_expectedUtility(const DSL_valXformContext& ctx);

	int GetType() const { return DSL_EXPECTEDUTILITY; }

	DSL_Dmatrix &GetUtilities() { return expectedUtilities; }

	const DSL_Dmatrix* GetMatrix() const { return &expectedUtilities; }
	DSL_Dmatrix* GetWriteableMatrix() { return &expectedUtilities; }

	int NormalizeExpectedUtilities(double origMinVal, double origMaxVal, double destMinVal, double destMaxVal);
	double GetMinimumUtility() const;
	double GetMaximumUtility() const;
	
	void InitXform(DSL_valXformContext& ctx) const {}
	void InitIndexingParents();

private:
	DSL_expectedUtility(const DSL_expectedUtility& src, DSL_network& targetNetwork);
	DSL_nodeVal* Clone(DSL_network& targetNetwork) const;

	DSL_Dmatrix expectedUtilities; // holds the expected utility for each policy
};

#endif
