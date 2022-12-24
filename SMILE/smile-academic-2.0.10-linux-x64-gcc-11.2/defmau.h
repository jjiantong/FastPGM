#ifndef SMILE_DEFMAU_H
#define SMILE_DEFMAU_H

// {{SMILE_PUBLIC_HEADER}}

#include "nodedef.h"
#include "dmatrix.h"
#include <vector>

class DSL_expression;

template <class T, class Policy> class DSL_Tmatrix;
class DSL_EmatrixPolicy;
typedef DSL_Tmatrix<DSL_expression, DSL_EmatrixPolicy> DSL_Ematrix;

// MAU (multi-attribute utility)
// represents a discrete & deterministic utility node with continuous parents

class DSL_mau : public DSL_nodeDef
{
public:
	DSL_mau(DSL_network& network, int handle);
	DSL_mau(const DSL_nodeXformContext& xfp);
	~DSL_mau();

	int GetType() const { return DSL_MAU; }
	const char *GetTypeName() const { return "MAU"; }

	const DSL_Dmatrix& GetWeights() const { return weights; }
	const DSL_Ematrix* GetExpressions() const { return expressions; }
	int GetExpressions(std::vector<std::string> &e) const;
	int SetExpressions(const DSL_Ematrix *e, std::string *errMsg = NULL);
	int SetExpressions(const std::vector<std::string> &e, std::string *errMsg = NULL);
	bool ValidateExpression(const std::string &str, std::string &errMsg, int *errPos = NULL);
	void InitExpressionMatrix(DSL_Ematrix *mtx) const;
	int ConvertToExpressions();
	
	const DSL_idArray* GetOutcomeIds() const { return NULL; }

	void GetParentsSplitByType(DSL_intArray &decisions, DSL_intArray &utilities) const
	{
		GetDecisionParentsHelper(decisions, true);
		GetDecisionParentsHelper(utilities, false);
	}

    int ValidateExpressions(
        const DSL_extFunctionContainer &extFxn,
        int &errFxnIdx,
        std::string &errMsg) const;

    int PatchExpressions(const DSL_extFunctionContainer &extFxn);

private:
	DSL_mau(const DSL_mau& src, DSL_network& targetNetwork);
	DSL_nodeDef* Clone(DSL_network& targetNetwork) const;
	void InitXform(DSL_nodeXformContext& xfp) const;

	DSL_nodeVal* CreateValue() const;
	DSL_nodeVal* CreateValue(const DSL_valXformContext& ctx) const;

	int AddParent(int parentHandle);
	int RemoveParent(int parentHandle);
	int OnParentOutcomeAdd(int parentHandle, int outcomeIndex);
	int OnParentOutcomeRemove(int parentHandle, int outcomeIndex);
	int OnParentOutcomeReorder(int parentHandle, const DSL_intArray &newOrder);
	int OnParentReorder(const DSL_intArray &newOrder);
	void OnParentIdChanging(int parentHandle, const char *oldId, const char *newId);
	void GetDecisionParentsHelper(DSL_intArray &parents, bool decisions) const;
	void AfterExpressionMatrixShrink();

	const DSL_Dmatrix* GetParameters() const { return &weights; }
	DSL_Dmatrix* GetParameters() { return &weights; }

	// This node is expecting continous entries from its utility/MAU parents
	// but may be indexed by discrete decision nodes
	DSL_Dmatrix weights;    // the weights of each parent for simple linear combination
	DSL_Ematrix *expressions; // generalized expressions
};

#endif
