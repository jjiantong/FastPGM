#ifndef SMILE_DEFLIST_H
#define SMILE_DEFLIST_H

// {{SMILE_PUBLIC_HEADER}}

#include "discdef.h"

class DSL_decision : public DSL_discDef
{
public:
    DSL_decision(DSL_network& network, int handle);
    DSL_decision(const DSL_nodeXformContext& xfp);

    int GetType() const { return DSL_LIST; }
    const char* GetTypeName() const { return "LIST"; }

    const char* GetOutcomePrefix() const;

private:
    DSL_decision(const DSL_decision& src, DSL_network& targetNetwork);
    DSL_nodeDef* Clone(DSL_network& targetNetwork) const;
    DSL_nodeVal* CreateValue() const;
    DSL_nodeVal* CreateValue(const DSL_valXformContext& ctx) const;

    void InitXform(DSL_nodeXformContext& ctx) const;

    int AddParent(int theParent) { return DSL_OKAY; }
    int RemoveParent(int theParent) { return DSL_OKAY; }
    int OnParentOutcomeAdd(int parentHandle, int thisPosition) { return DSL_OKAY; }
    int OnParentOutcomeRemove(int parentHandle, int thisPosition) { return DSL_OKAY; }
    int OnParentOutcomeReorder(int parentHandle, const DSL_intArray& newOrder) { return DSL_OKAY; }

    int AfterOutcomeCountChanged() { return DSL_OKAY; }

    const DSL_Dmatrix* GetParameters() const { return NULL; }
    DSL_Dmatrix* GetParameters() { return NULL; }
};

#endif
