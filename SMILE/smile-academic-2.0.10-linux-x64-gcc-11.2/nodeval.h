#ifndef SMILE_NODEVAL_H
#define SMILE_NODEVAL_H

// {{SMILE_PUBLIC_HEADER}}

#include "general.h"
#include "doublearray.h"

#include <vector>

// types of values
#define DSL_BELIEFVECTOR        3
#define DSL_POLICYVALUES        6
#define DSL_EXPECTEDUTILITY     4
#define DSL_MAUEXPECTEDUTILITY  7
#define DSL_EQUATIONEVALUATION  8

// flags 
#define DSL_VALUE_EVIDENCE             1       
#define DSL_VALUE_VALID                2
#define DSL_VALUE_EVIDENCE_PROPAGATED  4 
#define DSL_VALUE_CONTROLLED           8
#define DSL_VALUE_VIRTUAL_EVIDENCE     16


class DSL_network;
class DSL_nodeDef;
class DSL_Dmatrix;
struct DSL_valXformContext;

class DSL_nodeVal
{
public:
    DSL_nodeVal(DSL_network& network, int handle);
    virtual ~DSL_nodeVal() {}
    virtual DSL_nodeVal* Clone(DSL_network& targetNetwork) const = 0;
    virtual int GetType() const = 0;

    template <class T> T* As() { return static_cast<T*>(this); }
    template <class T> const T* As() const { return static_cast<const T*>(this); }

    DSL_network *Network() const { return &network; }
    int Handle() const { return handle; }
  
    const DSL_intArray& GetIndexingParents() const { return indexingParents; }
    virtual int AddIndexingParent(int parentHandle);
    virtual void InitIndexingParents();

    void SetFlag(int thisOne) { flags |= thisOne; }
    void ClearFlag(int thisOne) { flags &= ~thisOne; }
  
    int IsFlagSet(int thisOne) const { return flags & thisOne; } 

    int IsValueValid() const { return IsFlagSet(DSL_VALUE_VALID); }
    void SetValueValid() { SetFlag(DSL_VALUE_VALID); lazyTag = NULL; }
    void SetValueInvalid() { ClearFlag(DSL_VALUE_VALID); lazyTag = NULL; }
  
    void* GetLazyTag() const { return lazyTag;  }
    void SetLazyTag(void *lt) { lazyTag = lt; }

    virtual const DSL_Dmatrix* GetMatrix() const = 0;
    virtual DSL_Dmatrix* GetWriteableMatrix() = 0;

    virtual int GetEvidence() const { return DSL_WRONG_NODE_TYPE; } 
    virtual int GetEvidence(double &evidence) const { return DSL_WRONG_NODE_TYPE; }
    virtual const char* GetEvidenceId() const { return NULL; }
    virtual int SetEvidence(double evidence) { return DSL_WRONG_NODE_TYPE; } 
    virtual int SetEvidence(int evidence) { return DSL_WRONG_NODE_TYPE; } 
    virtual int SetEvidence(const char* outcomeId) { return DSL_WRONG_NODE_TYPE; }
    virtual int ClearEvidence() { return DSL_WRONG_NODE_TYPE; } 
    virtual int ClearPropagatedEvidence() { return DSL_WRONG_NODE_TYPE; }
    virtual int SetPropagatedEvidence(int evidence) { return DSL_WRONG_NODE_TYPE; } 
    virtual int SetPropagatedEvidence(double evidence) { return DSL_WRONG_NODE_TYPE; } 
    virtual int GetVirtualEvidence(std::vector<double> &evidence) const { return DSL_WRONG_NODE_TYPE; }
    virtual int GetVirtualEvidence(DSL_doubleArray &evidence) const { return DSL_WRONG_NODE_TYPE; }
    virtual int SetVirtualEvidence(const std::vector<double>& evidence) { return DSL_WRONG_NODE_TYPE; }
    virtual int SetVirtualEvidence(const DSL_doubleArray &evidence) { return DSL_WRONG_NODE_TYPE; }
#ifdef DSL_INITLIST_SUPPORTED
    int SetVirtualEvidence(std::initializer_list<double> evidence) { return SetVirtualEvidence(DSL_doubleArray(evidence)); }
#endif
    virtual int ClearVirtualEvidence() { return DSL_WRONG_NODE_TYPE; }
    
