#ifndef SMILE_NODEDEF_H
#define SMILE_NODEDEF_H

// {{SMILE_PUBLIC_HEADER}}

#include "general.h"
#include "doublearray.h"
#include <string>
#include "platform.h"
#ifdef DSL_INITLIST_SUPPORTED
#include <initializer_list>
#endif

// these flags represent the four basic roles of nodes
#define DSL_DECISION           1    // set if decision node
#define DSL_CHANCE             2    // set if chance node
#define DSL_DETERMINISTIC      4    // set if deterministic node
#define DSL_UTILITY            8    // set if utility node

// these flags represent some properties of the nodes
#define DSL_DISCRETE          16    // set if discrete, cleared if continous
#define DSL_CASTLOGIC         32    // set if node is CAST logic 
#define DSL_DEMORGANLOGIC     64    // set if node is DeMorgan logic
#define DSL_NOISYMAXLOGIC    128    // set if Noisy MAX representation
#define DSL_NOISYADDERLOGIC  256    // set if NoisyAdder representation
#define DSL_PARENTSCONTIN    512    // set if any parent is continuous


// types of definitions
//                          node type           features
#define DSL_TRUTHTABLE     (DSL_DETERMINISTIC | DSL_DISCRETE)
#define DSL_CPT            (DSL_CHANCE        | DSL_DISCRETE)
#define DSL_NOISY_MAX      (DSL_CHANCE        | DSL_DISCRETE |  DSL_NOISYMAXLOGIC)
#define DSL_NOISY_ADDER    (DSL_CHANCE        | DSL_DISCRETE |  DSL_NOISYADDERLOGIC)
#define DSL_CAST           (DSL_CHANCE        | DSL_DISCRETE |  DSL_CASTLOGIC)
#define DSL_DEMORGAN       (DSL_CHANCE        | DSL_DISCRETE |  DSL_DEMORGANLOGIC)
#define DSL_LIST           (DSL_DECISION      | DSL_DISCRETE)
#define DSL_TABLE          (DSL_UTILITY)
#define DSL_MAU            (DSL_UTILITY       | DSL_PARENTSCONTIN)
#define DSL_EQUATION       (DSL_DETERMINISTIC)


class DSL_network;
class DSL_idArray;
class DSL_Dmatrix;
class DSL_extFunctionContainer;
class DSL_randGen;
class DSL_nodeVal;
struct DSL_outcomeExt;
struct DSL_nodeXformContext;
struct DSL_valXformContext;

class DSL_nodeDef
{
public:
    static DSL_nodeDef* Create(int defType, DSL_network& network, int handle, const char *id);
    static bool IsValidType(int defType);
    static bool IsArcAllowedForTypes(int defTypeParent, int defTypeChild);

    template <class T> T* As() { return static_cast<T*>(this); }
    template <class T> const T* As() const { return static_cast<const T*>(this); }

    virtual ~DSL_nodeDef() {}

    DSL_nodeDef* Xform(int newDefType) const;
    virtual DSL_nodeDef* Clone(DSL_network& targetNetwork) const = 0;
    virtual DSL_nodeVal* CreateValue() const = 0;
    virtual DSL_nodeVal* CreateValue(const DSL_valXformContext &ctx) const = 0;
    void CopyParameters(DSL_nodeDef& target) const;

    int Handle() const { return handle; }
    DSL_network* Network() { return &network; }
    const DSL_network* Network() const { return &network; }
    
    const DSL_intArray& GetParents() const;
    const DSL_intArray& GetChildren() const;

    virtual int GetType() const = 0;
    virtual const char* GetTypeName() const = 0;
    bool TypeIsOneOf(const int* types, int count) const;
    bool TypeHasFlag(int flags) const { return 0 != (flags & GetType()); }

    // methods to deal with changes in my relationships
    virtual int AddParent(int parentHandle) { return DSL_WRONG_NODE_TYPE; }
    virtual int RemoveParent(int parentHandle) { return DSL_WRONG_NODE_TYPE; }
    
    virtual void OnNodeIdChanging(const char* oldId, const char* newId) {}

    // methods to deal with changes in the definition of parents
    virtual int OnParentOutcomeAdd(int parentHandle, int thisPosition) { return DSL_WRONG_NODE_TYPE; }
    virtual int OnParentOutcomeRemove(int parentHandle, int thisPosition) { return DSL_WRONG_NODE_TYPE; }
    virtual int OnParentOutcomeReorder(int parentHandle, const DSL_intArray& newOrder) { return DSL_WRONG_NODE_TYPE; }
    virtual int OnParentReorder(const DSL_intArray& newOrder) { return DSL_WRONG_NODE_TYPE; }
    virtual void OnParentIdChanging(int parentHandle, const char* oldId, const char* newId) {}

    virtual int IsParentNecessary(int parentIndex, double epsilon, bool& necessary) const;

