#ifndef SMILE_DEFDEMORGAN_H
#define SMILE_DEFDEMORGAN_H

// {{SMILE_PUBLIC_HEADER}}

#include "qualdef.h"

// different types of parents

#define DSL_DEMORGAN_INHIBITOR     0
#define DSL_DEMORGAN_REQUIREMENT   1
#define DSL_DEMORGAN_CAUSE         2
#define DSL_DEMORGAN_BARRIER       3

class DSL_demorgan : public DSL_qualDef
{
public:
	DSL_demorgan(DSL_network& network, int handle);
	DSL_demorgan(const DSL_nodeXformContext& context);

	const char* GetTypeName() const { return "DEMORGAN"; }
	int GetType() const { return DSL_DEMORGAN; }

	int SetParentWeights(const DSL_doubleArray& weights);
	int SetParentWeight(int numParent, double value);
	double GetParentWeight(int numParent) const;
	const DSL_doubleArray& GetParentWeights() const { return parentWeights; }

	int SetParentTypes(const DSL_intArray& types);	
	int SetParentType(int numParent, int value);
	int	GetParentType(int numParent) const;
	const DSL_intArray& GetParentTypes() const { return parentTypes; }

    int GetTemporalParentType(int order, int numParent) const;
    int SetTemporalParentType(int order, int numParent, int parentType);
    double GetTemporalParentWeight(int order, int numParent) const;
    int SetTemporalParentWeight(int order, int numParent, double weight);

	double GetPriorBelief() const { return priorBelief; }
	int SetPriorBelief(double value);

private:
	DSL_demorgan(const DSL_demorgan& src, DSL_network& targetNetwork);
	DSL_nodeDef* Clone(DSL_network& targetNetwork) const;
	void DoCopyParameters(DSL_nodeDef& target) const;

	int AddParent(int theParent);
	int RemoveParent(int theParent);
	int OnParentReorder(const DSL_intArray& newOrder);

	double CalculateProbability(const DSL_intArray &parentStates) const;

    int ConvertTemporalParentIndex(int order, int parentIndex) const;

	DSL_intArray parentTypes;		// stores the type of each parent
	DSL_doubleArray parentWeights;	// stores the weight of each parent
	double priorBelief;			// stores the prior belief for this node
};

#endif
