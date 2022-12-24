#ifndef SMILE_DEFEQUATION_H
#define SMILE_DEFEQUATION_H

// {{SMILE_PUBLIC_HEADER}}

#include "nodedef.h"
#include "generalequation.h"
#include <string>
#include <map>

// represents an equation node which can only have
// DSL_equation as its parents.


struct DSL_equationDiscretizationInput
{
	int nodeHandle;
	bool interval;
	double value[2];
};
bool operator==(const DSL_equationDiscretizationInput& lhs, const DSL_equationDiscretizationInput& rhs);


class DSL_equation : public DSL_nodeDef
{
public:
	typedef std::vector<std::pair<std::string, double> > IntervalVector;

	DSL_equation(DSL_network& network, int handle, const char* nodeId);
    DSL_equation(const DSL_nodeXformContext &context);

    int GetType() const { return DSL_EQUATION; }
    const char* GetTypeName() const { return "EQUATION"; }
	int GetSize() { return DSL_WRONG_NODE_TYPE; }

    int AddParent(int theParent);
    int RemoveParent(int theParent);

    const DSL_generalEquation& GetEquation() const { return equation; }
	DSL_expression* GetSolution() { return solution ? solution : Solve(); }
	const DSL_expression* GetSolution() const { return solution ? solution : Solve(); }

	int Evaluate(const std::map<std::string, double>& values, double& value) const;
	int EvaluateConstant(double& value) const;

    int SetEquation(const std::string &eq, int *errPos = NULL, std::string *errMsg = NULL);
    bool ValidateEquation(const std::string &eq, std::vector<std::string> &vars, std::string &errMsg, int *errPos = NULL, bool *isConst = NULL) const;

    void SetBounds(double low, double high);
    void GetBounds(double &low, double &high) const { low = lowBound; high = highBound; }

	bool HasDiscIntervals() const { return !discIntervals.empty(); }
	void EnsureIntervalsExist();
	const DSL_Dmatrix* GetDiscProbs() const;
	int GetDiscProbInputs(std::vector<DSL_equationDiscretizationInput> &inputs) const;
	void InvalidateDiscProbs() const;
	int SetDiscIntervals(const IntervalVector &intervals) { return SetDiscIntervals(lowBound, highBound, intervals); }
	int SetDiscIntervals(double lo, double hi, const IntervalVector &intervals);
	int ClearDiscIntervals();
	const IntervalVector& GetDiscIntervals() const { return discIntervals; }
	int GetDiscInterval(int intervalIndex, double &lo, double &hi) const;
	int Discretize(double x) const;

    void GetIntervalIds(DSL_idArray &states) const;
	void GetIntervalEdges(std::vector<double> &edges) const;
	void GetIntervalEdges(DSL_doubleArray& edges) const;
	void GetIntervalEdges(double* edges) const;

	int OnParentOutcomeAdd(int parentHandle, int thisPosition) { return OnDiscreteParentChange(); }
	int OnParentOutcomeRemove(int parentHandle, int thisPosition) { return OnDiscreteParentChange(); }
	int OnParentOutcomeReorder(int parentHandle, const DSL_intArray &newOrder) { return OnDiscreteParentChange(); }
	int OnParentReorder(const DSL_intArray &newOrder) { return OnDiscreteParentChange(); }

    int ValidateExpressions(
        const DSL_extFunctionContainer &extFxn,
        int &errFxnIdx,
        std::string &errMsg) const;

    int PatchExpressions(const DSL_extFunctionContainer &extFxn);

    int PrepareForDiscreteChild();

	virtual const DSL_idArray* GetOutcomeIds() const { return NULL; }

	int CalcDiscProbs(bool useCurrentEvidence, std::vector<std::vector<double> > *samples = NULL) const;

private:
	DSL_equation(const DSL_equation& src, DSL_network& targetNetwork);
	~DSL_equation();
	DSL_nodeDef* Clone(DSL_network& targetNetwork) const;
	void InitXform(DSL_nodeXformContext &ctx) const;
	DSL_nodeVal* CreateValue() const;
	DSL_nodeVal* CreateValue(const DSL_valXformContext& ctx) const;
	int GetDiscreteOutcomeCount();

	DSL_expression* Solve() const;

	const DSL_Dmatrix* GetParameters() const { return NULL; }
	DSL_Dmatrix* GetParameters() { return NULL; }

	const char* GetId() const;
	const char* GetPeerId(int peerHandle) const;

	void OnNodeIdChanging(const char* oldId, const char* newId);
	void OnParentIdChanging(int parentHandle, const char *oldId, const char *newId);
    void InvalidateDescendants() const; 
	void InvalidateWithDescendants() const;
	
	void InvalidateChildrenDiscProbs() const;
	
	int OnDiscreteParentChange();

    DSL_generalEquation equation;
    double lowBound;
    double highBound;
	mutable DSL_expression* solution;
	
	IntervalVector discIntervals;
	mutable DSL_Dmatrix *discProbs;
	mutable std::vector<DSL_equationDiscretizationInput> *discInputs;
};

#endif
