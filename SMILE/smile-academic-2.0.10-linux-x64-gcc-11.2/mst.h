#ifndef SMILE_MST_H
#define SMILE_MST_H

// {{SMILE_PUBLIC_HEADER}}

#include <vector>
#include <utility>
#include <map>

class DSL_mst
{
public:
	typedef std::pair<int,int> edge;
	typedef std::pair<double,edge> weight;
	virtual std::vector<weight> operator() (std::vector<weight> A, const bool Min=false)=0;
};

class DSL_mst_kruskal : public DSL_mst
{
public:
	virtual std::vector<weight> operator() (std::vector<weight> A, const bool Min=false);
private:
	class DisjointSets
	{
	public:
		void MakeSet(int x);
		int Find(int x);
		void Union(int x, int y);
	private:
		class Node
		{
		public:
			Node():rank(0),parent(0){}
			Node(int r, int p): rank(r),parent(p) {}
			int rank;
			int parent;
		};
		std::map<int,Node> sets;
	};
};

#endif
