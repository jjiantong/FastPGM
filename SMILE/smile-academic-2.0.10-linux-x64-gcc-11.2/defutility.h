#ifndef SMILE_DEFTABLE_H
#define SMILE_DEFTABLE_H

// {{SMILE_PUBLIC_HEADER}}

#include "nodedef.h"
#include "dmatrix.h"

class DSL_randGen;


class DSL_utility : public DSL_nodeDef
{
public:
    DSL_utility(DSL_network& network, int handle);
    DSL_utility(const DSL_nodeXformContext& xfp);

    int GetType() const { return DSL_TABLE; }
    const char* GetTypeName() const { return "TABLE"; }

    const DSL_Dmatrix& GetUtilities() { return table; }

    int Normalize();

    double GetMinimumUtility() const;
    double GetMaximumUtility() const;

    int IsParentNecessary(int parentIndex, double epsilon, bool &necessary) const;

    int MakeUniform();
    int MakeRandom(DSL_randGen &randGen);

private:
    DSL_utility(const DSL_utility& src, DSL_network& targetNetwork);
    DSL_nodeDef* Clone(DSL_network& targetNetwork) const;
    void InitXform(DSL_nodeXformContext &xfp) const;
    DSL_nodeVal* CreateValue() const;
    DSL_nodeVal* CreateValue(const DSL_valXformContext& ctx) const;

    const DSL_idArray* GetOutcomeIds() const { return NULL; }

    int AddParent(int parentHandle);
    int RemoveParent(int parentHandle);
    int OnParentOutcomeAdd(int parentHandle, int thisPosition);
    int OnParentOutcomeRemove(int parentHandle, int thisPosition);
    int OnParentOutcomeReorder(int parentHandle, const DSL_intArray& newOrder);
    int OnParentReorder(const DSL_intArray& newOrder);

    const DSL_Dmatrix* GetParameters() const { return &table; }
    DSL_Dmatrix* GetParameters() { return &table; }

    DSL_Dmatrix table;      // the table with the utilities
};

#endif
