#ifndef SMILE_VALBELIEFVECTOR_H
#define SMILE_VALBELIEFVECTOR_H

// {{SMILE_PUBLIC_HEADER}}

#include "discval.h"

class DSL_beliefVector : public DSL_discVal
{
public:
	DSL_beliefVector(DSL_network& network, int handle);
	DSL_beliefVector(const DSL_valXformContext& ctx);

	int GetType() const { return DSL_BELIEFVECTOR; }

	int SetVirtualEvidence(const std::vector<double> &evidence);
	int SetVirtualEvidence(const DSL_doubleArray &evidence);
	int GetVirtualEvidence(std::vector<double> &evidence) const;
	int GetVirtualEvidence(DSL_doubleArray &evidence) const;

	const DSL_Dmatrix &GetBeliefs() const { return values; }

	// manipulation 
	int ControlValue(int theValue);
	int ClearControlledValue();
	int GetControlledValue() const { return IsControlled() ? evidence : DSL_OUT_OF_RANGE; }
	int IsControlled() const { return (0 != IsFlagSet(DSL_VALUE_CONTROLLED)) && IsEvidence(); }
	bool IsControllable() const;

	void InitXform(DSL_valXformContext& ctx) const;

private:
	DSL_beliefVector(const DSL_beliefVector& src, DSL_network& targetNetwork);
	DSL_nodeVal* Clone(DSL_network& targetNetwork) const;
	~DSL_beliefVector();

	int ClearVirtualEvidence();
	int DoSetVirtualEvidence(const double* src, int count);

	std::vector<double> *virtualEvidence;
};

#endif
