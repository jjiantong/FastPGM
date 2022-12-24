#ifndef SMILE_DEFCPT_H
#define SMILE_DEFCPT_H

// {{SMILE_PUBLIC_HEADER}}

#include "discdef.h"
#include "dmatrix.h"

// represents a discrete & non-deterministic chance node.

class DSL_cpt : public DSL_discDef
{
public:
	DSL_cpt(DSL_network& network, int handle);
	DSL_cpt(const DSL_nodeXformContext& xfparams);

	int GetType() const { return DSL_CPT; }
	const char* GetTypeName() const { return "CPT"; }
	
	const DSL_Dmatrix& GetProbabilities() const { return table; }

	const DSL_Dmatrix* GetTemporalProbabilities(int order) const;
	int SetTemporalProbabilities(int order, const DSL_Dmatrix& probs);
	int SetTemporalProbabilities(int order, const DSL_doubleArray& probs);

	int MakeUniform();
	int MakeRandom(DSL_randGen &r);

protected:
	int AddParent(int theParent);
	int RemoveParent(int theParent);
	int OnParentOutcomeAdd(int parentHandle, int thisPosition);
	int OnParentOutcomeRemove(int parentHandle, int thisPosition);
	int OnParentOutcomeReorder(int parentHandle, const DSL_intArray& newOrder);
	int OnParentReorder(const DSL_intArray& newOrder);

	int AfterInsertOutcome(int outcomeIndex);
	int AfterRemoveOutcome(int outcomeIndex);
	int AfterReorderOutcomes(const DSL_intArray& newOrder);
	int AfterOutcomeCountChanged();

	DSL_cpt(const DSL_cpt& src, DSL_network& targetNetwork);
	DSL_nodeDef* Clone(DSL_network& targetNetwork) const;
	void InitXform(DSL_nodeXformContext &xfp) const;

	const DSL_Dmatrix* GetParameters() const { return &table; }
	DSL_Dmatrix* GetParameters() { return &table; }

	DSL_Dmatrix table; // conditional probabilities
};

#endif
