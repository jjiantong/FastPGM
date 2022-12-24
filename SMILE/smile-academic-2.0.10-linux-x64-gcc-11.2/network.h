#ifndef SMILE_NETWORK_H
#define SMILE_NETWORK_H

// {{SMILE_PUBLIC_HEADER}}

#include "general.h"
#include "node.h"
#include <vector>

class DSL_submodelHandler;
class DSL_errorStringHandler;
class DSL_valueOfInformation;
class DSL_header;
class DSL_idArray;
class DSL_caseManager;
class DSL_case;
class DSL_temporalArcs;
class DSL_dbnImpl;
class DSL_Dmatrix;
class DSL_progress;
class DSL_jptContainer;
class DSL_extFunctionContainer;
class DSL_extFunctionFactory;
class DSL_randGen;
class DSL_fileFormat;
class DSL_instanceCounts;

enum dsl_temporalType { dsl_normalNode, dsl_anchorNode, dsl_terminalNode, dsl_plateNode };
enum dsl_arcType { dsl_normalArc, dsl_costObserve };

// BN algorithms
#define DSL_ALG_BN_LAURITZEN              0
#define DSL_ALG_BN_HENRION                1
#define DSL_ALG_BN_PEARL                  2
#define DSL_ALG_BN_LSAMPLING              3
#define DSL_ALG_BN_SELFIMPORTANCE         4
#define DSL_ALG_BN_HEURISTICIMPORTANCE    5
#define DSL_ALG_BN_BACKSAMPLING           6
#define DSL_ALG_BN_AISSAMPLING            7
#define DSL_ALG_BN_EPISSAMPLING           8
#define DSL_ALG_BN_LBP					  9
#define DSL_ALG_BN_LAURITZEN_OLD          10
#define DSL_ALG_BN_RELEVANCEDECOMP        11
#define DSL_ALG_BN_RELEVANCEDECOMP2       12

// ID algorithms
#define DSL_ALG_ID_COOPERSOLVING          0
#define DSL_ALG_ID_SHACHTER               1

// file formats
#define DSL_ERGO_FORMAT         1
#define DSL_NETICA_FORMAT       2
#define DSL_INTERCHANGE_FORMAT  3 
#define DSL_DSL_FORMAT          4
#define DSL_HUGIN_FORMAT        5
#define DSL_XMLBIF_FORMAT       6
#define DSL_KI_FORMAT           7
#define DSL_XDSL_FORMAT         8


// helper structures

struct DSL_EPISParams
{
	DSL_EPISParams();
	bool IsValid() const;
	bool Equals(const DSL_EPISParams &rhs) const;

	int iPropagationLength;
	int iNumStateSmall;
	int iNumStateMedium;
	int iNumStateBig;
	double dStateSmallEpsil;
	double dStateMediumEpsil;
	double dStateBigEpsil;
	double dStateOtherwise;
};

struct DSL_LBPParams
{
	DSL_LBPParams();
	bool IsValid() const;
	int iPropagationLength;
};

struct DSL_AnnealedMAPParams
{
	DSL_AnnealedMAPParams();
	bool IsValid() const;
	double speed;		// Annealing speed
	double Tmin;		// Mininum temperature
	double Tinit;		// Initial temperature
	double kReheat;	// RFC coefficient
	int kMAP;			// Number of best solutions we want
	double kRFC;		// coefficient for RFC
	int numCycle;		// Number of iterations per cycle;
	int iReheatSteps;	// Number of no-improvement iterations before reheating
	int iStopSteps;	// Number of no-improvement iterations before stopping
};

struct DSL_SimpleGraphLayoutParams
{
	DSL_SimpleGraphLayoutParams();
	bool flowTopDown; // if false, flow goes from left to right
	int percSpacing;  // spacing as percentage of max node width/height
	bool setNodeSizes;
	int nodeWidth;
	int nodeHeight;
};

class DSL_network;
struct DSL_updateBeliefsListener
{
	virtual void AfterUpdateBeliefs(DSL_network *net, int result) = 0;
};


//////////////////////////////////////////////////////////////////////////////
// class DSL_network
//////////////////////////////////////////////////////////////////////////////

// some flags
#define DSL_USE_RELEVANCE          1
#define DSL_PROPAGATE_EVIDENCE     2
#define DSL_UPDATE_IMMEDIATELY     8