    virtual int ValidateExpressions(const DSL_extFunctionContainer& extFxn, int& errFxnIdx, std::string& errMsg) const { return DSL_OKAY; }
    virtual int PatchExpressions(const DSL_extFunctionContainer& extFxn) { return DSL_OKAY; }

    virtual int AddOutcome(const char* outcomeId) { return DSL_WRONG_NODE_TYPE; }
    virtual int InsertOutcome(int outcomeIndex, const char* outcomeId) { return DSL_WRONG_NODE_TYPE; }
    virtual int RemoveOutcome(int outcomeIndex) { return DSL_WRONG_NODE_TYPE; }
    virtual int GetNumberOfOutcomes() const { return DSL_WRONG_NODE_TYPE; }
    virtual int RenameOutcome(int outcomeIndex, const char* newOutcomeId) { return DSL_WRONG_NODE_TYPE; }
    virtual int RenameOutcomes(const DSL_idArray& newOutcomeIds) { return DSL_WRONG_NODE_TYPE; }
    virtual int RenameOutcomes(const std::vector<std::string>& newOutcomeIds) { return DSL_WRONG_NODE_TYPE; }
    virtual const DSL_idArray* GetOutcomeIds() const = 0;
    virtual int SetNumberOfOutcomes(int numOutcomes) { return DSL_WRONG_NODE_TYPE; }
    virtual int SetNumberOfOutcomes(const DSL_idArray& outcomeIds) { return DSL_WRONG_NODE_TYPE; }
    virtual int ChangeOrderOfOutcomes(const DSL_intArray& newOrder) { return DSL_WRONG_NODE_TYPE; }
    virtual int ChangeOrderOfOutcomesWithAddAndRemove(const DSL_idArray& ids, const DSL_intArray& newOrder) { return DSL_WRONG_NODE_TYPE; }

    virtual int MakeUniform() { return DSL_WRONG_NODE_TYPE; }
    virtual int MakeRandom(DSL_randGen& randGen) { return DSL_WRONG_NODE_TYPE; }

    virtual int RawDefinition(DSL_Dmatrix** parameters);
    int GetDefinition(const DSL_Dmatrix** parameters) const;
    int SetDefinition(const DSL_Dmatrix& parameters);
    int SetDefinition(const DSL_doubleArray& parameters);
    int SetDefinition(const std::vector<double>& parameters);

#ifdef DSL_INITLIST_SUPPORTED 
    int SetDefinition(std::initializer_list<double> parameters) { return SetDefinition(DSL_doubleArray(parameters)); }
    int SetTemporalDefinition(int order, std::initializer_list<double> temporal) { return SetTemporalDefinition(order, DSL_doubleArray(temporal)); }
#endif   
    
    // parameter matrix as seen by the inference algorithm 
    // (lazily calculated for CI and qual nodes, but usually specified by the user through SetDefinition)
    virtual const DSL_Dmatrix* GetMatrix() const { return GetParameters(); }

    const DSL_Dmatrix* GetTemporalDefinition(int order) const;
    int SetTemporalDefinition(int order, const DSL_Dmatrix& temporal);
    int SetTemporalDefinition(int order, const std::vector<double>& temporal);
    int SetTemporalDefinition(int order, const DSL_doubleArray& temporal);

    // internal use only
    virtual int GetDiscreteOutcomeCount() { return DSL_WRONG_NODE_TYPE; }

#ifndef SMILE_NO_V1_COMPATIBILITY
    // BACKWARD COMPATIBILITY ONLY
    const DSL_idArray* GetOutcomesNames() const { return GetOutcomeIds(); }
    int NodeTypeIs(int flags) const { return flags & GetType(); }
#endif

protected:
    DSL_nodeDef(DSL_network& network, int handle);
    DSL_nodeDef(const DSL_nodeXformContext& xfCtx);
    DSL_nodeDef(const DSL_nodeDef& src, DSL_network& targetNetwork);
    
    virtual void InitXform(DSL_nodeXformContext& xfCtx) const = 0;
    
    virtual void DoCopyParameters(DSL_nodeDef& target) const;

    DSL_nodeDef* GetPeerDef(int peerHandle) const;
    const char* GetPeerId(int peerHandle) const;

    virtual const DSL_Dmatrix* GetParameters() const = 0;
    virtual DSL_Dmatrix* GetParameters() = 0;

    int FindParentPosition(int parentHandle) const;
    const DSL_nodeDef* GetPlateDef(int order) const;
    DSL_nodeDef* GetPlateDef(int order);
    int GetPlateDef(int order, DSL_nodeDef* &p);

    DSL_network& network;
    int const handle;
};

struct DSL_nodeXformContext
{
    DSL_nodeXformContext(DSL_network& n, int h);
    ~DSL_nodeXformContext();
    DSL_network& network;
    int const handle;
    const DSL_idArray* outcomeIds;
    bool deleteOutcomeIds;
    const DSL_doubleArray* outcomeLevels;
    bool deleteOutcomeLevels;
    const DSL_outcomeExt* outcomeExt;
    const DSL_Dmatrix* cpt;
    bool deleteCpt;
};

#endif
