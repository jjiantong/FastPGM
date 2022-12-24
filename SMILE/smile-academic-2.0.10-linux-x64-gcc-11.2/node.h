#ifndef SMILE_NODE_H
#define SMILE_NODE_H

// {{SMILE_PUBLIC_HEADER}}

#include "dmatrix.h"

class DSL_network;
class DSL_nodeDef;
class DSL_nodeVal;

enum dsl_diagType { dsl_diagFault, dsl_diagObservation, dsl_diagAux };

class DSL_nodeHeader : public DSL_header
{
public:
	DSL_nodeHeader(DSL_node& n, const char *id) : DSL_header(id), node(n) {}
	int SetId(const char* id);
private:
	DSL_node& node;
};

class DSL_nodeInfo 
{
public:
	DSL_nodeInfo(DSL_node& node, const char* id);
	DSL_nodeInfo(const DSL_nodeInfo& src, DSL_node* targetNode, int skipFlags);
	const DSL_header& Header() const { return header; }
	const DSL_screenInfo& Screen() const { return screen; }
	const DSL_userProperties& UserProperties() const { return userProperties; }
	const DSL_documentation& Documentation() const { return documentation; }
	DSL_header& Header() { return header; }
	DSL_screenInfo& Screen() { return screen; }
	DSL_userProperties& UserProperties() { return userProperties; }
	DSL_documentation& Documentation() { return documentation; }

private:
	DSL_nodeHeader header;         // id, name, comment, etc
	DSL_screenInfo  screen;         // size, position, color, etc
	DSL_userProperties userProperties; // user-defined properties...
	DSL_documentation documentation;  // documentation for the node 

	friend class DSL_node;
};

struct DSL_cost
{
	DSL_cost();
	DSL_intArray parents;
	DSL_intArray children;
	DSL_Dmatrix cost;
};


class DSL_node
{
public:
	friend class DSL_network;

	int Handle() const { return handle; }
	DSL_network* Network() { return &network; } 
	
	DSL_nodeDef* Def() { return def; }
	const DSL_nodeDef* Def() const { return def; }
	template <class defT> defT* Def() { return static_cast<defT*>(def); }
	template <class defT> defT* Def() const { return static_cast<const defT*>(def); }

	DSL_nodeVal* Val() { return val;}
	const DSL_nodeVal* Val() const { return val; }
	template <class valT> valT* Val() { return static_cast<valT*>(val); }
	template <class valT> valT* Val() const { return static_cast<const valT*>(val); }

	DSL_nodeInfo& Info() { return info; }
	const DSL_nodeInfo& Info() const { return info; }

	const char* GetId() const { return info.header.GetId(); }
	int SetId(const char* newId) { return info.header.SetId(newId); }

	const char* GetName() const { return info.header.GetName(); }
	int SetName(const char *name) { return info.header.SetName(name); }

	const char* GetComment() const { return info.header.GetComment(); }
	int SetComment(const char* name) { return info.header.SetComment(name); }

	int GetSubmodel() const { return submodel; }
	int SetSubmodel(int thisSubmodel);
	int SetSubmodel(const char *thisSubmodel);

	int ChangeType(int newType);

	static int NodeTypeFromName(const char *typeName);
	int ChangeType(const char *typeName);

	dsl_diagType GetDiagType() const;
	int SetDiagType(dsl_diagType diagType);

	bool IsDiagRanked() const;
	bool IsDiagMandatory() const;
	void SetDiagRanked(bool ranked);
	void SetDiagMandatory(bool ranked);

	const char* GetDiagQuestion() const { return diagQuestion; }
	void SetDiagQuestion(const char* question);

	const DSL_Dmatrix& GetCosts() const;
	int SetCosts(const DSL_doubleArray& costs);
	int SetCosts(const DSL_Dmatrix& costs);
	int SetCosts(const std::vector<double>& costs);

#ifndef SMILE_NO_V1_COMPATIBILITY
	// BACKWARD COMPATIBILITY ONLY
	DSL_nodeDef* Definition() { return def; }
	DSL_nodeVal* Value() { return val; }
#endif

private:
	DSL_node(int definitionType, int handle, const char* id, DSL_network& net);
	DSL_node(const DSL_node& src, DSL_network& targetNetwork, int skipFlags);

	~DSL_node();

	void OnAddOutcome(int outcomeIndex);
	void OnDeleteOutcome(int outcomeIndex);
	void OnReorderOutcomes(const DSL_intArray& newOrder);

	int GetMarks() const { return traversalFlags; }
	bool IsMarked(int flag) const { return 0 != (flag & traversalFlags); }
	void SetMark(int flag) { traversalFlags |= flag; }
	void CleanMark(int flag) { traversalFlags &= ~flag; }

	const DSL_intArray& GetCostParents() const;
	const DSL_intArray& GetCostChildren() const;
	void AddCostParent(int parentHandle);
	void RemoveCostParent(int parentHandle);

	DSL_network& network;
	int const handle;
	
	bool isTarget;
	int temporalType;
	int submodel;   // graphical submodel handle
	int diagFlags;  // diag type and mandatory/ranked 
	const char* diagQuestion;

	int traversalFlags; // transient, traversal algorithms only

	DSL_nodeDef* def;
	DSL_nodeVal* val;
	DSL_intArray parents;
	DSL_intArray children;

	DSL_nodeInfo info;
	DSL_cost* cost;
};

#endif

