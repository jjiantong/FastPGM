#ifndef SMILE_VALPOLICYVALUES_H
#define SMILE_VALPOLICYVALUES_H

// {{SMILE_PUBLIC_HEADER}}

#include "discval.h"

class DSL_policyValues : public DSL_discVal
{
public:
	DSL_policyValues(DSL_network& network, int handle);
	DSL_policyValues(const DSL_valXformContext &ctx);
	int GetType() const { return DSL_POLICYVALUES; }

	const DSL_Dmatrix& GetPolicyValues() const { return values; }

private:
	DSL_policyValues(const DSL_policyValues& src, DSL_network &targetNetwork);
	DSL_nodeVal* Clone(DSL_network& targetNetwork) const;
};

#endif
