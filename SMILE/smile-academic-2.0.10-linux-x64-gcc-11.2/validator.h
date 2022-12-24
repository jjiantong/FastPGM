#ifndef SMILE_VALIDATOR_H
#define SMILE_VALIDATOR_H

// {{SMILE_PUBLIC_HEADER}}

class DSL_progress;
class DSL_em;
class DSL_network;
class DSL_dataset;
struct DSL_datasetWriteParams;
struct DSL_datasetMatch;

#include <vector>
#include <utility>

class DSL_validator
{
public:
	DSL_validator(
		DSL_dataset& ds, const DSL_network &net, 
		const std::vector<DSL_datasetMatch> &matching, 
		const std::vector<int> *fixedNodes = 0);

	int AddClassNode(int classNodeHandle);

	int Test(DSL_progress *progress = 0);
	int KFold(DSL_em &em, int foldCount, int randSeed = 0, DSL_progress *progress = 0);
	int LeaveOneOut(DSL_em &em, DSL_progress *progress = 0);

	int GetPosteriors(int classNodeHandle, int recordIndex, std::vector<double> &posteriors) const;
	int GetAccuracy(int classNodeHandle, int outcome, double &acc) const;
	int GetConfusionMatrix(int classNodeHandle, std::vector<std::vector<int> > &matrix) const;
	int GetPredictedOutcome(int classNodeHandle, int recordIndex) const;
	int GetPredictedNode(int recordIndex) const;
	int GetPredictedNodeIndex(int recordIndex) const;
	int GetFoldIndex(int recordIndex) const;

	int CalibrateByBinning(int classNodeHandle, int outcomeIndex, int binCount, std::vector<std::pair<double, double> > &curve, double &hosmerLemeshTest) const;
	int CalibrateByMovingAverage(int classNodeHandle, int outcomeIndex, int windowSize, std::vector<std::pair<double, double> > &curve) const;
	int CreateROC(int classNodeHandle, int outcomeIndex, std::vector<std::pair<double, double> > &curve, std::vector<double> &thresholds, double &auc) const;

	void GetResultDataset(DSL_dataset &output) const;

    int ReuseResults();

private:
	int InitOutput();
	int InstantiateRecord(DSL_network &net, int recordIndex);
	int GetClassNodeIndex(int nodeHandle) const;
	int GetPosteriorsPosition(int classNodeIndex, int recordIndex) const;
	int GetOutcomeCount(int classNodeIndex) const;
	int Predict(int classNodeIndex, int recordIndex) const;

	int BeginCalibration(int classNodeHandle, int outcomeIndex, int &classNodeIndex, int &varIndex, int &firstPosteriorPos, std::vector<int> &indices) const;

	//int BinCalibrate(int varIndex, int classNodeIndex, int outcomeIndex, int binCount, std::vector<std::pair<double, double> > &curve) const;
	int MovAvgCalibrate(int varIndex, int classNodeIndex, int outcomeIndex, int windowSize, std::vector<std::pair<double, double> > &curve) const;

	const DSL_network &origNet;
	DSL_dataset &ds;
	std::vector<DSL_datasetMatch> matching;
	std::vector<int> fixedNodes;
	std::vector<int> classNodes;
	std::vector<int> posteriorPos;
	std::vector<int> folds;
	int posteriorsPerRecord;
	std::vector<double> posteriors;
	std::vector<std::vector<std::vector<int> > > results;
};

#endif