// some flags for relevance
#define DSL_RELEVANCE_ARC_ADDED                      1
#define DSL_RELEVANCE_ARC_REMOVED                    2
#define DSL_RELEVANCE_EVIDENCE_SET                   4
#define DSL_RELEVANCE_EVIDENCE_CLEARED               8
#define DSL_RELEVANCE_PROPAGATED_EVIDENCE_SET       16
#define DSL_RELEVANCE_PROPAGATED_EVIDENCE_CLEARED   32 
#define DSL_RELEVANCE_NUMBER_OF_OUTCOMES_CHANGED    64
#define DSL_RELEVANCE_DISTRIBUTION_CHANGED         128
#define DSL_RELEVANCE_GENERAL_CHANGE               256

#define DSL_NETWORK_DEFAULT (DSL_USE_RELEVANCE | DSL_PROPAGATE_EVIDENCE)

#define DSL_RELEVANCE_STRUCTURE_CHANGED (DSL_RELEVANCE_ARC_ADDED | DSL_RELEVANCE_ARC_REMOVED | DSL_RELEVANCE_NUMBER_OF_OUTCOMES_CHANGED)
#define DSL_RELEVANCE_DEFVAL_CHANGED    (DSL_RELEVANCE_EVIDENCE_SET | DSL_RELEVANCE_EVIDENCE_CLEARED | DSL_RELEVANCE_DISTRIBUTION_CHANGED)

#define DSL_RELEVANCE_DEFAULT           (DSL_RELEVANCE_STRUCTURE_CHANGED | DSL_RELEVANCE_DEFVAL_CHANGED | DSL_RELEVANCE_GENERAL_CHANGE)

#define DSL_NOISY_DECOMP_LIMIT_DEFAULT 4

#define DSL_NETWORK_COPY_SKIP_DOCS 1
#define DSL_NETWORK_COPY_SKIP_DBN 2
#define DSL_NETWORK_COPY_SKIP_CASES 4
#define DSL_NETWORK_COPY_SKIP_UPDATE_LISTENER 8
#define DSL_NETWORK_COPY_SKIP_ALL 0xFFFF

class DSL_network
{
public:
	DSL_network();
	DSL_network(const DSL_network &src, int skipFlags = 0);  
	~DSL_network();

	DSL_network& operator=(const DSL_network& likeThisOne);
	int Copy(const DSL_network& src, int skipFlags = 0);

	void Clear();

	int GetFirstNode() const;
	int GetNextNode(int handle) const;
	int GetLastNode() const;

	DSL_node* GetNode(int handle) { return Ok(handle) ? nodes[handle] : NULL; }
	const DSL_node* GetNode(int handle) const { return Ok(handle) ? nodes[handle] : NULL; }

	DSL_node* GetNode(const char* nodeId) { int h = FindNode(nodeId); return h < 0 ? NULL : nodes[h]; }
	const DSL_node* GetNode(const char* nodeId) const { int h = FindNode(nodeId); return h < 0 ? NULL : nodes[h]; } 

	DSL_submodelHandler& GetSubmodelHandler() { return *submodels; }
	const DSL_submodelHandler& GetSubmodelHandler() const { return *submodels; }

	const DSL_intArray& GetParents(int nodeHandle) const { assert(Ok(nodeHandle)); return nodes[nodeHandle]->parents; }
	const DSL_intArray& GetChildren(int nodeHandle) const { assert(Ok(nodeHandle)); return nodes[nodeHandle]->children; }

	DSL_header& Header();
	const DSL_header& Header() const;
	DSL_userProperties& UserProperties() { return userProperties; }
	const DSL_userProperties& UserProperties() const { return userProperties; }
	DSL_documentation& Documentation() { return documentation; }
	const DSL_documentation& Documentation() const { return documentation; }

	// inference algorithm selection / control
	void SetDefaultBNAlgorithm(int theAlgorithm);
	void SetDefaultIDAlgorithm(int theAlgorithm);
	int GetDefaultBNAlgorithm() const { return defaultBNalg; }
	int GetDefaultIDAlgorithm() const { return defaultIDalg; }
    
	bool IsUpdateImmediate() const { return 0 != IsNetworkFlagSet(DSL_UPDATE_IMMEDIATELY); }
	int SetUpdateImmediate(bool immediate);

	void EnableNoisyDecomp(bool enable) { noisyDecompEnabled = enable; }
	void SetNoisyDecompLimit(int limit) { noisyDecompLimit = limit;  }
	bool IsNoisyDecompEnabled() const { return noisyDecompEnabled; }
	int GetNoisyDecompLimit() const { return noisyDecompLimit; }

	bool IsJptStoreEnabled() const { return NULL != jptContainer; }
	void EnableJptStore(bool enable);
	int GetJpts(int ofThisNode, std::vector<std::pair<std::vector<int>, const DSL_Dmatrix *> > & jpts) const;
	void RemoveJpts();
	DSL_Dmatrix& InitJpt(const std::vector<int> &nodes);

