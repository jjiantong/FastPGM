#ifndef SMILE_LAZYDEF_H
#define SMILE_LAZYDEF_H

// {{SMILE_PUBLIC_HEADER}}

#include "discdef.h"
#include "dmatrix.h"

// base class for constrained discrete chance nodes
// CPT calculated on demand

class DSL_lazyDef : public DSL_discDef
{
public:
	const DSL_Dmatrix& GetProbabilities() const { EnsureCptValid(); return cpt; }
	const DSL_Dmatrix* GetMatrix() const { return &GetProbabilities(); }
	int RawDefinition(DSL_Dmatrix** parameters);

#ifndef SMILE_NO_V1_COMPATIBILITY
	const DSL_Dmatrix& GetCpt() const { return GetProbabilities(); }
#endif
	
protected:
	DSL_lazyDef(DSL_network& network, int handle, const DSL_idArray* outcomeIds = NULL);
	DSL_lazyDef(const DSL_nodeXformContext& context);
	DSL_lazyDef(const DSL_lazyDef& src, DSL_network& targetNetwork);

	void InitXform(DSL_nodeXformContext &xfp) const;
	void DoCopyParameters(DSL_nodeDef& target) const;

	virtual void CalculateCpt() const = 0;
	void InvalidateCpt() const { cptValid = false; }

	mutable DSL_Dmatrix cpt;

private:
	void EnsureCptValid() const;
	mutable bool cptValid;
};

#endif
