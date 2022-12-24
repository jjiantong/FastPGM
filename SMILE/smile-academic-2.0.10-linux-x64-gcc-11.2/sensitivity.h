#ifndef SMILE_SENSITIVITY_H
#define SMILE_SENSITIVITY_H

// {{SMILE_PUBLIC_HEADER}}

#include <vector>
#include <utility>

class DSL_network;
class DSL_Dmatrix;
class DSL_sensRes;


class DSL_sensitivity
{
public:
	DSL_sensitivity();
	~DSL_sensitivity();

	int Calculate(DSL_network &net, bool relevance = true);


	typedef std::pair<int, int> Target; // a target node handle followed by a target outcome index

	double GetMaxSensitivity() const;
	double GetMaxSensitivity(Target target) const;
	double GetMaxSensitivity(int node) const;
	void GetMaxSensitivity(int node, DSL_Dmatrix &maxSens) const;
	double GetMaxSensitivity(int node, Target target) const;

	const DSL_Dmatrix* GetSensitivity(int node, Target target) const;
	
	// For chance nodes, GetCoefficients output is a 4-element vector with a, b, c, d.
	// For probabilitites, the target posterior (tp) is a function of given CPT entry (p):
	//   tp = (a * p + b) / (c * p + d)
	// This applies to BNs and IDs alike.
	//
	// For utilities the relationship is linear, the target utility (tu) is a funciton of 
	// given utility or ALU weight (u): tu = a * u + b
	// The coeffs vector will have only two entries with a and b, respectively.
	void GetCoefficients(int node, Target target, std::vector<const DSL_Dmatrix *> &coeffs) const; 

	void GetTargets(std::vector<Target> &targets) const;

	// used only with influence diagrams
	int SetCurrentConfig(int configIndex);
	int GetCurrentConfig() const;
	bool IsConfigPossible(int configIndex) const;
	const std::vector<int>& GetIndexingNodes() const { return indexingNodes; }
	int GetNumberOfConfigs() const { return (int)allConfigs.size(); }


private:
	void Clear();
	int CalculateInfluenceDiagram(DSL_network &net);

	DSL_sensRes *res; 
	int currentConfigIndex;
	std::vector<DSL_sensRes *> allConfigs;
	std::vector<int> indexingNodes;
};

#endif
