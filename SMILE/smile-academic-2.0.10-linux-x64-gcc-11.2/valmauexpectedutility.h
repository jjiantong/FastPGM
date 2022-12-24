#ifndef SMILE_VALMAUEXPECTEDUTILITY_H
#define SMILE_VALMAUEXPECTEDUTILITY_H

// {{SMILE_PUBLIC_HEADER}}

#include "nodeval.h"
#include "dmatrix.h"

class DSL_mauExpectedUtility : public DSL_nodeVal
{
public:
	DSL_mauExpectedUtility(DSL_network& network, int handle);
	DSL_mauExpectedUtility(const DSL_valXformContext& ctx);

	int GetType() const { return DSL_MAUEXPECTEDUTILITY; }

	DSL_Dmatrix &GetUtilities() { return expectedUtilities; }

	const DSL_Dmatrix* GetMatrix() const { return &expectedUtilities; }
	DSL_Dmatrix* GetWriteableMatrix() { return &expectedUtilities; }

	int CalculateMAU();

	void InitXform(DSL_valXformContext& ctx) const {}
	void InitIndexingParents();

private:
	DSL_mauExpectedUtility(const DSL_mauExpectedUtility& src, DSL_network& targetNetwork);
	DSL_nodeVal* Clone(DSL_network& targetNetwork) const;

	int BuildCustomVector(int forThisNode, const DSL_intArray& fromTheseCoords, DSL_intArray& here) const;
	double Utility(const DSL_doubleArray &theParentsUtilities, int startIndex);

	DSL_Dmatrix expectedUtilities; // holds the expected utility for each policy
};

#endif
