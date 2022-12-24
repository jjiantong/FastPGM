#ifndef SMILE_DEFNOISYMAX_H
#define SMILE_DEFNOISYMAX_H

// {{SMILE_PUBLIC_HEADER}}

#include "cidef.h"
#include <vector>

// This is an implementation of noisy-MAX.
// As native parametrization we use weight[i] = P(Y|Xi), 
// called 'Diez' but this convention is not strict. 
// We DON'T use P(Y|~X1,...,Xi,...,Xn)

class DSL_noisyMAX : public DSL_ciDef  
{
public:
    DSL_noisyMAX(DSL_network& network, int handle);
    DSL_noisyMAX(const DSL_nodeXformContext& context);

    int GetType() const { return DSL_NOISY_MAX; }
    const char* GetTypeName() const { return "NOISY_MAX"; }

    void GetUnconstrainedColumns(std::vector<int> &columns) const;

    int GetStrengthOfOutcome(int parentPos, int parentPosOutcome) const { return parentOutcomeStrengths[parentPos].FindPosition(parentPosOutcome); }
    int GetOutcomeOfStrength(int parentPos, int outcomeStrength) const { return parentOutcomeStrengths[parentPos][outcomeStrength]; }

    const DSL_intArray& GetParentOutcomeStrengths(int parentPos) const { return parentOutcomeStrengths[parentPos]; }
    int SetParentOutcomeStrengths(int parentPos, const DSL_intArray &newStrengths); 

    int GetTemporalParentOutcomeStrengths(int order, std::vector<DSL_intArray>& strengths) const;
    int SetTemporalParentOutcomeStrengths(int order, const std::vector<DSL_intArray>& strengths);

    int ChangeOrderOfStrengths(int parentPos, const DSL_intArray &newOrder);

    int IsParentNecessary(int parentIndex, double epsilon, bool &necessary) const;

    int CheckCiWeightsConsistency(const DSL_Dmatrix &weightsToTest, std::string &errMsg, int& errRow, int& errCol) const;

    //  ==== Converting noisy-MAX into Henrion parametrization ====
    int GetHenrionProbabilities(DSL_Dmatrix &henrion);
    int SetHenrionProbabilities(const DSL_Dmatrix &henrion);
    int CheckHenrionConsistency(DSL_Dmatrix &henrion, std::string &errMsg, int &errRow, int &errCol, bool &leakInconsistent) const;

    //  ==== Converting noisy-MAX into CPT distribution ====
    void CalculateCpt() const;
    void CiToCpt(DSL_Dmatrix& ci, DSL_Dmatrix& cpt) const;

    static void CiToCumulativeCi(DSL_Dmatrix& ci);
    static void CumulativeCiToCpt(const DSL_Dmatrix& ci, const std::vector<DSL_intArray> &parentOutcomeStrengths, const DSL_intArray &parentStartingPos, DSL_Dmatrix& cpt);

    int CptToCi();
  
    // Part Added during transition noisyOR/AND -> noisyMAX
    int SetLegacyNoisyOrProbabilities(DSL_doubleArray &legacyWeights);
    int GetLegacyNoisyOrProbabilities(DSL_doubleArray &legacyWeights);

    void SetParentOutcomeStrengthsUnchecked(int parentPos, const DSL_intArray &newStrengths);

    void CalcParentColumnBases(DSL_intArray& colBases) const;

private:
    DSL_noisyMAX(const DSL_noisyMAX& src, DSL_network& targetNetwork);
    DSL_nodeDef* Clone(DSL_network& targetNetwork) const;
    void DoCopyParameters(DSL_nodeDef& target) const;

    int AddParent(int parentHandle);
    int RemoveParent(int parentHandle);
    int OnParentOutcomeAdd(int parentHandle, int thisPosition);
    int OnParentOutcomeRemove(int parentHandle, int thisPosition);
    int OnParentOutcomeReorder(int parentHandle, const DSL_intArray& newOrder);
    int OnParentReorder(const DSL_intArray& newOrder);

    void FixConstrainedColumns();

    int CiIndexConstrained(const DSL_Dmatrix& ci, int index) const;
    
    int SquareCptToCi(DSL_Dmatrix& ci, double step, double minStep) const;
    double SquareCiToCptSingleStep(DSL_Dmatrix& ci, int index, double step) const;

    std::vector<DSL_intArray> parentOutcomeStrengths;
};

#endif 
