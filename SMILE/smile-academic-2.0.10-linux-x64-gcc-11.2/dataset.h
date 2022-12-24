#ifndef SMILE_DATASET_H
#define SMILE_DATASET_H

// {{SMILE_PUBLIC_HEADER}}

#include <vector>
#include <string>
#include <cmath>
#include <cassert>
#include <cstdio>

class DSL_network;
class DSL_node;

#define DSL_MISSING_INT		(-1)
#define DSL_MISSING_FLOAT ((float)sqrt(-1.0))

struct DSL_datasetVarInfo
{
	DSL_datasetVarInfo() : discrete(true), missingInt(DSL_MISSING_INT) {}
	bool discrete;
	std::string id;
	int missingInt;
	float missingFloat;
	std::vector<std::string> stateNames;
};
 
struct DSL_datasetParseParams
{
    DSL_datasetParseParams() : 
		missingValueToken("*"), 
		missingInt(DSL_MISSING_INT), 
		missingFloat(DSL_MISSING_FLOAT),  
		columnIdsPresent(true) {}
    std::string missingValueToken;
    int missingInt;
    float missingFloat;
	bool columnIdsPresent;
};

struct DSL_datasetWriteParams
{
	DSL_datasetWriteParams() : 
		missingValueToken("*"), 
		columnIdsPresent(true),
		useStateIndices(false),
		separator('\t'),
		floatFormat("%g") {}

	std::string missingValueToken;
	bool columnIdsPresent;
	bool useStateIndices;
	char separator;
	std::string floatFormat;
};


struct DSL_datasetMatch
{
	DSL_datasetMatch() : node(-1), slice(0), column(-1) {}
    int node;
	int slice;
    int column;
};

#ifdef NDEBUG
	#define DS_VALIDATE_IDX(var, rec) 
#else
	#define DS_VALIDATE_IDX(var, rec) ValidateIdx(var, rec)
#endif

class DSL_dataset 
{
public:
	DSL_dataset();
	DSL_dataset(const DSL_dataset &src);
	DSL_dataset(const DSL_dataset &src, const std::vector<int> &varFilter);
	DSL_dataset& operator=(const DSL_dataset &src);
	~DSL_dataset() { FreeData(); }
	
	int ReadFile(const std::string &filename, const DSL_datasetParseParams *params = NULL, std::string *errOut = NULL);
	int WriteFile(const std::string &filename, const DSL_datasetWriteParams *params = NULL, std::string *errOut = NULL) const;

	// MatchNetwork may change the integer indices in the dataset to ensure
	// the correct fit to outcome ids in the network - hence the method isn't const
	int MatchNetwork(const DSL_network &net, std::vector<DSL_datasetMatch> &matching, std::string &errMsg);

	void PrepareStructure(DSL_network &net);

	int AddIntVar(const std::string id = std::string(), int missingValue = DSL_MISSING_INT); 
	int AddFloatVar(const std::string id = std::string(), float missingValue = DSL_MISSING_FLOAT); 
	int RemoveVar(int var);
	void AddEmptyRecord();
	void SetNumberOfRecords(int numRecords);
	int RemoveRecord(int rec);

	int FindVariable(const std::string &id) const;

	int GetNumberOfVariables() const { return int(metadata.size()); }
	int GetNumberOfRecords() const
	{
		return filter == NULL ? GetRealRowCount() : int(filter->size());
	}

	int ApplyFilter(const std::vector<int> &filter);
	void ClearFilter();

	int GetInt(int var, int rec) const 
	{ 
		DS_VALIDATE_IDX(var, rec);
		assert(IsDiscrete(var));
		return (*static_cast<const std::vector<int> *>(data[var]))[RealIdx(rec)];
	}

	float GetFloat(int var, int rec) const 
	{ 
		DS_VALIDATE_IDX(var, rec);
		assert(!IsDiscrete(var));
		return (*static_cast<const std::vector<float> *>(data[var]))[RealIdx(rec)];
	}

	void SetInt(int var, int rec, int value)
	{
		DS_VALIDATE_IDX(var, rec);
		assert(IsDiscrete(var));
		(*static_cast<std::vector<int> *>(data[var]))[RealIdx(rec)] = value;
	}

	void SetFloat(int var, int rec, float value)
	{
		DS_VALIDATE_IDX(var, rec);
		assert(!IsDiscrete(var));
		(*static_cast<std::vector<float> *>(data[var]))[RealIdx(rec)] = value;
	}
	
	void SetMissing(int var, int rec)
	{
		if (IsDiscrete(var))
		{
			SetInt(var, rec, metadata[var].missingInt);
		}
		else
		{
			SetFloat(var, rec, metadata[var].missingFloat);
		}
	}

	int GetMissingInt(int var) const
	{
		assert(IsDiscrete(var));
		return metadata[var].missingInt;
	}

	float GetMissingFloat(int var) const
	{
		assert(!IsDiscrete(var));
		return metadata[var].missingFloat;
	}

	bool IsMissing(int var, int rec) const 
	{ 
		const DSL_datasetVarInfo &m = metadata[var];
		return m.discrete ? 
			GetInt(var, rec) == m.missingInt :
			IsMissingFloat(var, rec, m.missingFloat);
	}

	bool IsDiscrete(int v) const { return metadata[v].discrete; }
	
	const std::vector<int> & GetIntData(int var) const
	{
		assert(IsDiscrete(var));
		return *static_cast<const std::vector<int> *>(data[var]);
	}

	const std::vector<float> & GetFloatData(int var) const
	{
		assert(!IsDiscrete(var));
		return *static_cast<const std::vector<float> *>(data[var]);
	}

	const DSL_datasetVarInfo& GetVariableInfo(int var) const { return metadata[var]; }
	const std::string& GetId(int var) const { return metadata[var].id; }
	int SetId(int v, const std::string &newId);
	
	const std::vector<std::string>& GetStateNames(int var) const 
	{
		assert(metadata[var].discrete);
		return metadata[var].stateNames;
	}

	int SetStateNames(int var, const std::vector<std::string> &stateNames);

	int GetMinMaxInt(int var, int &minval, int &maxval) const;
	int GetMinMaxFloat(int var, float &minval, float &maxval) const;

	int Get(int var, int rec, int &val) const;
	int Get(int var, int rec, float &val) const;
	
	bool HasMissingData(int var) const;
	bool IsConstant(int var) const;

	enum DiscretizeAlgorithm { Hierarchical, UniformWidth, UniformCount };
	int Discretize(int var, DiscretizeAlgorithm alg, int intervals, const std::string &statePrefix, std::vector<double> &edges);
	int Discretize(int var, DiscretizeAlgorithm alg, int intervals, const std::string &statePrefix);

private:
    int TranslateDiscrete(int var, const DSL_node *node, std::string &errMsg);
	bool IsMissingFloat(int var, int rec, float missingFloat) const;
	void FreeData();
	void FreeColData(int var);
    void ValidateIdx(int var, int rec) const;
	int RealIdx(int rec) const
	{
		return filter == NULL ? rec : (*filter)[rec];
	}

	int AddVarHelper(const std::string &id, bool discrete, int missingInt, float missingFloat);
	int GetRealRowCount() const;

	int DoWriteFile(FILE *fout, const DSL_datasetWriteParams &params, std::string &errOut) const;

	std::vector<DSL_datasetVarInfo> metadata;
	std::vector<void *> data;
	std::vector<int>* filter;

    friend class DSL_datasetParser;
};

#endif
