#ifndef SMILE_SUBMODEL_H
#define SMILE_SUBMODEL_H

// {{SMILE_PUBLIC_HEADER}}

#include "network.h"

#define DSL_MAIN_SUBMODEL 0x8000 
#define DSL_DEFAULT_NODE_COLOR 0x00f7f6e5

class DSL_submodel
{
public:
	DSL_submodel(const char* id);

	DSL_header      header;       // id, comment, etc
	DSL_screenInfo  info;         // submodel as icon
	DSL_rectangle   windowpos;    // submodel as window
	int             bkcolor;      // submodel as window (background color)
	std::vector<std::pair<std::string, DSL_rectangle> > comments; // comments on screen;
};


class DSL_submodelEntry
{
public:
	DSL_submodelEntry();
	bool IsEmpty() const;
	bool IsMainSubmodel() const;
	DSL_submodel* submodel;
	int           parent;
};
 

class DSL_submodelHandler
{
public:
	DSL_submodelHandler(DSL_network &network);
	DSL_submodelHandler(DSL_network &network, const DSL_submodelHandler &src);
	~DSL_submodelHandler();

	int IsThisIdentifierInUse(const char *theId);
	int CreateSubmodel(int theParent, const char *thisId);
	int DeleteSubmodel(int theSubmodel);
	int Ok(int index);
	int GetParent(int ofThisSubmodel);
	DSL_submodel *GetSubmodel(int handler);
	int GetFirstSubmodel(void);
	int GetNextSubmodel(int ofThisSubmodel);
	int FindSubmodel(const char *withThisID);
	int MoveSubmodel(int thisSubmodel, int intoThisSubmodel);
	int GetSubmodelsOfManyNodes(DSL_intArray &theNodes, DSL_intArray &here);
	int IsMainSubmodel(int handle) { return handle == DSL_MAIN_SUBMODEL; }
	int PartialOrdering(DSL_intArray &here);
	int GetNumberOfSubmodels(void);
	DSL_screenInfo &GetNodeDefaultValues() { return defaultNodeValues; }
	DSL_screenInfo &GetSubmodelDefaultValues() { return defaultSubmodelValues; }

	// INTERFACE FUNCTIONS (MAKE TRANSLATION IN/OUT)
	int GetIncludedSubmodels(int inThisSubmodel, DSL_intArray &here);
	int GetIncludedNodes(int inThisSubmodel, DSL_intArray &here);
	int GetDeepIncludedSubmodels(int inThisSubmodel, DSL_intArray &here);
	int GetDeepIncludedNodes(int inThisSubmodel, DSL_intArray &here);
	int GetParentSubmodels(int ofThisGuy, DSL_intArray &here);
	int GetChildSubmodels(int ofThisGuy, DSL_intArray &here);
	int GetParentNodes(int ofThisGuy, DSL_intArray &here);
	int GetChildNodes(int ofThisGuy, DSL_intArray &here);
	int GetOutsideParentNodes(int ofThisGuy, DSL_intArray &here);
	int GetOutsideChildNodes(int ofThisGuy, DSL_intArray &here);

private:
	int iOk(int index) {return(index>=0 && index<maxSubmodels && !submodels[index].IsEmpty());}; 
	int IsEmpty(int thisSlot) { return(submodels[thisSlot].IsEmpty());};
	int FindEmptySlot(void);
	int Grow(void);
	int IsIncluded(int thisSubmodel, int intoThisSubmodel);
	int CreateUniqueSubmodelIdentifier(char *here, int tryThis);
	int GetNodeSubmodel(int thisNode);
	// INTERNAL FUNCTIONS
	int iCreateSubmodel(int theParent, const char *thisId);
	int iDeleteSubmodel(int theSubmodel);
	int iGetParent(int ofThisSubmodel);
	DSL_submodel *iGetSubmodel(int handler);
	int iGetFirstSubmodel(void);
	int iGetNextSubmodel(int ofThisSubmodel);
	int iFindSubmodel(const char *withThisID);
	int iMoveSubmodel(int thisSubmodel, int intoThisSubmodel);
	int iGetSubmodelsOfManyNodes(const DSL_intArray &theNodes, DSL_intArray &here);
	int FillMainSubmodelDefaultValues(void);
	int iFillDefaultValues(int intoThisSubmodel, int fromThisSubmodel);
	/////////   RELATIONSHIPS
	int iGetParentSubmodelsOfNode(int ofThisGuy, DSL_intArray &here);
	int iGetIncludedSubmodels(int inThisSubmodel, DSL_intArray &here);
	int iGetIncludedNodes(int inThisSubmodel, DSL_intArray &here);
	int iGetDeepIncludedSubmodels(int inThisSubmodel, DSL_intArray &here);
	int iGetDeepIncludedNodes(int inThisSubmodel, DSL_intArray &here);
	// parent submodels
	int iGetParentSubmodelsOfSubmodel(int theSubmodel, DSL_intArray &here);
	// child submodels
	int iGetChildSubmodelsOfNode(int theNode, DSL_intArray &here);
	int iGetChildSubmodelsOfSubmodel(int theSubmodel, DSL_intArray &here);
	// parent nodes
	int iGetParentNodesOfNode(int theNode, DSL_intArray &here);
	int iGetParentNodesOfSubmodel(int theSubmodel, DSL_intArray &here);
	// child nodes
	int iGetChildNodesOfNode(int theNode, DSL_intArray &here);
	int iGetChildNodesOfSubmodel(int theSubmodel, DSL_intArray &here);
	// outside parent nodes
	int iGetOutsideParentNodesOfNode(int theNode, DSL_intArray &here);
	int iGetOutsideParentNodesOfSubmodel(int theSubmodel, DSL_intArray &here);
	// outside child nodes
	int iGetOutsideChildNodesOfNode(int theNode, DSL_intArray &here);
	int iGetOutsideChildNodesOfSubmodel(int theSubmodel, DSL_intArray &here);

	DSL_network& network;  // the network I belong to

	DSL_submodelEntry* submodels;
	int                maxSubmodels;
	int                delta;

	DSL_screenInfo     defaultNodeValues; // default info for node
	DSL_screenInfo     defaultSubmodelValues; // default info for submodel
};

#endif