    virtual int GetMean(double& mean) const { return DSL_WRONG_NODE_TYPE; }
    virtual int GetStdDev(double& stddev) const { return DSL_WRONG_NODE_TYPE; }
    virtual int GetTemporalMeanStdDev(DSL_doubleArray& temporalMean, DSL_doubleArray& temporalStdDev) const { return DSL_WRONG_NODE_TYPE; }
    virtual int GetTemporalMeanStdDev(std::vector<double>& temporalMean, std::vector<double>& temporalStdDev) const { return DSL_WRONG_NODE_TYPE; }

    int IsEvidence() const { return 0 != IsFlagSet(DSL_VALUE_EVIDENCE | DSL_VALUE_EVIDENCE_PROPAGATED); }
    int IsRealEvidence() const { return IsFlagSet(DSL_VALUE_EVIDENCE); }
    int IsPropagatedEvidence() const { return IsFlagSet(DSL_VALUE_EVIDENCE_PROPAGATED); }
    int IsVirtualEvidence() const { return IsFlagSet(DSL_VALUE_VIRTUAL_EVIDENCE); }

    bool HasTemporalEvidence() const;
    bool IsTemporalEvidence(int slice) const;
    bool IsTemporalVirtualEvidence(int slice) const;
    int GetTemporalEvidence(int slice) const;
    int GetTemporalEvidence(int slice, DSL_doubleArray& evidence) const;
    int GetTemporalEvidence(int slice, std::vector<double> &evidence) const;
    int SetTemporalEvidence(int slice, int evidence);
    int SetTemporalEvidence(int slice, const DSL_doubleArray &evidence);
    int SetTemporalEvidence(int slice, const std::vector<double> &evidence);
#ifdef DSL_INITLIST_SUPPORTED
    int SetTemporalEvidence(int slice, std::initializer_list<double> evidence)
    {
        return SetTemporalEvidence(slice, DSL_doubleArray(evidence));
    }
#endif
    int ClearTemporalEvidence(int slice);
    int GetTemporalMean(int slice, double& mean) const;
    int GetTemporalStdDev(int slice, double& stddev) const;

    // BACKWARD COMPATIBILITY ONLY
    int GetValue(const DSL_Dmatrix** m) const
    {
        *m = GetMatrix();
        return (NULL == *m) ? DSL_WRONG_NODE_TYPE : DSL_OKAY;
    }

    virtual void InitXform(DSL_valXformContext& ctx) const = 0;

protected:
    DSL_nodeVal(const DSL_nodeVal& src, DSL_network& targetNetwork);
    DSL_nodeVal(const DSL_valXformContext& ctx);

    DSL_nodeDef* Def() const;
    DSL_nodeDef* GetPeerDef(int peerHandle) const;
    DSL_nodeVal* GetPeerVal(int peerHandle) const;

    void InitMatrix(DSL_Dmatrix& m);

    DSL_network &network;
    int const handle;
    int flags;
    DSL_intArray indexingParents;  // handles of the parents indexing the values (influence diagrams only)
    void *lazyTag; // initialized to NULL and set back to NULL whenever valid flag changes
};

struct DSL_valXformContext
{
    DSL_valXformContext(DSL_network& n, int h);
    DSL_network& network;
    int const handle;
    enum { evNone, evDiscrete, evCont, evVirtual } evidenceType;
    int discreteEvidence;
    double contEvidence;
    std::vector<double> virtualEvidence;
};


#endif
