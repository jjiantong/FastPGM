#ifndef SMILE_INSTANCECOUNTS_H
#define SMILE_INSTANCECOUNTS_H

// {{SMILE_PUBLIC_HEADER}}

#include <vector>
#include "dataset.h"

class DSL_instanceCounts
{
public:
    int Calculate(const DSL_network &net, const DSL_dataset &ds, const std::vector<DSL_datasetMatch> &matching);
    int SetUniform(const DSL_network &net, int uniformCount);
    int Override(int nodeHandle, int count);

    const std::vector<int>& Get(int nodeHandle) const { return counts[nodeHandle]; }

private:
    int Init(const DSL_network &net);

    std::vector<std::vector<int> > counts;
};

#endif
