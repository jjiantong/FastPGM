#ifndef SMILE_DEFNOISYADDER_H
#define SMILE_DEFNOISYADDER_H

// {{SMILE_PUBLIC_HEADER}}

#include "cidef.h"

class DSL_noisyAdder : public DSL_ciDef  
{
public:
    enum Function { fun_average, fun_single_fault };

    DSL_noisyAdder(DSL_network& network, int handle);
    DSL_noisyAdder(const DSL_nodeXformContext& context);
  
    int GetType() const { return DSL_NOISY_ADDER; } 
    const char* GetTypeName() const { return "NOISY_ADDER"; }

    void GetUnconstrainedColumns(std::vector<int> &columns) const;
    
    int CheckCiWeightsConsistency(const DSL_Dmatrix &ciWeights, char * errorMsg, int errorMsgBufSize);

    void CalculateCpt() const;
    void CiToCpt(const DSL_Dmatrix& ci, DSL_Dmatrix& cpt) const;

    int GetDistinguishedState() const { return dState; }
    int GetParentDistinguishedState(int parentPos) const { return dParentStates[parentPos]; }
    double GetParentWeight(int parentPos) const { return parentWeights[parentPos]; }

    int SetDistinguishedState(int newDistState);
    int SetParentDistinguishedState(int parentPos, int newDistState);
    int SetParentWeight(int parentPos, double value);

    int SetFunction(Function val);
    Function GetFunction() const { return function; }

    int GetTemporalFunction(int order, Function &val) const;
    int SetTemporalFunction(int order, Function val);

    int GetTemporalDistinguishedState(int order) const;
    int SetTemporalDistinguishedState(int order, int state);

    int GetTemporalParentInfo(int order, DSL_doubleArray &weights, DSL_intArray &distStates) const;
    int SetTemporalParentInfo(int order, const DSL_doubleArray &weights, const DSL_intArray &distStates);

private:
    DSL_noisyAdder(const DSL_noisyAdder& src, DSL_network& targetNetwork);
    DSL_nodeDef* Clone(DSL_network& targetNetwork) const;
    void DoCopyParameters(DSL_nodeDef& target) const;

    int AddParent(int theParent);
    int RemoveParent(int theParent);
    int OnParentOutcomeAdd(int parentHandle, int thisPosition);
    int OnParentOutcomeRemove(int parentHandle, int thisPosition);
    int OnParentOutcomeReorder(int parentHandle, const DSL_intArray& newOrder);
    int OnParentReorder(const DSL_intArray& newOrder);
    int AfterInsertOutcome(int outcomeIndex);
    int AfterRemoveOutcome(int outcomeIndex);
    int AfterOutcomeCountChanged();
    int AfterReorderOutcomes(const DSL_intArray& newOrder);

    void FixConstrainedColumns();
    int CiIndexConstrained(int index) const;

    int CiToCptAverage(const DSL_Dmatrix& ci,DSL_Dmatrix& cpt) const;
    int CiToCptSingleFault(const DSL_Dmatrix& ci,DSL_Dmatrix& cpt) const;

    int dState;
    DSL_intArray dParentStates;
    DSL_doubleArray parentWeights;
    Function function;
};

#endif