    const DSL_extFunctionFactory* GetExtFunctionFactory() const;
    DSL_extFunctionContainer* GetExtFunctionContainer();
    void GetExtFunctions(std::vector<std::string> &functions, bool includeDistributions = true) const;
    int SetExtFunctions(const std::vector<std::string> &functions, int &errFxnIdx, int &errPos, std::string &errMsg);
    DSL_extFunctionContainer* ValidateExtFunctions(const std::vector<std::string> &functions, int &errFxnIdx, int &errPos, std::string &errMsg);

	int GetNumberOfSamples() const { return numSamples; }
	int SetNumberOfSamples(int aNumber);
	void EnableRejectOutlierSamples(bool enable) { rejectOutliers = enable;}
	bool IsRejectOutlierSamplesEnabled() const { return rejectOutliers; }
	int GetNumberOfDiscretizationSamples() const { return numDiscSamples; }
	int SetNumberOfDiscretizationSamples(int aNumber);
	void EnableZeroAvoidance(bool enable) { zeroAvoidanceEnabled = enable; }
	bool IsZeroAvoidanceEnabled() const { return zeroAvoidanceEnabled; }
	void SetRandSeed(int seed) { randSeed = seed; }
	int GetRandSeed() const { return randSeed; }
	DSL_randGen* GetRandGen();
	void RestartRandGen();

	int GetNumberOfNodes() const { return numNodes; }
	int AddNode(int nodeType, const char *nodeId);  
	int DeleteNode(int handle);
	int DeleteAllNodes(); 
	int AddArc(int theParent, int theChild, dsl_arcType layer = dsl_normalArc);
	int RemoveArc(int theParent, int theChild, dsl_arcType layer = dsl_normalArc);
	int ReverseArc(int theParent, int theChild);
	int IsArcNecessary(int parent, int child, double epsilon, bool &necessary) const;
	int RemoveAllArcs();
	int MarginalizeNode(int thisNode, DSL_progress *progress = NULL);
  
	int MakeUniform(const std::vector<int> *nodeFilter = NULL);
    int MakeRandom(DSL_randGen *extRandGen = NULL, const std::vector<int> *nodeFilter = NULL);

	int GetNumberOfTargets() const { return numTargets; }
	int IsTarget(int nodeHandle) const;
	int SetTarget(int nodeHandle, bool target = true);
	int ClearAllTargets();

	int ClearAllEvidence();
	int ClearAllDecisions();
	int ClearAllPropagatedEvidence();
	int IsThereAnyEvidence() const;
	int IsThereAnyDecision() const;
	int Ok(int handle) const { return handle >= 0 && handle < (int)nodes.size() && nodes[handle] != NULL; }
	int Related(int aNode, int anotherNode) const;
	int GetAncestors(int theNode, DSL_intArray &here); 
	int GetDescendants(int theNode, DSL_intArray &here); 
	int GetAllNodeIds(DSL_idArray &here) const;
	int GetAllNodes(DSL_intArray &here) const;
	int GetAllEvidenceNodes(DSL_intArray &here);
	int IsThisIdentifierInUse(const char *id, int handleToSkip = -1) const;
	int FindNode(const char *withThisID) const;
	int IsPolyTree();
	int InDegree() const;
	int IsAcyclic();
	bool IsInfluenceDiagram() const;
	const DSL_intArray& PartialOrdering() const;

	// input/output
	int ReadFile(const char *thisFile, int fileType = 0, void *ext = NULL);
	int WriteFile(const char *thisFile, int fileType = 0, void *ext = NULL);
  
	int ReadString(const char *xdslString, void *ext = NULL);
	int WriteString(std::string &xdslOutputString, void *ext = NULL);

	// to improve load performance of large models, 
	// we postpone some consistency checks during load
	bool IsLoading() const { return 0 != loadTimeFlags; }

	int CreateUniqueNodeIdentifier(char *here);
	DSL_errorStringHandler &ErrorHandler();

	// inference
	int UpdateBeliefs();
	int InvalidateAllBeliefs();
	int CallIDAlgorithm();
	int CallBNAlgorithm();

	void SetUpdateBeliefsListener(DSL_updateBeliefsListener *listener);

	void ActivateRelevance() { networkFlags |= DSL_USE_RELEVANCE; }
	void DeactivateRelevance() { networkFlags &= ~DSL_USE_RELEVANCE; }
	bool IsRelevanceActive() const { return 0 != (networkFlags & DSL_USE_RELEVANCE); } 

