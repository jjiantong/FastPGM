#ifndef SMILE_DISCDEF_H
#define SMILE_DISCDEF_H

// {{SMILE_PUBLIC_HEADER}}

#include "nodedef.h"
#include "idarray.h"

struct DSL_outcomeInfo
{
    DSL_outcomeInfo() : fault(false) {}
    bool fault;
    std::string comment;
    std::string label;
    std::string fix;
    DSL_documentation documentation;
};

struct DSL_outcomeExt
{
    DSL_outcomeExt() : defaultOutcome(-1) {}
    int defaultOutcome; 
    std::vector<DSL_outcomeInfo> outcomeInfo;
};


class DSL_discDef : public DSL_nodeDef
{
public:
    virtual bool HasFixedOutcomes() const { return false; }

    virtual int GetNumberOfOutcomes() const { return outcomeIds.GetSize(); }
    virtual const DSL_idArray* GetOutcomeIds() const { return &outcomeIds; }

    virtual int AddOutcome(const char* outcomeId) { return InsertOutcome(GetNumberOfOutcomes(), outcomeId); }
    virtual int InsertOutcome(int outcomeIndex, const char* outcomeId);
    virtual int RemoveOutcome(int outcomeIndex);
    virtual int ChangeOrderOfOutcomes(const DSL_intArray& newOrder);

    virtual int RenameOutcome(int outcomeIndex, const char* newOutcomeId);
    virtual int RenameOutcomes(const DSL_idArray& newOutcomeIds);
    virtual int RenameOutcomes(const std::vector<std::string>& newOutcomeIds);

    virtual int SetNumberOfOutcomes(int numOutcomes);
    virtual int SetNumberOfOutcomes(const DSL_idArray& outcomeIds);

    bool HasIdentifiers() const;

    bool HasIntervals() const;
    int RemoveIntervals();
    int GetInterval(int outcomeIndex, double& lo, double& hi) const;
    int GetIntervals(DSL_doubleArray& intervals) const;
    int GetIntervals(std::vector<double>& intervals) const;
    int SetIntervals(const DSL_doubleArray& intervals, bool removeIds = false);
    int SetIntervals(const std::vector<double>& intervals, bool removeIds = false);
#ifdef DSL_INITLIST_SUPPORTED 
    int SetIntervals(std::initializer_list<double> intervals, bool removeIds = false) { return SetIntervals(DSL_doubleArray(intervals), removeIds); }
#endif

    bool HasPointValues() const;
    int RemovePointValues();
    int PointValueToOutcomeIndex(double pv) const;
    int GetPointValue(int outcomeIndex, double &pointValue) const;
    int GetPointValues(DSL_doubleArray& pointValues) const;
    int GetPointValues(std::vector<double>& pointValues) const;
    int SetPointValues(const DSL_doubleArray& pointValues);
    int SetPointValues(const std::vector<double>& pointValues);
#ifdef DSL_INITLIST_SUPPORTED 
    int SetPointValues(std::initializer_list<double> pointValues) { return SetIntervals(DSL_doubleArray(pointValues)); }
#endif

    int Discretize(double value) const;
    double SamplePoint(int outcomeIndex) const;

    int GetMean(const DSL_Dmatrix& posteriors, double& mean) const;
    int GetStdDev(const DSL_Dmatrix& posteriors, double& stdDev) const;

    // diagnostic attributes
    int GetDefaultOutcome() const;
    int SetDefaultOutcome(int newDefOutcome);

    bool IsFaultOutcome(int outcomeIndex) const;
    int SetFaultOutcome(int outcomeIndex, bool fault);

    const DSL_documentation* GetOutcomeDocumentation(int outcomeIndex) const;
    int SetOutcomeDocumentation(int outcomeIndex, const DSL_documentation* doc);

    const char* GetOutcomeLabel(int outcomeIndex) const;
    int SetOutcomeLabel(int outcomeIndex, const char* label);

    const char* GetOutcomeComment(int outcomeIndex) const;
    int SetOutcomeComment(int outcomeIndex, const char* comment);

    const char* GetOutcomeFix(int outcomeIndex) const;
    int SetOutcomeFix(int outcomeIndex, const char* fix);

protected:
    DSL_discDef(DSL_network& network, int handle, const DSL_idArray& outcomes);
    DSL_discDef(const DSL_nodeXformContext& xfCtx);
    DSL_discDef(const DSL_discDef& src, DSL_network& targetNetwork);
    ~DSL_discDef();

    void InitXform(DSL_nodeXformContext& xfCtx) const;

    void SetMatrixDimensions(DSL_Dmatrix& m) const;

    DSL_nodeVal* CreateValue() const;
    DSL_nodeVal* CreateValue(const DSL_valXformContext& ctx) const;

    virtual const char* GetOutcomePrefix() const;

    virtual int AfterInsertOutcome(int outcomeIndex) { return DSL_OKAY; }
    virtual int AfterRemoveOutcome(int outcomeIndex) { return DSL_OKAY; }
    virtual int AfterReorderOutcomes(const DSL_intArray& newOrder) { return DSL_OKAY; }
    virtual int AfterOutcomeCountChanged() = 0;

    virtual int GetDiscreteOutcomeCount() { return outcomeIds.GetSize(); }

    virtual int ChangeOrderOfOutcomesWithAddAndRemove(const DSL_idArray& ids, const DSL_intArray& newOrder);

    bool CheckMatrixDimensions(const DSL_Dmatrix& mtx) const;

    const DSL_outcomeExt* GetExt() const { return ext; }

private:
    void EnsureExt();
    const char* GetExtString(int outcomeIndex, const std::string DSL_outcomeInfo::* memptr) const;
    int SetExtString(int outcomeIndex, std::string DSL_outcomeInfo::* memptr, const char* val);

    int SetNumberOfOutcomesWithAddAndRemove(int numOutcomes);
    int CompleteSetNumberOfOutcomes(const DSL_idArray& ids);

    int DoSetIntervals(const double* src, int count, bool removeIds);
    int DoSetPointValues(const double* src, int count);

    DSL_idArray outcomeIds;
    DSL_doubleArray* levels;
    DSL_outcomeExt* ext;
};

#endif
