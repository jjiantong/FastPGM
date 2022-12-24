#ifndef SMILE_QUALITATIVEDEF_H
#define SMILE_QUALITATIVEDEF_H

// {{SMILE_PUBLIC_HEADER}}

#include "lazydef.h"
#include "dmatrix.h"

// base class for DeMorgan and CAST nodes

class DSL_qualDef : public DSL_lazyDef
{
protected:
	DSL_qualDef(DSL_network& network, int handle);
	DSL_qualDef(const DSL_nodeXformContext& context);
	DSL_qualDef(const DSL_qualDef& src, DSL_network& targetNetwork);

	bool HasFixedOutcomes() const { return true; }

	int DoAddParent(int parentHandle);
	int DoRemoveParent(int parentHandle);

	// never called
	int AfterOutcomeCountChanged() { return DSL_OKAY; }

	virtual void CalculateCpt() const;

private:
	virtual double CalculateProbability(const DSL_intArray& parentStates) const = 0;

	const DSL_Dmatrix* GetParameters() const { return NULL; }
	DSL_Dmatrix* GetParameters() { return NULL; }
};

#endif