	void SetRelevanceLevelFlag(int thisFlag) { relevanceFlags |= thisFlag; }
	void ClearRelevanceLevelFlag(int thisFlag) { relevanceFlags &= ~thisFlag; }
	int IsRelevanceLevelSet(int thisFlag) { return relevanceFlags & thisFlag; }
	int GetRelevanceLevelFlags() { return relevanceFlags; }
	void SetRelevanceLevelFlags(int thisFlags) { relevanceFlags = thisFlags; }

	void SetNetworkFlag(int thisFlag) { networkFlags |= thisFlag; }
	void ClearNetworkFlag(int thisFlag) { networkFlags &= ~thisFlag; }
	int IsNetworkFlagSet(int thisFlag) const { return networkFlags & thisFlag; }
	int GetNetworkFlags(void) const { return networkFlags; }

	int ValueOfInformation(DSL_valueOfInformation &here);
	int TemporalOrder(DSL_intArray &here);
	int OrderPartially(DSL_intArray &theseNodes);
	int OrderTemporally(DSL_intArray &theseNodes);

	const DSL_intArray& GetCostParents(int nodeHandle) const { assert(Ok(nodeHandle)); return nodes[nodeHandle]->GetCostParents(); }
	const DSL_intArray& GetCostChildren(int nodeHandle) const { assert(Ok(nodeHandle)); return nodes[nodeHandle]->GetCostChildren(); }
	void CostPartialOrdering(DSL_intArray &here);
	int OrderPartiallyByCost(DSL_intArray &theseNodes);
	int GetCostDescendants(int nodeHandle, DSL_intArray &here); 
	bool IsGroupCost(int node) const;

	int ChangeOrderOfParents(int nodeHandle, const DSL_intArray &newOrder);

	int SetEPISParams(const DSL_EPISParams &p);
	const DSL_EPISParams& GetEPISParams() const { return episParams;  }

	int SetLBPParams(const DSL_LBPParams &p);
	const DSL_LBPParams& GetLBPParams() const { return lbpParams; }

	int SetAnnealedMAPParams(const DSL_AnnealedMAPParams &p);
	const DSL_AnnealedMAPParams& GetAnnealedMAPParams() const { return annealedMAPParams; }
	int AnnealedMAP(
		const std::vector<std::pair<int,int> > &evidNodes,
		const std::vector<int> &mapNodes,
		std::vector<int> &mapStates, double &probM1E, double &probE, int randSeed = 0);

	int GetDepthOfNet() const;

	bool CalcProbEvidence(double &pe, bool forceChainRule = false);
	bool CalcProbEvidence_JoinTree(double &pe);
	bool CalcProbEvidence_ChainRule(double &pe);

    int CalcConfidenceIntervals(
        const std::vector<int> &nodeHandles, const DSL_instanceCounts &counts, double percentage, int iterations,
        std::vector<std::vector<std::pair<double, double> > > &intervals);


	// if params is NULL, defaults will be used
	void SimpleGraphLayout(const DSL_SimpleGraphLayoutParams *params = NULL);

	// case management added by Mark on 02-22-2005
	DSL_case * AddCase(const std::string & name);
	DSL_case * GetCase(int index) const;
	DSL_case * GetCase(const std::string & name) const;
	int DeleteCase(int index);
	void DeleteAllCases();
	int GetNumberOfCases() const;
	void EnableSyncCases(bool sync);
	bool IsEnableSyncCases() const;

	// dynamic network support
    bool HasTemporalNodes() const;
	int UnrollNetwork(DSL_network &unrolled, std::vector<int> &unrollMap) const;
	int GetMaxTemporalOrder() const;
	int GetNumberOfSlices() const;
	int SetNumberOfSlices(int slices);
	int AddTemporalArc(int parent, int child, int order);
	int RemoveTemporalArc(int parent, int child, int order);
	bool TemporalArcExists(int parent, int child, int order) const;
	int IsTemporalArcNecessary(int parent, int child, int order, double epsilon, bool &necessary) const;
	dsl_temporalType GetTemporalType(int nodeHandle) const;
	int SetTemporalType(int nodeHandle, dsl_temporalType type);
	int GetMaxTemporalOrder(int nodeHandle) const;
	int GetTemporalOrders(int nodeHandle, DSL_intArray &orders) const;
	int GetTemporalChildren(int parent, std::vector<std::pair<int, int> > &children) const;
	int GetTemporalParents(int child, int order, DSL_intArray &parents) const;
	int FindTemporalParentPosition(int parent, int child, int order) const;
	int GetUnrolledParents(int child, int order, std::vector<std::pair<int, int> > &parents) const;
	int GetUnrolledParents(int child, std::vector<std::pair<int, int> > &parents) const;
	DSL_dbnImpl* GetDbn() { return dbn; } 
	// DBN - end

