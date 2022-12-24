#ifndef SMILE_DISCRETIZER_H
#define SMILE_DISCRETIZER_H

// {{SMILE_PUBLIC_HEADER}}

#include <vector>

class DSL_discretizer
{
public:
	enum MethodType { Hierarchical, UniformWidth, UniformCount };

	DSL_discretizer(const std::vector<float> &data);
	DSL_discretizer(const std::vector<int> &data);
	
	int Discretize(MethodType method, int intervals, std::vector<double> &edges) { return DoDiscretize(method, intervals, &edges, 0); }
	int Discretize(MethodType method, int intervals, std::vector<int> &discretized) { return DoDiscretize(method, intervals, 0, &discretized); }
	int Discretize(MethodType method, int intervals, std::vector<double> &edges, std::vector<int> &discretized) { return DoDiscretize(method, intervals, &edges, &discretized); }

private:
	int DoDiscretize(MethodType method, int intervals, std::vector<double> *edges, std::vector<int> *discretized);

	const std::vector<float> *fdata;
	const std::vector<int> *idata;
};

#endif
