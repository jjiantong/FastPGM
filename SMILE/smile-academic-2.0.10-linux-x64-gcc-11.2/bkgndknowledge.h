#ifndef SMILE_BKGNDKNOWLEDGE_H
#define SMILE_BKGNDKNOWLEDGE_H

// {{SMILE_PUBLIC_HEADER}}

#include <vector>
#include <map>
#include <string>
#include <utility>

class DSL_dataset;

struct DSL_bkgndVarInfo
{
	std::string id;
	std::string comment;
	std::vector<int> position;
};

struct DSL_bkgndKnowledge
{
	typedef std::vector<std::pair<int, int> > IntPairVector;
	IntPairVector forcedArcs;
	IntPairVector forbiddenArcs;
	IntPairVector tiers;

	//Adding new type
	IntPairVector forcedEdges;
	//end new type

	// the data members that follow are defined here only for GeNIe compatibility
	// and do not influence learning algorithms directly. Use MatchData if there's
	// a need to apply knowledge loaded from file to DSL_dataset passed as learning input
	std::vector<DSL_bkgndVarInfo> vars;
	std::vector<int> tierWidths;
	std::map<std::pair<int, int>, std::string> arcComments;

	int ReadFile(const std::string &filename, std::string *errOut = NULL);
	int WriteFile(const std::string &filename, std::string *errOut = NULL);
	
	int FindVariable(const std::string &id) const;
	int GetTier(int var) const;

	// if vars is not empty, this method will modify the indices
	// to match the variable ids in the dataset
	int MatchData(const DSL_dataset &ds, std::string *errOut = NULL);
};

#endif
