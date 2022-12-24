#ifndef SMILE_DIAGSESSION_H
#define SMILE_DIAGSESSION_H

// {{SMILE_PUBLIC_HEADER}}

#include <vector>
#include <algorithm>
#include "intarray.h"

class DSL_network;
class DSL_fastEntropyAssessor;
class DSL_discDef;

// Approaches for multiple fault diagnosis
#define DSL_DIAG_MARGINAL		1
#define DSL_DIAG_INDEPENDENCE	2
#define DSL_DIAG_DEPENDENCE		4

// Selections of combinations to pursue
#define DSL_DIAG_PURSUE_ATLEAST_ONE_COMB	8
#define DSL_DIAG_PURSUE_ONLY_ONE_COMB		16
#define DSL_DIAG_PURSUE_ONLY_ALL_COMB		32

// Marginal Function for the marginal approach
#define DSL_DIAG_MARGINAL_STRENGTH1	64
#define DSL_DIAG_MARGINAL_STRENGTH2	128

#define DSL_DIAG_DEFAULT (DSL_DIAG_INDEPENDENCE | DSL_DIAG_PURSUE_ATLEAST_ONE_COMB)

struct DSL_diagTestInfo
{
	int    test;
	double entropy;
	double cost;
	double strength;
	int observationPriorStartIndex;
	int faultPosteriorsStartIndex;
};

struct DSL_diagFaultState
{
	int node;
	int state;

	bool operator<(const DSL_diagFaultState& rhs) const
	{
		if (node == rhs.node)
			return (state<rhs.state);
		return (node<rhs.node);
	}
};

struct DSL_diagFaultInfo
{
	int nodeHandle;
	int nodeState;
	int faultHandle;
	double posterior;

	bool operator<(const DSL_diagFaultInfo& rhs) const
	{
		return posterior > rhs.posterior;
	}
};

class DSL_diagSession
{
public:
	DSL_diagSession(DSL_network &diagNetwork);  
	~DSL_diagSession();

	DSL_network& GetNetwork() { return net; }
	int RestartDiagnosis();
	void UpdateFaultBeliefs();
	int ComputeTestStrengths(int flags = DSL_DIAG_DEFAULT);

	bool IsDSepEnabled() const { return dsepEnabled; }
	void EnableDSep(bool enable) { dsepEnabled = enable; }

	bool AreQuickTestsEnabled() const { return quickTestsEnabled; }
	void EnableQuickTests(bool enable) { quickTestsEnabled = enable; }

	bool IsDetailedEntropyEnabled() const { return detailedEntropyEnabled; }
	void EnableDetailedEntropy(bool enable) { detailedEntropyEnabled = enable; }
	const std::vector<double>& GetEntropyDetails() const { return entropyDetails; }

	// returns the node handles of unperformed tests
	const DSL_intArray& GetUnperformedTests() const 
	{ 
		return unperformedTests; 
	}

	// returns the array of tests statistics. This array is paired with the
	// one returned by [GetUnperformedTests]
	const std::vector<DSL_diagTestInfo>& GetTestStatistics() const { return testStatistics; }

	// all faults (node/outcome pairs) in the model
	const std::vector<DSL_diagFaultState>& GetFaults() const { return faults; }

	void CalculateRankedFaults(std::vector<DSL_diagFaultInfo> &here, double lower, double upper);
	void CalculateRankedFaults(std::vector<DSL_diagFaultInfo> &here, bool ignoreEvidence = false);

	// sets the ratio in [theNet] as a user-defined property
	void SetEntropyCostRatio(double ratio, double maxRatio);
	double GetEntropyCostRatio() const;
	double GetMaxEntropyCostRatio() const;

	int InstantiateObservation(int thisNode, int toThisOutcome);
	int ReleaseObservation(int thisNode);

	// sets [faultHandle] as the pursued fault
	int GetPursuedFault() const;  
	const DSL_intArray& GetPursuedFaults() const { return pursuedFaults; }

	int SetPursuedFault(int faultIndex);
	int SetPursuedFaults(const DSL_intArray & faultindices);
	int AddPursuedFault(int faultIndex);    
	int DeletePursuedFault(int faultIndex);   

	// traverses the network and set to their default state all those
	// nodes flagged as 'set to default'
	void SetDefaultStates();

	// returns the handle of the most likely fault (assummes 
	// the [theNet] is solved already). To solve the DSL_network 
	// you should call GetNetwork()->UpdateBeliefs();
	int FindMostLikelyFault();
	// returns the handle of the fault [faultNode,faultState]
	int FindFault(int faultNode, int faultState) const;

	// returns true if all mandatories are instantiated...
	bool MandatoriesInstantiated() const;

	void AppendTestStatistics(const DSL_diagTestInfo& ti) { testStatistics.push_back(ti); }
	void AppendEntropyDetails(const double* p, int count);

private:
	int CollectNetworkInfo();
	void CompileModel();
	double GetFromProperty(const char *propertyName, double defaultValue) const;
	void DoCalcRankedFaults(std::vector<DSL_diagFaultInfo> &here, double lower, double upper, bool openSet );
	void InitEntropyDetails();

	const DSL_discDef* GetDiscreteDef(int nodeHandle) const;

	DSL_network &net;
	DSL_fastEntropyAssessor *entAss;

	// contains all ranked observations not observed yet
	DSL_intArray unperformedTests;

	// test results (paired with [unperformedTests])
	std::vector<DSL_diagTestInfo> testStatistics;

	std::vector<DSL_diagFaultState> faults;
	DSL_intArray pursuedFaults; // indexes in [faults]

	std::vector<double> entropyDetails;

	bool dsepEnabled;
	bool quickTestsEnabled;
	bool detailedEntropyEnabled;
};

#endif
