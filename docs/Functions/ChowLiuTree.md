# class ChowLiuTree

## double ComputeMutualInformation(Node \*Xi, Node \*Xj, const Trainer \*trainer)
Compute the mutual information between two nodes on a training set. The formula is
```
LaTeX:
$$ I(X,Y) = \sum_{x \in X, y \in Y} p(x,y) log \frac{p(x,y)}{p(x)p(y)} $$
```

## void StructLearnCompData(Trainer \*trainer)
Call function `StructLearnChowLiuTreeCompData`.

## void StructLearnChowLiuTreeCompData(Trainer \*trainer)
Structure learning with complete data by constructing a Chow-Liu tree. First, construct the mutual information matrix. Second, use Prim's algorithm to generate a maximum-spanning tree. Third, add arrows in the spanning tree, which is to set the parents and children of each node.

## pair<int\*, int> SimplifyDefaultElimOrd(Combination evidence) override
Call function `SimplifyTreeDefaultElimOrd`.

## pair<int\*, int> SimplifyTreeDefaultElimOrd(Combination evidence)
The default elimination ordering of a Chow-Liu tree is the reversed topological ordering of the tree. Given some evidence (observation), some nodes in default elimination will be summed out while not contribute any useful information. Thus, we can simply remove these nodes. The algorithm is implemented according to the paper *[Zhang and Poole - 1994 - A simple approach to Bayesian network computations.pdf]*. First, remove all barren nodes. Second, remove all m-separated nodes.

## void DepthFirstTraversalUntillMeetObserved(Combination evidence, int start, set<int>& visited, set<int>& to_be_removed)
Used by function `SimplifyTreeDefaultElimOrd` in the process of removing all m-separated nodes.

## void DepthFirstTraversalToRemoveMSeparatedNodes(int start, set<int>& visited, set<int>& to_be_removed)
Used by function `DepthFirstTraversalUntillMeetObserved` in the process of removing all m-separated nodes.


