#ifndef SMILE_TRUTHTABLE_H
#define SMILE_TRUTHTABLE_H

// {{SMILE_PUBLIC_HEADER}}

#include "defcpt.h"

// represents a discrete & deterministic chance node.
// It's a CPT with only 0's and 1's

class DSL_truthTable : public DSL_cpt
{
public:
	DSL_truthTable(DSL_network& network, int handle);
	DSL_truthTable(const DSL_nodeXformContext& xfp);

	int GetType() const { return DSL_TRUTHTABLE; }
	const char *GetTypeName() const { return "TRUTHTABLE"; }

	int SetResultingStates(const DSL_stringArray& states);
	int SetResultingStates(const DSL_intArray& states);
	int GetResultingStates(DSL_intArray& states) const;
	int GetResultingStates(DSL_stringArray& states) const;

	int GetTemporalResultingStates(int order, DSL_intArray &states) const;
	int SetTemporalResultingStates(int order, const DSL_intArray &states);
  
	int MakeUniform() { return DSL_WRONG_NODE_TYPE; }
	int MakeRandom(DSL_randGen &randGen);

protected:
	DSL_truthTable(const DSL_truthTable& src, DSL_network& target);
	DSL_nodeDef* Clone(DSL_network& targetNetwork) const;

	void MakeConsistent();
	int AfterOutcomeCountChanged();
};

#endif