	// methods that are forwarded to the case manager and dbn implementation
	// to handle changes in the network, only for internal use for methods in nodedef.h!!!
	void OnTypeChanged(int handle, int prevType);
	void OnAddOutcome(int handle, int outcomeIndex);
	void OnDeleteOutcome(int handle, int outcomeIndex);
	void OnReorderOutcomes(int handle, const DSL_intArray &newOrder);
	int OnNodeIdChanging(int handle, const char* oldId, const char* newId);

	void GetBuild(std::string &build) const;  
	void GetTopComment(std::string &comment) const;

	// these methods should not be called directly,
	// they are called by node definition/value classes
	int RelevanceControlValue(int thisNode);
	int RelevanceClearControlledValue(int thisNode);
	int RelevanceEvidenceSet(int thisNode);
	int RelevanceEvidenceCleared(int thisNode);
	int RelevanceContEvidenceChanged(int thisNode);
	int RelevanceVirtualEvidenceSet(int thisNode);
	int RelevanceVirtualEvidenceCleared(int thisNode);
	int RelevanceArcRemoved(int theParent, int theChild);
	int RelevanceArcAdded(int theParent, int theChild);
	int RelevanceDefinitionChanged(int thisNode, int changeType);
	int RelevanceEvidencePropagated(int thisNode);
	int RelevancePropagatedEvidenceCleared(int thisNode);

private:
	void Create();
	void Destroy();

	DSL_fileFormat* CreateFileFormat(const char* thisFile, int& fileType);
	int BeginRead(DSL_fileFormat* reader);
	void EndRead(int readResult, int oldRelevanceFlags);

	int GetNodeContainerSize() const { return (int)nodes.size(); }

	int DoUpdateBeliefs();

	void PartialOrderHelper(int handle) const;
	void InvalidatePartialOrder();
	void MarkAncestors(int nodeHandle);
	void MarkDescendants(int nodeHandle);
	void MarkCostDescendants(int nodeHandle);
	void AcyclicVisitChildren(int thisNode, int *visited);
	int VisitNeighbor(int thisNode, int fromThisNode);
	bool RemainsAcyclic(int theParent, int theChild);

	void CleanTraversalFlag(int flag) const;

	void CreateCoordinates(int nodeHandle, int nodeState, DSL_intArray& coords) const;

	void InitializeNodeValues();
	bool RelevanceProlog(int flag) const { return IsRelevanceActive() && (0 != (relevanceFlags & flag)); }
	int NodeChanged(int affectedNode);
	void InvalidateControl(int aNode, int fromWhere, int &invalidUtility);
	void InvalidateDecisions();
	int FunctionDetermine(int anEvidenceNode);
	void RecursiveMarkRelevant(int thisNode);
	int ReverseArcHelper(int theParent, int theChild);
	int AddObservCostArc(int theParent, int theChild);
	int RemoveObservCostArc(int theParent, int theChild);
	void CostPartialOrderHelper(int aNode, DSL_intArray &here);
	DSL_dbnImpl& Dbn() const;

	std::vector<DSL_node*> nodes;
	int numNodes;        // nodes vector can have NULL entries
	int numTargets;      // number of target nodes in this network
	int defaultBNalg;    // Default belief network algorithm
	int defaultIDalg;    // Default influence diagram algorithm
	int randSeed;        // seed to initialize randgen before sampling starts, 0 = use clock etc.
	int numSamples;      // number of samples for sampling algorithms
	int numDiscSamples;  // number of samples for discretization
	bool zeroAvoidanceEnabled;
	bool rejectOutliers; // controls the sample rejection policy for continuous nets
	bool noisyDecompEnabled;
	int noisyDecompLimit;

	int networkFlags;
	int relevanceFlags;
	int loadTimeFlags;

	friend class DSL_updateImmediateContext;
	friend class DSL_updateGuard;
	bool inUpdate;
	int immediateUpdateCounter;

    mutable bool partialOrderValid;
    mutable DSL_intArray partialOrder;

	DSL_userProperties userProperties;
	DSL_documentation documentation;
    DSL_EPISParams episParams;
    DSL_LBPParams lbpParams;
    DSL_AnnealedMAPParams annealedMAPParams;

	DSL_submodelHandler *submodels;
    DSL_randGen *randGen;
    DSL_caseManager *caseManager;
    DSL_dbnImpl *dbn;
    DSL_jptContainer *jptContainer;
    DSL_extFunctionContainer *extFxnContainer;
    DSL_updateBeliefsListener *updateBeliefsListener;
};

#endif 
