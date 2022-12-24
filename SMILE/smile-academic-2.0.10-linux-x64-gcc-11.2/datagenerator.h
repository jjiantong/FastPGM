#ifndef SMILE_DATAGENERATOR_H
#define SMILE_DATAGENERATOR_H

// {{SMILE_PUBLIC_HEADER}}

#include <vector>
#include <string>
#include <cstdio>

class DSL_network;
class DSL_dataGeneratorOutput;
class DSL_dataset;
class DSL_randGen;
struct DSL_datasetWriteParams;

class DSL_dataGenerator
{
public:
	DSL_dataGenerator(DSL_network &net);
	~DSL_dataGenerator();

	// generalized output function
	int GenerateData(DSL_dataGeneratorOutput &out);
	
	// convenience methods implemented in terms of subclassed 
	// DSL_dataGeneratorOutput objects, see class declarations below
	int GenerateData(const char *filename, const DSL_datasetWriteParams *params = NULL);
	int GenerateData(DSL_dataset &ds);

	int GetNumberOfRecords() const { return recordCount; }
	void SetNumberOfRecords(int numrec) { recordCount = numrec; }

	// defaults to zero, meaning seed taken from system clock
	int GetRandSeed() const { return randSeed; }
	void SetRandSeed(int seed) { randSeed = seed; InitRandGen(); }

	// defaults to no missing values
	int GetMissingValuePercent() const { return missingValuePercent; }
	void SetMissingValuePercent(int perc) { missingValuePercent = perc; }

	// defaults to false
	bool GetBiasSamplesByEvidence() const { return biasSamplesByEvidence; }
	void SetBiasSamplesByEvidence(bool bias) { biasSamplesByEvidence = bias; }
	
	// by default no selection, meaning all nodes in the network are included
	const std::vector<int> GetSelectedNodes() { return selectedNodes; }
	int SetSelectedNodes(const std::vector<int> &selection);

    bool MissingDataCheck();

private:
	int GenerateDiscreteData(DSL_dataGeneratorOutput &out, DSL_network &net, const std::vector<int> &outputNodes);
	int GenerateContinuousData(DSL_dataGeneratorOutput &out, DSL_network &net, const std::vector<int> &outputNodes);

	int DoGenerateData(DSL_dataGeneratorOutput &out, DSL_network &net, const std::vector<int> &selNodes);
	int GenerateDataOnUnrolledDbn(DSL_dataGeneratorOutput &out);

	void InitRandGen();

	void FindOutputNodes(std::vector<int> &outputNodes);

	DSL_network &origNet;
	std::vector<int> selectedNodes;
	int recordCount;
	int randSeed;
	int missingValuePercent;
	bool biasSamplesByEvidence;

	DSL_randGen *randGen;
};


class DSL_dataGeneratorOutput
{
public:
	virtual int Open(DSL_network &net, const std::vector<int> &outputNodes, int recordCount) = 0;
	virtual void PreRecord(int recIdx) {}
	virtual void WriteDiscreteValue(int recIdx, int colIdx, int nodeHandle, int outcomeIdx) {}
	virtual void WriteContinuousValue(int recIdx, int colIdx, int nodeHandle, float value) {}
	virtual void WriteMissingValue(int recIdx, int colIdx, int nodeHandle) {}
	virtual int PostRecord(int recIdx); 
	virtual int Close();

	// not guaranteed to be called after each valid record, use only for UI progress/cancel
	virtual bool Progress(int sampleCount, int rejectedCount) { return true; }
};


class DSL_dataGeneratorFileOutput : public DSL_dataGeneratorOutput
{
public:
	DSL_dataGeneratorFileOutput(const char *filename, const DSL_datasetWriteParams *writeParams = NULL);
	~DSL_dataGeneratorFileOutput();

	virtual int Open(DSL_network &net, const std::vector<int> &outputNodes, int recordCount);
	virtual void WriteDiscreteValue(int recIdx, int colIdx, int nodeHandle, int outcomeIdx);
	virtual void WriteContinuousValue(int recIdx, int colIdx, int nodeHandle, float value);
	virtual void WriteMissingValue(int recIdx, int colIdx, int nodeHandle);
	virtual int PostRecord(int recIdx);
	virtual int Close();

private:
	FILE *fout;
	std::string filename;
	DSL_datasetWriteParams *writeParams; 
	bool useStateNames;
	DSL_network *net;
};

class DSL_dataGeneratorSetOutput : public DSL_dataGeneratorOutput
{
public:
	DSL_dataGeneratorSetOutput(DSL_dataset &ds);

	virtual int Open(DSL_network &net, const std::vector<int> &outputNodes, int recordCount);
	virtual void WriteDiscreteValue(int recIdx, int colIdx, int nodeHandle, int outcomeIdx);
	virtual void WriteContinuousValue(int recIdx, int colIdx, int nodeHandle, float value);

private:
	DSL_dataset &ds;
};


#endif
