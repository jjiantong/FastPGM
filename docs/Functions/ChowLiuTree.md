# class ChowLiuTree

## double ComputeMutualInformation(Node \*Xi, Node \*Xj, const Dataset \*dts)

Compute the mutual information between two nodes on a training set. The formula is

```LaTeX
(LaTeX)
$$ I(X,Y) = \sum_{x \in X, y \in Y} p(x,y) log \frac{p(x,y)}{p(x)p(y)} $$
```

Time complexity: O( |dom(X)| \* |dom(Y)| \* number of instances in dataset ), where |dom(X)| is the number of possible values of X.

## void StructLearnCompData(Dataset \*trainer)

Call function `StructLearnChowLiuTreeCompData`.

## void StructLearnChowLiuTreeCompData(Dataset \*trainer)

Structure learning with complete data by constructing a Chow-Liu tree.

1. construct the mutual information matrix.
2. use Prim's algorithm to generate a maximum-spanning tree.
3. add arrows in the spanning tree, which is to set the parents and children of each node.

Time complexity: O( n^2 \* time complexity of `ComputeMutualInformation`) for step 1. O(n^2) for step 2, because the data structure is matrix. The time spent on step 3 is negligible comparing with step 1 and 2.

## pair<int\*, int> SimplifyDefaultElimOrd(DiscreteConfig evidence) override

Call function `SimplifyTreeDefaultElimOrd`.

## pair<int\*, int> SimplifyTreeDefaultElimOrd(DiscreteConfig evidence)

The default elimination ordering of a Chow-Liu tree is the reversed topological
ordering of the tree. Given some evidence (observation), some nodes in default
elimination will be summed out while not contribute any useful information.
Thus, we can simply remove these nodes. The algorithm is implemented according to the paper
*[Zhang and Poole - 1994 - A simple approach to Bayesian network computations.pdf]*.
First, remove all barren nodes. Second, remove all m-separated nodes.

## void DepthFirstTraversalUntillMeetObserved(DiscreteConfig evidence, int start, set<int>& visited, set<int>& to_be_removed)

Used by function `SimplifyTreeDefaultElimOrd` in the process of removing all m-separated nodes.

## void DepthFirstTraversalToRemoveMSeparatedNodes(int start, set<int>& visited, set<int>& to_be_removed)

Used by function `DepthFirstTraversalUntillMeetObserved` in the process of removing all m-separated nodes.
