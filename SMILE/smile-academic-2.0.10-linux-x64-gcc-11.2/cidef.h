#ifndef SMILE_CIDEF_H
#define SMILE_CIDEF_H

// {{SMILE_PUBLIC_HEADER}}

#include "lazydef.h"
#include "dmatrix.h"
#include <vector>


class DSL_ciDef : public DSL_lazyDef
{
public:
    const DSL_Dmatrix& GetCiWeights() const { return ciWeights; }
    int SetCiWeights(const DSL_Dmatrix& weights);
    int SetCiWeights(const DSL_doubleArray& weights);

    int GetNumberOfParentOutcomes(int parentIndex) const;
    int GetParentColumnBase(int parentIndex) const;

    virtual void GetUnconstrainedColumns(std::vector<int>& columns) const = 0;

    int GetDefinition(const DSL_Dmatrix** probs) const { *probs = &ciWeights; return DSL_OKAY; }
    int SetDefinition(const DSL_Dmatrix& probs) { return SetCiWeights(probs); }
    int SetDefinition(const DSL_doubleArray& probs) { return SetCiWeights(probs); }

    const DSL_Dmatrix* GetTemporalCiWeights(int order) const;
    int SetTemporalCiWeights(int order, const DSL_Dmatrix& weights);

    int MakeUniform();
    int MakeRandom(DSL_randGen &randGen);

protected:
    DSL_ciDef(DSL_network& network, int handle);
    DSL_ciDef(const DSL_nodeXformContext& context);
    DSL_ciDef(const DSL_ciDef& src, DSL_network& targetNetwork);

    void DoCopyParameters(DSL_nodeDef& target) const;

    virtual void FixConstrainedColumns() = 0;

    int AfterInsertOutcome(int outcomeIndex);
    int AfterRemoveOutcome(int outcomeIndex);
    int AfterOutcomeCountChanged();
    int AfterReorderOutcomes(const DSL_intArray& newOrder);

    void SetConstrainedColumn(int colIdx, int onePos = -1);

    int AddParent(int parentHandle);
    int RemoveParent(int parentHandle);
    void DoAddParentOutcome(int parentIndex, int outcomeIndex, bool uniformize);
    void DoRemoveParentOutcome(int parentIndex, int outcomeIndex);
    void DoParentReorder(const DSL_intArray& newOrder);

    bool CheckWeightsDimensions(int changingParentIndex) const;

    const DSL_Dmatrix* GetParameters() const { return &ciWeights; }
    DSL_Dmatrix* GetParameters() { return &ciWeights; }

    DSL_Dmatrix ciWeights; // causal independent parameters 

private:
    void InitWeights(int colCount);
};

#endif 
